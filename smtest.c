/*
** pem 2018-06-28, revised 2023-05-07
**
** Unit tests for the string metrics library.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "string-metrics.h"
#include "sm-utf8.h"

#ifdef SM_WCHAR
#define SM_FUN(N) N ## _wchar
#else
#define SM_FUN(N) N
#endif

#define DE1337_STRING_IN  "!abc0123456789xyz~"
#define DE1337_STRING_OUT "!abcoizeasgt8gxyz~"

typedef struct testcase_s
{
    char *s1, *s2;
    int ld, osad, dld, lcs;
} testcase_t;

int
main(int argc, char **argv)
{
    int fail = 0;
#ifndef SM_WCHAR    
    int realfail = 0;
#endif

    /* Long enough for the longest test string */
#define SM_MAXLEN 16

    testcase_t tcases[] =
        {
	    { "", "", 0, 0, 0, 0 },	     /* Empty string */
            { "", "abc", 3, 3, 3, 0 },	     /* Additions to an empty string */
            { "abc", "", 3, 3, 3, 0},	     /* Delete all */
            { "a", "a", 0, 0, 0, 1 },	     /* No edit */
            { "a", "b", 1, 1, 1, 0 },           /* Single substitution */
            { "climax", "volmax", 3, 3, 3, 3 }, /* Three substitutions */
            { "foo123", "fool23", 1, 1, 1, 3 }, /* Sneaky substitution */
            { "test1", "test2", 1, 1, 1, 4 },   /* Single substitution again */
            { "test1", "test11", 1, 1, 1, 5 },  /* Addition */
            { "foobar", "foboar", 2, 1, 1, 2 }, /* Transposition */
            { "foobar", "fubar", 2, 2, 2, 3 },  /* Deletion and substitution */
            { "password", "pa55word", 2, 2, 2, 4 }, /* More substitutions */
            { "password", "passwd", 2, 2, 2, 5 },   /* Deletion */
            /* Various */
            { "book", "back", 2, 2, 2, 1 },
            { "kitten", "sitting", 3, 3, 3, 3 },
            { "saturday", "sunday", 3, 3, 3, 3 },
	    { "ca", "abc", 3, 3, 2, 1 },
            { "a", "aaaaaa", 5, 5, 5, 1 },
            { "pantera", "aorta", 5, 5, 4, 1 },
            { "elevenbytes", "e1evenbyt3s", 2, 2, 2, 7 },
            { "alongpassword", "A1ongPa55word", 5, 5, 5, 4 },
            { "alongpassword", "a1ongpassw0rd1", 3, 3, 3, 8 },
            { "waht", "what", 2, 1, 1, 1 },
            { "thaw", "what", 2, 2, 2, 2 },
            { "waht", "wait", 1, 1, 1, 2 },
            { "Damerau", "uameraD", 2, 2, 2, 5 },
            { "Damerau", "Daremau", 2, 2, 2, 2 },
            { "Damerau", "Damreau", 2, 1, 1, 3 },
            { "waht", "whit", 2, 2, 2, 1 },
            { "what", "wtah", 2, 2, 2, 1 },
            { "a cat", "an act", 3, 2, 2, 1 },
            { "a cat", "a abct", 3, 3, 2, 2 },
	    /* LCS */
	    { "foobar", "foobar123", 3, 3, 3, 6 },
	    { "foobar", "1foobar23", 3, 3, 3, 6 },
	    { "foobar", "12foobar3", 3, 3, 3, 6 },
	    { "foobar", "123foobar", 3, 3, 3, 6 },
            /* UTF-8 */
#ifdef SM_WCHAR
            { "*** Converting UTF-8 to wchar_t works",},
#else
            { "*** Treating UTF-8 as byte strings does not work" },
#endif
            { "å", "a", 1, 1, 1, 0 },
            { "blå", "blä", 1, 1, 1, 2 },
            { "åäö", "åöä", 2, 1, 1, 1 },
            { "skärgårdsnö", "skärgårdsö", 1, 1, 1, 9 },
            { "blåbär", "småbarn", 4, 4, 4, 2 },
            { "Höganäs", "Hoganas", 2, 2, 2, 3 },
            { "まるです", "るまです", 2, 1, 1, 2 },
            { "まるです", "まるす", 1, 1, 1, 2 },
            { "まるです", "ですまる", 4, 4, 4, 2 },
            { NULL, NULL, 0, 0, 0 }
        };

    printf("%-*s %-*s   L        OSA      DL       LCS\n",
	   SM_MAXLEN, "", SM_MAXLEN, "");
    for (int i = 0 ; tcases[i].s1 != NULL ; i++)
    {
        if (tcases[i].s1[0] == '*')
        {
#ifndef SM_WCHAR
            realfail = fail;
#endif
            printf("%s\n", tcases[i].s1);
            continue;
        }
        int ld, osad, dld, ldfailed = 0, osadfailed = 0, dldfailed = 0;
	int lcs, lcsfailed = 0;

#ifdef SM_WCHAR
        wchar_t s1[32];
        wchar_t s2[32];
        size_t s1len = utf8towstr(tcases[i].s1, s1);
        size_t s2len = utf8towstr(tcases[i].s2, s2);
#else
        char *s1 = tcases[i].s1;
        char *s2 = tcases[i].s2;
        size_t s1len = strlen(s1);
        size_t s2len = strlen(s2);
#endif
        int *tmpbuf;

        tmpbuf = (int *)malloc(SM_LEVENSHTEIN_TMP_SIZE(s2len) * sizeof(int));
        ld = SM_FUN(sm_levenshtein_distance)(s1, s2, tmpbuf);
        tmpbuf = (int *)realloc(tmpbuf,
                                SM_OSA_TMP_SIZE(s1len,s2len) * sizeof(int));
        osad = SM_FUN(sm_osa_distance)(s1, s2, tmpbuf);
	tmpbuf = (int *)realloc(tmpbuf,
				SM_DL_TMP_SIZE(s1len,s2len) * sizeof(int));
	dld = SM_FUN(sm_dl_distance)(s1, s2, tmpbuf);

	tmpbuf = (int *)realloc(tmpbuf,
				SM_LCS_TMP_SIZE(s1len,s2len * sizeof(int)));
	lcs = SM_FUN(sm_lcs_length)(s1, s2, tmpbuf);
        free(tmpbuf);
        if (ld != tcases[i].ld)
        {
            fail = 1;
            ldfailed = 1;
        }
        if (osad != tcases[i].osad)
        {
            fail = 1;
            osadfailed = 1;
        }
        if (dld != tcases[i].dld)
        {
            fail = 1;
            dldfailed = 1;
        }
	if (lcs != tcases[i].lcs)
	{
	    fail = 1;
	    lcsfailed = 1;
	}

        /* We need to adjust the width for the strings to make printf
           space correctly for UTF-8 strings. It doesn't work for
           Japanese, but it's better than nothing. */
        int pad1 = strlen(tcases[i].s1) - utf8len(tcases[i].s1);
        int pad2 = strlen(tcases[i].s2) - utf8len(tcases[i].s2);

        printf("%-*s %-*s  %2d (%2d)%c %2d (%2d)%c %2d (%2d)%c %2d (%2d)%c\n",
               SM_MAXLEN+pad1, tcases[i].s1,
               SM_MAXLEN+pad2, tcases[i].s2,
               ld, tcases[i].ld, (ldfailed  ? '*' : ' '),
               osad, tcases[i].osad, (osadfailed ? '*' : ' '),
	       dld, tcases[i].dld, (dldfailed ? '*' : ' '),
	       lcs, tcases[i].lcs, (lcsfailed ? '*' : ' '));
    }

    putchar('\n');

