/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/
#define DIVHEADER
#include <STDIO.H>
/*
#include <MSXBIOS.H>
*/
#include <CONIO.H>
#include <MALLOC.H>

#define JIFFY   0xFC9E
#define TRUE	1
#define FALSE	0

extern int opldir();
extern int ghibli();

unsigned int i,k;
unsigned int prime, count, limit;
char dump, key;
size_t start, stop;
char *base, *p1;


int clear()
{
	p1 = base;
	*p1 = TRUE;
	opldir(p1 + 1, p1, limit);

	return (0);
}


int found()
{
	for (k = i * 2; k <= limit; k += i)
	{
		/* cut away all multiples */

		base[k] = FALSE;
	}
}


int main()
{
	printf("Max limit? ");
	i = scanf("%u", &limit);

	if (i != 1)
	{
		printf("\nbad value.\n");
		return;
	}

	base = malloc(limit + 1);
	if (base == NULL)
	{
		printf("\nnot enough memory.\n");
		return;
	}

	/* setup work array */

	clear();

	dump = TRUE;

    /* initialize prime counter */

	count = 0;

	/* read start timer */

	start = *((size_t *) JIFFY);

	/* 0 and 1 are not considered */

	for (i = 2; i <= limit; i++)
	{
		if (base[i])
		{
		     /* found a prime */

			found();

			/* prime found */

			count++;
			if (dump)
				printf("%u\n", i);
		}
	}

	stop = *((size_t *) JIFFY);

	/* dump how may primes found */

	printf("%u primes found.\n", count);

	/* dump how much time it took */

	printf("Elapsed vdp interrupt(s) = %u.\n", stop - start);

	free(base);
}



/* EOF */

