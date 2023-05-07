/*
** pem 2018-06-28, revised 2023-05-07
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
*/

#include "string-metrics.h"
#include <string.h>

#ifdef SM_WCHAR
#include <stdlib.h>
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
    for (int i = 0 ; i < rowmax ; i++)
    {
        for (int j = 0 ; j < colmax ; j++)
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
SM_FUN(sm_levenshtein_distance)(const SM_CHAR_T *s1,
                                const SM_CHAR_T *s2,
                                int *tmpbuf)
{
    int m = SM_STRLEN(s1);
    int n = SM_STRLEN(s2);
    int vmax = n+1;
    int *v0 = tmpbuf;
    int *v1 = tmpbuf+vmax;

    for (int i = 0 ; i < vmax ; i++)
        v0[i] = i;

    for (int i = 0 ; i < m ; i++)
    {
        int *tmp;

        v1[0] = i+1;
        for (int j = 0 ; j < n ; j++)
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
SM_FUN(sm_osa_distance)(const SM_CHAR_T *s1,
                        const SM_CHAR_T *s2,
                        int *tmpbuf)
{
    int m = SM_STRLEN(s1);
    int rowmax = m+1;
    int n = SM_STRLEN(s2);
    int colmax = n+1;

    for (int i = 0 ; i < rowmax ; i++)
        D(tmpbuf, colmax, i, 0) = i;
    for (int i = 0 ; i < colmax ; i++)
        D(tmpbuf, colmax, 0, i) = i;

    for (int i = 0 ; i < m ; i++)
    {
        for (int j = 0 ; j < n ; j++)
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

/* The Damerau-Levenshtein Distance algorithm requires an auxiliary
** array which can be up to the size of the alphabet. For 8-bit chars
** this is no problem, we simply use an array where we index with
** the char value, but for wide chars this is not feasible.
** We use a key-value pair array with just the characters that occur
** in the strings in that case.
*/

typedef int da256_t[256];
#define da256_set(DA, K, V) do { (DA)[(unsigned)(K)&0xff] = (V); } while(0)
#define da256_get(DA, K) ((DA)[(unsigned)(K)&0xff])

#ifdef SM_WCHAR
/* For wchar_t we use an array for single-byte chars, and a another array
** with key-value pairs for the rest.
** Since the number characters in the strings are expected to be fairly
** small (a "word" or a short sentence), and for most languages only a few
** of the characters are >= 256, so we simply use a linear search there.
** If this is perceived to be a performance problem we can optimize this
** to keep it sorted and use binary search, but for now this will do.
*/

typedef struct
{
    wchar_t key;
    int val;
} da_node_t;

typedef struct
{
    da256_t a;                  /* For bytes < 256 */
    da_node_t *n;               /* For bytes >= 256 */
    int i;
} da_t;

static void
da_init(da_t *da, int size)
{
    da->n = (da_node_t *)malloc(size * sizeof(da_node_t));

    if (da->n != NULL)
    {
        memset(da->n, 0, size * sizeof(da_node_t));
        da->i = 0;
    }
    memset(da->a, 0, sizeof(da256_t));
}

static void
da_destroy(da_t *da)
{
    if (da != NULL)
        free(da->n);
}

/* The number of character */
static void
da_set(da_t *da, wchar_t key, int val)
{
    if (key < 256)
        da256_set(da->a, key, val);
    else
    {
        int i;

        for (i = 0 ; i < da->i ; i++)
            if (da->n[i].key == key)
                break;
        if (i == da->i)
            da->n[da->i++].key = key;
        da->n[i].val = val;
    }
}

static int
da_get(da_t *da, wchar_t key)
{
    if (key < 256)
        return da256_get(da->a, key);
    for (int i = 0 ; i < da->i ; i++)
        if (da->n[i].key == key)
            return da->n[i].val;
    return 0;
}

#else  /* #ifdef SM_WCHAR */

typedef struct
{
    da256_t a;
} da_t;

#define da_init(DA, Ignore) memset((DA)->a, 0, sizeof((DA)->a))
#define da_set(DA, K, V) da256_set((DA)->a, (K), (V))
#define da_get(DA, K) da256_get((DA)->a, (K))
#define da_destroy(DA)

#endif  /* #ifdef SM_WCHAR else */

/* Damerau-Levenshtein Distance
**
** See https://en.wikipedia.org/wiki/Damerau-Levenshtein_distance
*/
int
SM_FUN(sm_dl_distance)(const SM_CHAR_T *s1,
                       const SM_CHAR_T *s2,
                       int *tmpbuf)
{
    int m = SM_STRLEN(s1);
    int rowmax = m+1;
    int n = SM_STRLEN(s2);
    int colmax = n+1;
    da_t da;

    da_init(&da, n+m);

    for (int i = 0 ; i < m ; i++)
        da_set(&da, s1[i], 0);
    for (int i = 0 ; i < n ; i++)
        da_set(&da, s2[i], 0);

    for (int i = 0 ; i < rowmax ; i++)
        D(tmpbuf, colmax, i, 0) = i;
    for (int i = 0 ; i < colmax ; i++)
        D(tmpbuf, colmax, 0, i) = i;

    for (int i = 1 ; i <= m ; i++)
    {
        int db = 0;

        for (int j = 1 ; j <= n ; j++)
        {
            int i1 = da_get(&da, s2[j-1]);
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
        da_set(&da, s1[i-1], i);
    }
    da_destroy(&da);
    DBG_PIM(tmpbuf, rowmax, colmax);
    return D(tmpbuf, colmax, rowmax-1, colmax-1);
}
