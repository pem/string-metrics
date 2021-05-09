/*
** pem 2018-06-28
**
** Some "edit distance" algorithms, i.e. algorithm that measure "similarity"
** between strings. There are many, we only implement a few that might be
** useful for doing "look-alike" checks of old/new passwords.
** See https://en.wikipedia.org/wiki/String_metric for more information.
**
** The Levenshtein algorithms are a bit cryptic and are not obvious in any way;
** they are all variants of the Wagner-Fischer dynamic programming algorithm,
** for more information, see
**   https://en.wikipedia.org/wiki/Wagner-Fischer_algorithm
**
** See also
**   http://scarcitycomputing.blogspot.com/2013/04/damerau-levenshtein-edit-distance.html
** for a detailed example of the three algorithms.
*/

#include <string.h>
#include "string-metrics.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define MINM(A,B) ((A) < (B) ? (A) : (B))

static int
min(int a, int b)
{
    return MINM(a, b);
}

static int
min3(int a, int b, int c)
{
    int ab = MINM(a, b);

    return MINM(ab, c);
}

#define D(A,CMAX,R,C) ((A)[(R)*(CMAX)+(C)])

#ifdef DEBUG
static void
print_int_matrix(const int *m, int rowmax, int colmax)
{
    int i;

    for (i = 0 ; i < rowmax ; i++)
    {
        int j;

        for (j = 0 ; j < colmax ; j++)
            fprintf(stderr, "%3d", D(m, colmax, i, j));
        putc('\n', stderr);
    }
    putc('\n', stderr);
}
#define DBG_PIM(M,R,C) print_int_matrix(M,R,C)
#else
#define DBG_PIM(M,R,C)
#endif	/* #ifdef DEBUG */

/* Levenshtein Distance
** See https://en.wikipedia.org/wiki/Levenshtein_distance
*/
int
sm_levenshtein_distance(const char *s1, const char *s2, int *tmpbuf)
{
    int i;
    int m = strlen(s1);
    int n = strlen(s2);
    int vmax = n+1;
    int *v0 = tmpbuf;
    int *v1 = tmpbuf+vmax;

    for (i = 0 ; i < vmax ; i++)
        v0[i] = i;

    for (i = 0 ; i < m ; i++)
    {
        int *tmp;
        int j;

        v1[0] = i+1;
        for (j = 0 ; j < n ; j++)
        {
            int subcost;
            int delcost = v0[j+1] + 1;
            int inscost = v1[j] + 1;

            if (s1[i] == s2[j])
                subcost = v0[j];
            else
                subcost = v0[j] + 1;
            v1[j+1] = min3(delcost, inscost, subcost);
        }
        /* Swap v0 and v1 */
        tmp = v0;
        v0 = v1;
        v1 = tmp;
    }
    DBG_PIM(tmpbuf, 2, vmax);
    return v0[n];
}

/* Optimal String Alignment Distance or Restricted Edit Distance
** This is like Levenshtein Distance plus transposition.
** See https://en.wikipedia.org/wiki/Damerau-Levenshtein_distance
*/
int
sm_osa_distance(const char *s1, const char *s2, int *tmpbuf)
{
    int i;
    int m = strlen(s1);
    int rowmax = m+1;
    int n = strlen(s2);
    int colmax = n+1;

    for (i = 0 ; i < rowmax ; i++)
        D(tmpbuf, colmax, i, 0) = i;
    for (i = 0 ; i < colmax ; i++)
        D(tmpbuf, colmax, 0, i) = i;

    for (i = 0 ; i < m ; i++)
    {
        int j;

        for (j = 0 ; j < n ; j++)
        {
            int cost = (s1[i] == s2[j] ? 0 : 1);

            D(tmpbuf, colmax, i+1, j+1) =
                min3(D(tmpbuf, colmax, i, j)+cost, /* subst */
                     D(tmpbuf, colmax, i, j+1)+1,  /* deletion */
                     D(tmpbuf, colmax, i+1, j)+1); /* insertion */
                     
            if (i > 0 && j > 0 &&
                s1[i] == s2[j-1] && s1[i-1] == s2[j])
            {
                /* transposition */
                D(tmpbuf, colmax, i+1, j+1) =
                    min(D(tmpbuf, colmax, i+1, j+1),
                        D(tmpbuf, colmax, i-1, j-1)+cost);
            }
        }
    }
    DBG_PIM(tmpbuf, rowmax, colmax);
    return D(tmpbuf, colmax, rowmax-1, colmax-1);
}

/* Damerau-Levenshtein Distance
**
** See https://en.wikipedia.org/wiki/Damerau-Levenshtein_distance
*/
int
sm_dl_distance(const char *s1, const char *s2, int *tmpbuf)
{
    int i;
    int m = strlen(s1);
    int rowmax = m+1;
    int n = strlen(s2);
    int colmax = n+1;
    int da[256];                /* We assume a full 8 bit alphabet */

#ifdef DEBUG
    memset(da, -1, sizeof(da));
#endif
    for (i = 0 ; i < m ; i++)
        da[(unsigned)s1[i]&0xff] = 0;
    for (i = 0 ; i < n ; i++)
        da[(unsigned)s2[i]&0xff] = 0;

    for (i = 0 ; i < rowmax ; i++)
        D(tmpbuf, colmax, i, 0) = i;
    for (i = 0 ; i < colmax ; i++)
        D(tmpbuf, colmax, 0, i) = i;

    for (i = 1 ; i <= m ; i++)
    {
        int j, db = 0;

        for (j = 1 ; j <= n ; j++)
        {
            int i1 = da[(unsigned)s2[j-1]&0xff];
            int j1 = db;
            int cost = 0;

            if (s1[i-1] == s2[j-1])
                db = j;
            else
                cost = 1;
            D(tmpbuf, colmax, i, j) =
                min3(D(tmpbuf, colmax, i-1, j-1) + cost, /* subst */
		     D(tmpbuf, colmax, i, j-1) + 1,  /* insertion */
		     D(tmpbuf, colmax, i-1, j) + 1); /* deletion */
            if (i1 > 0 && j1 > 0)
            {
                /* transposition */
                D(tmpbuf, colmax, i, j) =
                    min(D(tmpbuf, colmax, i, j),
                        D(tmpbuf, colmax, i1-1, j1-1) + (i-i1-1) + (j-j1-1) + 1);
            }
        }
        da[(unsigned)s1[i-1]&0xff] = i;
    }
#ifdef DEBUG
    for (i = 0 ; i < sizeof(da)/sizeof(int) ; i++)
        if (da[i] >= 0)
            fprintf(stderr, " %2c:%3d", i, da[i]);
    putc('\n', stderr);
#endif
    DBG_PIM(tmpbuf, rowmax, colmax);
    return D(tmpbuf, colmax, rowmax-1, colmax-1);
}
