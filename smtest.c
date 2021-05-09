/*
**
** pem 2018-06-28
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string-metrics.h"

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
    int i, maxlen;
    int *tmpbuf;
    char buf[128];
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
            { NULL, NULL, 0, 0, 0 }
        };

    /* Figure out max. string length for nice output alignment */
    maxlen = 0;
    for (i = 0 ; tcases[i].s1 != NULL ; i++)
    {
        int len = strlen(tcases[i].s1);

        if (len > maxlen)
            maxlen = len;
        len = strlen(tcases[i].s2);
        if (len > maxlen)
            maxlen = len;
    }

    printf("%-*s %-*s   L        OSA      DL       LCS\n",
	   maxlen, "", maxlen, "");
    for (i = 0 ; tcases[i].s1 != NULL ; i++)
    {
        char *s1 = tcases[i].s1;
        char *s2 = tcases[i].s2;
        int ld, osad, dld, ldfailed = 0, osadfailed = 0, dldfailed = 0;
	int lcs, lcsfailed = 0;

        tmpbuf = (int *)malloc(SM_LEVENSHTEIN_TMP_SIZE(strlen(s2)) * sizeof(int));
        ld = sm_levenshtein_distance(s1, s2, tmpbuf);
        tmpbuf = (int *)realloc(tmpbuf,
                                SM_OSA_TMP_SIZE(strlen(s1),strlen(s2)) * sizeof(int));
        osad = sm_osa_distance(s1, s2, tmpbuf);
	tmpbuf = (int *)realloc(tmpbuf,
				SM_DL_TMP_SIZE(strlen(s1),strlen(s2)) * sizeof(int));
	dld = sm_dl_distance(s1, s2, tmpbuf);

	tmpbuf = (int *)realloc(tmpbuf,
				SM_LCS_TMP_SIZE(strlen(s1),strlen(s2) * sizeof(int)));
	lcs = sm_lcs_length(s1, s2, tmpbuf);
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
        printf("%-*s %-*s  %2d (%2d)%c %2d (%2d)%c %2d (%2d)%c %2d (%2d)%c\n",
               maxlen, s1, maxlen, s2,
               ld, tcases[i].ld, (ldfailed  ? '*' : ' '),
               osad, tcases[i].osad, (osadfailed ? '*' : ' '),
	       dld, tcases[i].dld, (dldfailed ? '*' : ' '),
	       lcs, tcases[i].lcs, (lcsfailed ? '*' : ' '));
    }

    putchar('\n');
    memset(buf, 0, sizeof(buf));
    strcpy(buf, DE1337_STRING_IN);
    sm_simple_de1337_1(buf);
    if (strcmp(buf, DE1337_STRING_OUT) == 0)
        printf("sm_simple_de1337_1 ok\n");
    else
    {
        printf("sm_simple_de1337_1 failed: %s != %s\n",
               buf, DE1337_STRING_OUT);
        fail = 1;
    }
    memset(buf, 0, sizeof(buf));
    sm_simple_de1337_2(DE1337_STRING_IN, buf, sizeof(buf));
    if (strcmp(buf, DE1337_STRING_OUT) == 0)
        printf("sm_simple_de1337_2 ok\n");
    else
    {
        printf("sm_simple_de1337_2 failed: %s != %s\n",
               buf, DE1337_STRING_OUT);
        fail = 1;
    }

    if (fail)
        printf("\nOne or more tests failed\n");
    else
        printf("\nSucceeded\n");
    exit(fail);
}
