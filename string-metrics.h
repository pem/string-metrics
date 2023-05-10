/*
** pem 2018-06-28, revised 2023-05-07
**
*/

#pragma once
#include <wchar.h>

/* Return the required buffert size (in number of ints) for the s2 length */
#define SM_LEVENSHTEIN_TMP_SIZE(S2LEN) (2*((S2LEN)+1))

int sm_levenshtein_distance(const char *s1, const char *s2, int *tmpbuf);
int sm_levenshtein_distance_wchar(const wchar_t *s1, const wchar_t *s2, int *tmpbuf);

/* Return the required buffert size (in number of ints) for the s1 and s2 lengths */
#define SM_OSA_TMP_SIZE(S1LEN, S2LEN) (((S1LEN)+1) * ((S2LEN)+1))

int sm_osa_distance(const char *s1, const char *s2, int *tmpbuf);
int sm_osa_distance_wchar(const wchar_t *s1, const wchar_t *s2, int *tmpbuf);

/* Return the required buffert size (in number of ints) for the s1 and s2 lengths */
#define SM_DL_TMP_SIZE(S1LEN, S2LEN) (((S1LEN)+1) * ((S2LEN)+1))

int sm_dl_distance(const char *s1, const char *s2, int *tmpbuf);
int sm_dl_distance_wchar(const wchar_t *s1, const wchar_t *s2, int *tmpbuf);

#define SM_LCS_TMP_SIZE(S1LEN, S2LEN) ((S1LEN) * (S2LEN))

int sm_lcs_length(const char *s1, const char *s2, int *tmpbuf);
int sm_lcs_length_wchar(const wchar_t *s1, const wchar_t *s2, int *tmpbuf);

