/*
    NB TeraTerm NON inserisce i delay impostabili nel setup serial port durante i file transfer ma
    sembra rispettare lo stopbit
*/
/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/

#define DIVHEADER
#include <STDIO.H>

/* #include <IO.H> */
extern	int	read(), write();

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\XM.H>
#include <A:\MAIN\H-LOCAL\XMP.H>

#include <A:\MAIN\H-LOCAL\JC.H>
#include <A:\MAIN\H-LOCAL\JCIFUNC.H>

extern void opldir();
extern void strcpy();

#include <A:\MAIN\H-LOCAL\UTILS.H>

#define BS_CS128  (3 +  128 + 1)

#include <A:\MAIN\H-LOCAL\XMVARS.H>

void resErr()
{
    e_TmOut = e_Stop = e_Prot = e_UnderRun = e_BadSeq = e_ChkCRC = 0;
}

#ifdef XMDBG
void peBadSeq(nBlk8)
U8 nBlk8;
{
    printf("e_BadSeq %u, expblk %u\n", (U16) e_BadSeq, (size_t) nBlk8);
}

void peUndRun(nBlk8)
U8 nBlk8;
{
    printf("e_UnderRun %u, expblk %u\n", (U16) e_UnderRun, (size_t) nBlk8);
}

void peTmOut(nBlk8)
U8 nBlk8;
{
    printf("e_TmOut %u, expblk %u\n", (U16) e_TmOut, (size_t) nBlk8);
}

void peProt(nBlk8)
U8 nBlk8;
{
    printf("e_Prot %u, expblk %u\n", (U16) e_Prot, (size_t) nBlk8);
}

void peStop(nBlk8)
U8 nBlk8;
{
    printf("e_Stop %u, expblk %u\n", (U16) e_Stop, (size_t) nBlk8);
}

void peChkCRC(nBlk8)
U8 nBlk8;
{
    printf("e_ChkCRC %u, expblk %u\n", (U16) e_ChkCRC, (size_t) nBlk8);
}

void peReTx(nBlk8)
U8 nBlk8;
{
    printf("ReTx, expblk %u\n", (size_t) nBlk8);
}
#endif

void txAck(device)
JC_HANDLE device;
{
    U16 lock;


    lock = jcLock(device);
    z0Tx(&msg_ack, (size_t) 1);
    jcUnlock(lock);
}

STATUS XMSend()
{
    return (OK);
}


/* 128B ChkSum
        SOH, nBlk8, ~nBlk8, data[128], checksum
offset  0    1       2      3          131
size    1    1       1      128        1        = 132
*/

