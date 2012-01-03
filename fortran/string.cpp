#include <string.h>

#include "string.h"

// Functions copied form OpenMPI

/*
 * creates a C string from an F77 string
 */
bool fortran::String::f2c(char* fstr, int len, char** cstr)
{
	char *end;
	int i;

	/* Leading and trailing blanks are discarded. */

	end = fstr + len - 1;

	for (i = 0; (i < len) && (' ' == *fstr); ++i, ++fstr)
		continue;

	if (i >= len)
		len = 0;
	else {
		for (; (end > fstr) && (' ' == *end); --end)
			continue;

		len = end - fstr + 1;
	}

	/* Allocate space for the C string. */

	if (0L == (*cstr = new char[len + 1]))
		return false;

	/* Copy F77 string into C string and NULL terminate it. */

	if (len > 0)
		strncpy(*cstr, fstr, len);
	(*cstr)[len] = '\0';

	return true;
}


/*
 * Copy a C string into a Fortran string.  Note that when Fortran
 * copies strings, even if it operates on subsets of the strings, it
 * is expected to zero out the rest of the string with spaces.  Hence,
 * when calling this function, the "len" parameter should be the
 * compiler-passed length of the entire string, even if you're copying
 * over less than the full string.  Specifically:
 *
 * http://www.ibiblio.org/pub/languages/fortran/ch2-13.html
 *
 * "Whole operations 'using' only 'part' of it, e.g. assignment of a
 * shorter string, or reading a shorter record, automatically pads the
 * rest of the string with blanks."
 */
void fortran::String::c2f(char *cstr, char *fstr, int len)
{
	int i;

	strncpy(fstr, cstr, len);
	for (i = strlen(cstr); i < len; ++i)
		fstr[i] = ' ';
}