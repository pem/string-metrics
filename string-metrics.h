/*
**
** pem 2018-06-28
**
*/

#pragma once

/* Return the required buffert size (in number of ints) for the s2 length */
#define SM_LEVENSHTEIN_TMP_SIZE(S2LEN) (2*((S2LEN)+1))

extern int sm_levenshtein_distance(const char *s1, const char *s2, int *tmpbuf);

/* Return the required buffert size (in number of ints) for the s1 and s2 lengths */
#define SM_OSA_TMP_SIZE(S1LEN, S2LEN) (((S1LEN)+1) * ((S2LEN)+1))

extern int sm_osa_distance(const char *s1, const char *s2, int *tmpbuf);

/* Return the required buffert size (in number of ints) for the s1 and s2 lengths */
#define SM_DL_TMP_SIZE(S1LEN, S2LEN) (((S1LEN)+1) * ((S2LEN)+1))

extern int sm_dl_distance(const char *s1, const char *s2, int *tmpbuf);

#define SM_LCS_TMP_SIZE(S1LEN, S2LEN) ((S1LEN) * (S2LEN))

extern int sm_lcs_length(const char *s1, const char *s2, int *tmpbuf);

extern void sm_simple_de1337_1(char *s);
extern int sm_simple_de1337_2(const char *s1, char *s2, int s2max);
