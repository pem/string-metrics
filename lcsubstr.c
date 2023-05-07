/*
** pem 2018-07-07, revised 2023-05-07
**
** Longest common substring algorithm.
**
** See: https://en.wikipedia.org/wiki/Longest_common_substring_problem
*/

#include "string-metrics.h"

#ifdef SM_WCHAR
#define SM_FUN(N) N ## _wchar
#define SM_CHAR_T wchar_t
#define SM_STRLEN wcslen
#else
#include <string.h>
#define SM_FUN(N) N
#define SM_CHAR_T char
#define SM_STRLEN strlen
#endif

#ifdef DEBUG
#include <stdio.h>
#endif

#define L(A,CMAX,R,C) ((A)[(R)*(CMAX)+(C)])

#ifdef DEBUG
static void
print_int_matrix(const int *m, int rowmax, int colmax)
{
    int i;

    for (i = 0 ; i < rowmax ; i++)
    {
        int j;

        for (j = 0 ; j < colmax ; j++)
            fprintf(stderr, "%3d", L(m, colmax, i, j));
        putc('\n', stderr);
    }
    putc('\n', stderr);
}
#define DBG_PIM(M,R,C) print_int_matrix(M,R,C)
#else
#define DBG_PIM(M,R,C)
#endif	/* #ifdef DEBUG */

/* Return the length of the LCS. (We don't care about which the
** substrings are.)
*/
int
SM_FUN(sm_lcs_length)(const SM_CHAR_T *s1,
                      const SM_CHAR_T *s2,
                      int *tmpbuf)
{
    int z = 0;
    int m = SM_STRLEN(s1);
    int n = SM_STRLEN(s2);

    for (int i = 0 ; i < m ; i++)
    {
        for (int j = 0 ; j < n ; j++)
        {
            if (s1[i] != s2[j])
                L(tmpbuf, n, i, j) = 0;
            else
            {
                if (i == 0 || j == 0)
                    L(tmpbuf, n, i, j) = 1;
                else
                    L(tmpbuf, n, i, j) = L(tmpbuf, n, i-1, j-1) + 1;
                if (L(tmpbuf, n, i, j) > z)
                    z = L(tmpbuf, n, i, j);
            }
        }
    }
    DBG_PIM(tmpbuf, m, n);
    return z;
}