STATUS XMRxChkSum(_fn, _device)
char *_fn;
JC_HANDLE _device;
{
    static char *fn;
    static JC_HANDLE device;
    static STATUS rc, s;
    static U8 nBlk8;    /* contatore di blocco ad 8 bit */
    static U16 nRx;
    static FILE *fp;
    static boolean stayIn;
    static U16 lock;
    static char *pTx;

    static U8 chksum;

    char rxBuf[BS_CS128];
    char f[8+1+3+1];     /* nome file in pagina 3 */
    char m[2+1];         /* mode in pagina 3 */


    fn = _fn;
    device = (JC_HANDLE) _device;

    strcpy(f, fn);
    strcpy(m, "WB");
    fp = fopen(f, m);
    if (fp == NULL)
    {
        printf("cannot open %s\n", fn);
        rc = ERROR;
        goto quit;
    }
    stayIn = TRUE;

    resErr();

    nBlk8 = 1;
    rc = OK;

    /* only the first tx is msg_nak */
    pTx = &msg_nak;

    do
    {
        lock = jcLock(device);
        z0Tx(pTx, (size_t) 1);
        s = z0Rx(rxBuf, (size_t) BS_CS128);
        jcUnlock(lock);

        if (s == OK)
        {
            /* ricevuti 132 chars */
            if (rxBuf[0] == SOH)
            {
                /* TODO verifica rxBuf[1] == rxBuf[2] ^ 0xFF */
                if (rxBuf[1] == nBlk8)
                {
                    /* calc checksum */
                    chksum = CS8(&rxBuf[3], (U16) 128);
                    if (chksum == rxBuf[131])
                    {
                        write(fileno(fp), &rxBuf[3], (size_t) 128);
                        nBlk8++;
                        pTx = &msg_ack;
                    }
                    else
                    {
                        e_ChkCRC++;
#ifdef XMDBG
                        peChkCRC((U8) nBlk8);
#endif
                        pTx = &msg_nak;
                        if (e_ChkCRC == m_ChkCRC)
                        {
                            stayIn = FALSE;
                            rc = XM_RXERR;
                        }
                    }
                }
                else
                {
                    /* no error when receiving previous block */
                    if ((U8) rxBuf[1] == (nBlk8 - 1))
                    {
#ifdef XMDBG
                        peReTx((U8) nBlk8);
#endif
                        pTx = &msg_ack;
                    }
                    else
                    {
                        e_BadSeq++;
#ifdef XMDBG
                        peBadSeq((U8) nBlk8);
#endif
/*
                        hexdump((char *) rxBuf, (size_t) BS_CS128);
*/
                        pTx = &msg_nak;
                        if (e_BadSeq == m_BadSeq)
                        {
                            stayIn = FALSE;
                            rc = XM_RXERR;
                        }
                    }
                }
            }
            else
            {
                /* received unsupported char at offset 0 */
                /* should wait for line to be free and then send NAK */
                e_Prot++;
#ifdef XMDBG
                peProt((U8) nBlk8);
#endif

                pTx = &msg_nak;
                if (e_Prot == m_Prot)
                {
                    stayIn = FALSE;
                    rc = XM_RXERR;
                }
            }
        }
        else if (s == JC_TMOUT)
        {
            /* evaluate number of received chars */
            nRx = (U16) BS_CS128 - (size_t) jcRxMiss(device);
            if (nRx == 1)
            {
                /* only 1 char received */
                if (rxBuf[0] == EOT)    /* 0x04 */
                {
#ifdef XMDBG
                    printf("remote EOT\n");
#endif
                    txAck(device);
                    stayIn = FALSE;
                    rc = OK;
                }
                else if (rxBuf[0] == CAN)   /* 0x18 */
                {
#ifdef XMDBG
                    printf("remote CAN\n");
#endif
                    txAck(device);
                    stayIn = FALSE;
                    rc = XM_CAN;
                }
                else
                {
                    /* ricevuto 1 char ma non tra quelli supportati */
                    e_Prot++;
#ifdef XMDBG
                    peProt((U8) nBlk8);
#endif
                    pTx = &msg_nak;
                    if (e_Prot == m_Prot)
                    {
                        stayIn = FALSE;
                        rc = XM_RXERR;
                    }
                }
            }
            else if (nRx != 0)
            {
                e_UnderRun++;
#ifdef XMDBG
                peUndRun((U8) nBlk8);
#endif
                pTx = &msg_nak;
                if (e_UnderRun == m_UnderRun)
                {
                    stayIn = FALSE;
                    rc = XM_RXERR;
                }
            }
            else
            {
                e_TmOut++;
#ifdef XMDBG
                peTmOut((U8) nBlk8);
#endif
                pTx = &msg_nak;
                if (e_TmOut == m_TmOut)
                {
                    stayIn = FALSE;
                    rc = XM_TMOUT;
                }
            }
        }
        else if (s == JC_RXERR)
        {
            nRx = (U16) BS_CS128 - (size_t) jcRxMiss(device);
            e_Stop++;
#ifdef XMDBG
            peStop((U8) nBlk8);
#endif
            pTx = &msg_nak;
            if (e_Stop == m_Stop)
            {
                stayIn = FALSE;
                rc = XM_RXERR;
            }
        }
        else
        {
            printf("JC rc = 0x%02x\n", (size_t) s);
            printf("block %u is bad\n", (U16) nBlk8);
            pTx = &msg_nak;
        }
    }
    while (stayIn);

quit:

    if (fp)
        fclose(fp);

    return (rc);
}


STATUS YMRx(_device)
JC_HANDLE _device;
{
    return OK;
}
