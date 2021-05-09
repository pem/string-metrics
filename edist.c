/*
**
** pem 2018-06-29
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "string-metrics.h"

static char *Progname;

static void
usage(void)
{
    fprintf(stderr, "Usage: %s [-l] [-o] [-d] [-D] [-C] <string1> <string2>\n", Progname);
    exit(1);
}

#define MAXSTR 256

int
main(int argc, char **argv)
{
    int c, *tmpbuf;
    char s1[MAXSTR], s2[MAXSTR];
    int s1len, s2len;
    int all = 1, lev = 0, osa = 0, dl = 0;
    int de1337 = 0, decase = 0;

    if ((Progname = strchr(argv[0], '/')) == NULL)
        Progname = argv[0];
    else
        Progname += 1;

    opterr = 0;
    while ((c = getopt(argc, argv, "CDdlo")) != EOF)
        switch (c)
        {
        case 'C':
            decase = 1;
            break;
        case 'D':
            de1337 = 1;
            break;
        case 'd':
            all = 0;
            dl = 1;
            break;
        case 'l':
            all = 0;
            lev = 1;
            break;
        case 'o':
            all = 0;
            osa = 1;
            break;
        default:
            usage();
        }

    if (optind != argc-2)
        usage();

    /* Always copy the strings, in case we'll de1337 or decase */
    strncpy(s1, argv[optind++], sizeof(s1));
    s1[sizeof(s1)-1] = '\0';
    strncpy(s2, argv[optind++], sizeof(s2));
    s2[sizeof(s2)-1] = '\0';
    s1len = strlen(s1);
    s2len = strlen(s2);

    if (de1337)
    {
      sm_simple_de1337_1(s1);
      sm_simple_de1337_1(s2);
    }
    if (decase)
    {
      int i;

      for (i = 0 ; i < s1len ; i++)
	s1[i] = tolower(s1[i]);
      for (i = 0 ; i < s1len ; i++)
	s2[i] = tolower(s2[i]);
    }
    if (all || lev)
    {
        tmpbuf = (int *)malloc(SM_LEVENSHTEIN_TMP_SIZE(s2len) * sizeof(int));
        printf("Levenshtein:              %4d\n",
               sm_levenshtein_distance(s1, s2, tmpbuf));
        free(tmpbuf);
    }
    if (all || osa)
    {
        tmpbuf = (int *)malloc(SM_OSA_TMP_SIZE(s1len,s2len) * sizeof(int));
        printf("Optimal String Alignment: %4d\n",
               sm_osa_distance(s1, s2, tmpbuf));
        free(tmpbuf);
    }
    if (all || dl)
    {
        tmpbuf = (int *)malloc(SM_DL_TMP_SIZE(s1len,s2len) * sizeof(int));
        printf("Damerau-Levenshtein:      %4d\n",
               sm_dl_distance(s1, s2, tmpbuf));
        free(tmpbuf);
    }

    exit(0);
}
