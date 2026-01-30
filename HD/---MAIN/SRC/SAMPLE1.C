/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/
#define DIVHEADER
#include <STDIO.H>
#include <IO.H>
#include <STDLIB.H>
#include <PROCESS.H>
#include <MALLOC.H>

/* #include <STRING.H> */
extern	char	*strcat(), *strcpy(), *strlen();

#include <MYTYPES.H>

/*
#define TRUE	1
#define FALSE	0
*/
#define JIFFY   (*((size_t *) 0xFC9E))

char *env;


int getEV()
{
    char *argv[10];
    char *pEV;
    char *pValue;


    pEV = "SHELL";
    pValue = getenv(pEV);
    if (pValue)
        printf("%s=%s\n",pEV, pValue);

    pEV = "PROGRAM";
    pValue = getenv(pEV);
    if (pValue)
        printf("%s=%s\n",pEV, pValue);

    pEV = "PARAMETERS";
    pValue = getenv(pEV);
    if (pValue)
        printf("%s=%s\n",pEV, pValue);

    pEV = "JC";
    pValue = getenv(pEV);
    if (pValue)
        printf("%s=%s\n",pEV, pValue);

    return (OK);
}

char buf[128];

int spawn(cmd)
char *cmd;
{
    char *argv[10];
    char *fn;
    static FILE *fp;
    static int retValue;


    fn = "JC.BAT";
    fp = fopen(fn, "W");
    if (fp == NULL)
    {
        printf("cannot open %s\n", fn);
        return (ERROR);
    }

    strcpy(buf, cmd);
    strcat(buf, "\n");
    retValue = fwrite(buf, (int) 1, (int) strlen(buf), (FILE *) fp);
/*
    printf("retValue = %d\n", retValue);
    printf("ferror = %s\n", ferror(fp) ? "TRUE" : "FALSE");
*/

    fclose(fp);

    argv[0] = "JC.BAT";
    argv[1] = "";
    execvp("COMMAND2.COM", argv);

    return 0;
}


int main()
{
    printf("enter main\n");

    getEV();

    spawn("dir\n");

    printf("exit main\n");
}

/* EOF */
