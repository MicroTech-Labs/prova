/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/
#define DIVHEADER
#include <STDIO.H>
#include <CONIO.H>
#include <STDLIB.H>
#include <PROCESS.H>

#include <MYTYPES.H>

#define TRUE	1
#define FALSE	0
#define JIFFY   (*((size_t *) 0xFC9E))

extern U16 crc16();
extern int calcrc();
extern void strcpy();


void test(_fn)
char *_fn;
{
    static char *fn;
    static FILE *fp;
    static int retValue;

    fn = _fn;

    fp = fopen(fn, "WB", (size_t) 1024);
    if (fp == NULL)
    {
        printf("cannot open %s\n", fn);
        return (ERROR);
    }

    retValue = fwrite("123 prova\ne poi", (int) 1, (int) 1024, (FILE *) fp);
    printf("retValue = %d\n", retValue);
    printf("ferror = %s\n", ferror(fp) ? "TRUE" : "FALSE");

    if (fp)
        fclose(fp);
}

#ifdef NOT_DEF

U16 crcnew(_ptr, _n)
char *_ptr;
U16 _n;
{
    static char *ptr;
    static U16 n;
    static U16 crc;
    static U8 i;

    ptr = _ptr;
    n = _n;

    crc = 0;
    while (n--)
    {
        crc ^= ((U16) *ptr++) << 8;
        i = 8;
        while (i--)
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
    }
    return (crc);
}

int main0()
{
    test("CRC.BIN");
}


int main1()
{
	U16 start, stop;
	U8 buf[32];
	U16 c1;
    int c2;


	start = JIFFY;
	while (start == JIFFY);
	c1 = crc16((U8 *) buf, (U16) 0, sizeof(buf));
	stop = JIFFY;
	printf("0x%04x %u\n", (U16) c1, (U16) (stop - start + 1));
}

int main2()
{
	char *buf = "1234567890XX";
	U16 c1;

	c1 = crc16(buf, (U16) 0, 10);
	printf("0x%04x\n", (U16) c1);
    buf[10] = c1 & 0xff;
    buf[11] = (c1 >> 8) & 0xff;    
	c1 = crc16(buf, (U16) 0, 12);
	printf("0x%04x\n", (U16) c1);
}
#endif

int main()
{
    char *pShell;
    char *pProgram;
    char *pParameters;
    char *argv[1];


    printf("enter sample2\n");

    pShell = getenv("SHELL");
    if (pShell == NULL)
    {
        printf("no SHELL\n");
        exit(0);
    }

    pProgram = getenv("PROGRAM");
    if (pProgram == NULL)
    {
        printf("no PROGRAM\n");
        exit(0);
    }

    pParameters = getenv("PARAMETERS");
    if (pParameters == NULL)
    {
        printf("no PARAMETERS\n");
        exit(0);
    }

    printf("%s - %s - %s\n", pParameters, pProgram, pShell);

/*
    test("12345678.123");
*/
    printf("exit sample2\n");
    exit(0);
}

/* EOF */
