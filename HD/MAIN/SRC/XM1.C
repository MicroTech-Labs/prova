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
extern U16 crc16();
extern U16 rdBE16();

#include <A:\MAIN\H-LOCAL\UTILS.H>

#define BS_CRC128 (3 +  128 + 2)
#define BS_CRC1K  (3 + 1024 + 2)

#include <A:\MAIN\H-LOCAL\XMVARS.H>

FILE *fileOpen(_fn)
char *_fn;
{
    char f[128+1];   /* nome file in pagina 3 */
    char m[2+1];     /* mode in pagina 3 */
    FILE *fp;


    strcpy(f, _fn);
    strcpy(m, "WB");
    fp = fopen(f, m);
    if (fp == NULL)
    {
#ifdef XMDBG
        printf("cannot open %s\n", _fn);
#endif
    }
    return (fp);
}

/* 1KB CRC
        STX, nBlk8, ~nBlk8, data[1024], crc[2]
offset  0    1       2      3           1027
size    1    1       1      1024        2       = 1029
*/

/* 128B CRC
        SOH, nBlk8, ~nBlk8, data[128], crc[2]
offset  0    1       2      3          131
size    1    1       1      128        2        = 133
*/


STATUS XMRxCRC(_fn, _device)
char *_fn;
JC_HANDLE _device;
{
    static JC_HANDLE device;
    static STATUS rc;
    static STATUS s;    
    static U8 nBlk8;    /* contatore di blocco ad 8 bit */
    static U16 nRx;
    static U8 rx0;
    static FILE *fp;
    static boolean stayIn;
    static U16 lock;
    static char *pTx;
    static U16 crc;
    static U8 nTryC;
    static U8 negoCRC;
    static boolean isAck;

    char rxBuf[BS_CRC1K];


    device = (JC_HANDLE) _device;

    if (NULL == (fp = fileOpen(_fn)))
        return (ERROR);

    stayIn = TRUE;
    nBlk8 = 1;
    rc = OK;
    isAck = FALSE;
    nTryC = 10;
    negoCRC = TRUE;
    resErr();

    while (stayIn)
    {
        /* select feedback message */
        if (isAck)
        {
            pTx = &msg_ack;
        }
        else
        {
            if (negoCRC)
            {
                if (nTryC--)
                {
                    /* advertise CRC capability */
                    pTx = &msg_C;

                    /* ignore error(s) during negotiation */
                    resErr();
                }
                else
                {
                    /* FE does not support CRC */
                    stayIn = FALSE;
                    rc = XM_UNSUPP;
                    break;      /* exit from loop */
                }
            }
            else
            {
                pTx = &msg_nak;
            }
        }
        isAck = FALSE;

        lock = jcLock(device);
        z0Tx(pTx, (size_t) 1);
        s = z0Rx(rxBuf, (size_t) BS_CRC1K);
        jcUnlock(lock);

        /* evaluate number of received chars */
        nRx = (U16) BS_CRC1K - (size_t) jcRxMiss(device);
        rx0 = rxBuf[0];

        switch (s)
        {
        case OK:
            if (rx0 == STX)
            {
                if (rxBuf[1] == 0)
                {
                    hexdump((char *) rxBuf, (size_t) 64);
                }
                else if (rxBuf[1] == nBlk8)                  /* TODO verifica rxBuf[1] == rxBuf[2] ^ 0xFF */
                {
                    crc = crc16((U8 *) &rxBuf[3], (U16) 0, (size_t) 1024);
                    if (crc == (rdBE16(&rxBuf[3 + 1024])))
                    {
                        write(fileno(fp), &rxBuf[3], (size_t) 1024);
                        nBlk8++;
                        isAck = TRUE;
                        negoCRC = FALSE;
                    }
                    else
                    {
                        e_ChkCRC++;
#ifdef XMDBG
                        peChkCRC((U8) nBlk8);
#endif
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
                        isAck = TRUE;
                    }
                    else
                    {
                        e_BadSeq++;
#ifdef XMDBG
                        peBadSeq((U8) nBlk8);
#endif
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
                if (e_Prot == m_Prot)
                {
                    stayIn = FALSE;
                    rc = XM_RXERR;
                }
            }
        break;

        case JC_TMOUT:

            switch (nRx)
            {
            case 1:
                /* only 1 char received */
                switch (rx0)
                {
                case EOT:    /* 0x04 */
#ifdef XMDBG
                    printf("remote: EOT\n");
#endif
                    txAck(device);
                    stayIn = FALSE;
                    rc = OK;
                break;

                case CAN:          /* 0x18 */
#ifdef XMDBG
                    printf("remote: CANCEL\n");
#endif
                    txAck(device);
                    stayIn = FALSE;
                    rc = XM_CAN;
                break;

                default:
                    /* ricevuto 1 char ma non tra quelli supportati */
                    e_Prot++;
#ifdef XMDBG
                    peProt((U8) nBlk8);
#endif
                    if (e_Prot == m_Prot)
                    {
                        stayIn = FALSE;
                        rc = XM_RXERR;
                    }
                }
            break;

            case BS_CRC128:
                if (rx0 == SOH)
                {
                    /* TODO verifica rxBuf[1] == rxBuf[2] ^ 0xFF */
                    if (rxBuf[1] == nBlk8)
                    {
                        crc = crc16((U8 *) &rxBuf[3], (U16) 0, (size_t) 128);
                        if (crc == rdBE16(&rxBuf[3 + 128]))
                        {
                            write(fileno(fp), &rxBuf[3], (size_t) 128);
                            nBlk8++;
                            isAck = TRUE;
                            negoCRC = FALSE;
                        }
                        else
                        {
                            e_ChkCRC++;
#ifdef XMDBG
                            peChkCRC((U8) nBlk8);
#endif
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
                            isAck = TRUE;
                        }
                        else
                        {
                            e_BadSeq++;
#ifdef XMDBG
                            peBadSeq((U8) nBlk8);
#endif
                            if (e_BadSeq == m_BadSeq)
                            {
                                stayIn = FALSE;
                                rc = XM_RXERR;
                            }
                        }
                    }
                }
/* TMPMAR_DEBUG */
                else
                {
                    /* received unsupported char at offset 0 */
                    /* should wait for line to be free and then send NAK */
                    e_Prot++;
#ifdef XMDBG
                    peProt((U8) nBlk8);
#endif
                    if (e_Prot == m_Prot)
                    {
                        stayIn = FALSE;
                        rc = XM_RXERR;
                    }
                }
            break;

            case 0:
                e_TmOut++;
#ifdef XMDBG
                peTmOut((U8) nBlk8);
#endif
                if (e_TmOut == m_TmOut)
                {
                    stayIn = FALSE;
                    rc = XM_TMOUT;
                }
            break;

            default:    /* nRx != 0 */
                e_UnderRun++;
#ifdef XMDBG
                peUndRun((U8) nBlk8);
#endif
                if (e_UnderRun == m_UnderRun)
                {
                    stayIn = FALSE;
                    rc = XM_RXERR;
                }
            }
            
        break;

        case JC_RXERR:
            e_Stop++;
#ifdef XMDBG
            peStop((U8) nBlk8);
#endif
            if (e_Stop == m_Stop)
            {
                stayIn = FALSE;
                rc = XM_RXERR;
            }
        break;

        default:
            printf("unknown s=0x%02x expblk %u\n", (size_t) s, (U16) nBlk8);
        }
    }

    fclose(fp);
    return (rc);
}

