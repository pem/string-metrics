/*
** pem 2018-06-28, revised 2023-05-07
**
** A simple de-1337er. This only covers the most common
** use of digits. In reality, there's no single standard
** way to do this, so a full de-1337er would have to try a
** number of different variants. For example, 1 can be either
** i or l (lower case L), 2 is not always Z, either 6 or 9
** can be G (but usually not both). And then there are usually
** other characters outside the numerical range as well.
*/

#include "string-metrics.h"

#ifdef SM_WCHAR
#define SM_FUN(N) N ## _wchar
#define SM_CHAR_T wchar_t
#else
#include <string.h>
#define SM_FUN(N) N
#define SM_CHAR_T char
#endif

static char *Simple_deleet_tab =
  /* 0123456789 */
    "oizeasgt8g";		/* Implicit lower case */

/* de-1337 inline */
void
SM_FUN(sm_simple_de1337_1)(SM_CHAR_T *s)
{
    for (; *s != 0 ; s++)
        if ('0' <= *s && *s <= '9')
            *s = Simple_deleet_tab[(*s) - '0'];
}

/* de-1337 while copying to buffer. Returns the length of the
   copied string. */
int
SM_FUN(sm_simple_de1337_2)(const SM_CHAR_T *s1, SM_CHAR_T *s2, int s2max)
{
    int i = 0;

    while (i < s2max && *s1 != 0)
        if ('0' <= *s1 && *s1 <= '9')
            s2[i++] = Simple_deleet_tab[(*s1++) - '0'];
        else
            s2[i++] = *s1++;
    if (i == s2max)
        i -= 1;
    s2[i] = 0;
    return i;
}
