#ifndef __STRING_H_
#define __STRING_H_

/*
	(C) Copyright by ASCII Corporation, 1989
		All rights Reserved.

	File: string.h		String Mnipulate Functions
*/
#ifndef HEADERtype
#include <type.h>
#endif	/*  HEADERtype  */


extern	char	*strncpy();
extern	VOID	sprintf(.);
extern	char	*strcpy();
extern	size_t  strlen();

extern	int     strcmp();

/* NOT_USED
extern	int     strcmp(), strncmp();
extern	char	*strcat(), *strchr();
extern	char	*strncat(), *strncpy();
extern	char	*strlwr(), *strupr();
extern	int	sscanf(.);
*/


#endif

