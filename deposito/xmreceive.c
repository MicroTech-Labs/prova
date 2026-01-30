
#ifdef NOT_DEF
/* ok se prima rx e poi tx, timeout quasi ok */

STATUS XMReceive(_filename, _device)
char *_filename;
A2_HANDLE *_device;
{
    static char *filename;
    static A2_HANDLE *device;
    static STATUS retCode, s;
    static U8 nBlk8;
    static U16 rxBlk;
    static boolean first;
    static U8 n;
    static U8 rx1;
    static U8 rxBuf[132];
    static FILE *fp;
    static boolean stayIn;
    static U16 lock;
    static char *pTx;
    static U16 tmOut;
    static U8 chksum;

    char f[8+1+3+1];     /* nome file in pagina 3 */
    char m[2+1];             /* mode in pagina 3 */


    filename = _filename;
    device = _device;


    strcpy(f, filename);
    strcpy(m, "WB");
    fp = fopen(f, m, 256);
    if (fp == NULL)
        return (ERROR);

    stayIn = TRUE;

    pTx = &nak;

    tmOut = 0;
    first = TRUE;
    nBlk8 = 1;
    rxBlk = 0;

    do
    {
        lock = rLock(device);

        swToTx(device);
        s0Tx(pTx, (U16) 1);
        swToRx(device);

        s = s0Rx(&rx1, (U8) 1, (U8) 50);
        if (s == OK)
        {
            if (rx1 == SOH)
            {
                s0Rx(& rxBuf[1], (U16) 131, (U8) 10);
                rUnlock(lock);

                /* check checksum */

                chksum = CS8(&rxBuf[3], (U16) 128);
                if (chksum == rxBuf[131])
                {
                    /* save block */

                    for (n = 3; n < 131; n++)
                        putc((char) rxBuf[n], fp);

                    nBlk8++;
                    rxBlk++;
                    pTx = &ack;
                }
                else
                {
                    pTx = &nak;
                }
            }
            else if (rx1 == EOT)
            {
                swToTx(device);
                s0Tx(&ack, (U16) 1);
                rUnlock(lock);
                printf("EOT\n");
                break;
            }
            else
            {
                /* rx unsupported char */
                /* should wait for line to be free and then send NAK */

                swToTx(device);
                s0Tx(&nak, (U16) 1);
                rUnlock(lock);
                printf("rx unsupp\n");
                break;
            }
        }
        else if (s == TIMEOUT)
        {
            rUnlock(lock);
            tmOut++;
            printf("timeout %d\n", (U16) tmOut);
            pTx = &nak;
        }
        else
        {
            rUnlock(lock);
            printf("block %u is bad\n", (U16) nBlk8);
            pTx = &nak;
        }
    }
    while (stayIn);

    if (fp)
        fclose(fp);

    printf("%u blocks received\n", rxBlk);
    retCode = OK;

quit:

    return (retCode);
}
#endif

#ifdef NOT_DEF
/* ok 19200, quasi anche in C */
STATUS XMReceive(_filename, _device)
char *_filename;
A2_HANDLE *_device;
{
    static char *filename;
    static A2_HANDLE *device;
    static STATUS retCode;
    static U16 nBlock;
    static boolean first;
    static U8 n;
    static U8 rx1;
    static U8 rxBuf[132];
    static FILE *fp;
    static boolean stayIn;
    static U16 lock;
    static char *pTx;
    static char *pRx;


    char f[8+1+3+1];     /* nome file in pagina 3 */
    char m[2+1];             /* mode in pagina 3 */


    filename = _filename;
    device = _device;


    strcpy(f, filename);
    strcpy(m, "WB");
    fp = fopen(f, m, 256);
    if (fp == NULL)
        return (ERROR);

    stayIn = TRUE;

    pTx = &nak;

    first = TRUE;
    nBlock = 1;

    do
    {
        pRx = &rxBuf[1];
        n = 131;

        lock = rLock(device);

        swToTx(device);

        s0Tx(pTx, (U16) 1);

        swToRx(device);

        if (OK == s0Rx(&rx1, (U8) 1), (U8) 50)
        /* if (s0Rx(&rx1, (U16) 1) == OK) */
        {
            if (rx1 == SOH)
            {
                s0Rx(& rxBuf[1], (U16) 131, (U8) 10);

                /*
                do
                {
                    s0Rx(pRx++, (U8) 1);
                }
                while (--n);
                */
                rUnlock(lock);

                /* check checksum */

                /* save block */

                for (n = 3; n < 131; n++)
                    putc((char) rxBuf[n], fp);

                pTx = &ack;
            }
            else if (rx1 == EOT)
            {
                swToTx(device);
                s0Tx(&ack, (U16) 1);
                rUnlock(lock);
                break;
            }
            else
            {
                /* rx unsupported char */
                /* should wait for line to be free and then send NAK */

                swToTx(device);
                s0Tx(&nak, (U16) 1);
                rUnlock(lock);

                break;
            }
        }
        else
        {
            rUnlock(lock);
            printf("bad block 0x%04x\n", nBlock);
            pTx = &ack;
        }
    }
    while (stayIn);

    if (fp)
        fclose(fp);

    retCode = OK;

quit:

    return (retCode);
}

#endif