#ifdef SM_WCHAR
    wchar_t s1[32], s2[32];
    memset(s1, 0, sizeof(s1));
    utf8towstr(DE1337_STRING_IN, s1);
    utf8towstr(DE1337_STRING_OUT, s2);
#define SM_STRCMP wcscmp
#else
    char s1[32], s2[32];
    memset(s1, 0, sizeof(s1));
    strcpy(s1, DE1337_STRING_IN);
    strcpy(s2, DE1337_STRING_OUT);
#define SM_STRCMP strcmp
#endif
    /* Modify s1, compare to s2 */
    SM_FUN(sm_simple_de1337_1)(s1);
    if (SM_STRCMP(s1, s2) == 0)
        printf("sm_simple_de1337_1 ok\n");
    else
    {
        printf("sm_simple_de1337_1 failed\n");
        fail = 1;
    }
#ifdef SM_WCHAR
    memset(s1, 0, sizeof(s1));
    utf8towstr(DE1337_STRING_IN, s1);
#define SM_CHAR_T wchar_t
#else
    memset(s1, 0, sizeof(s1));
    strcpy(s1, DE1337_STRING_IN);
#define SM_CHAR_T char
#endif
    /* Put output in s2 */
    SM_FUN(sm_simple_de1337_2)(s1, s2, sizeof(s2)/sizeof(SM_CHAR_T));
    /* Put the expected result in s1 */
#ifdef SM_WCHAR
    utf8towstr(DE1337_STRING_OUT, s1);
#else
    strcpy(s1, DE1337_STRING_OUT);
#endif
    if (SM_STRCMP(s1, s2) == 0)
        printf("sm_simple_de1337_2 ok\n");
    else
    {
        printf("sm_simple_de1337_2 failed\n");
        fail = 1;
    }

#ifndef SM_WCHAR
    fail = realfail;
#endif
    if (fail)
        printf("\nOne or more tests failed\n");
    else
        printf("\nSucceeded\n");
    exit(fail);
}
