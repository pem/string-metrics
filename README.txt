C functions for calculating some string metrics:
- Levenshtein Distance
- Optimal String Alignment Distance or Restricted Edit Distance
  This is like Levenshtein Distance plus transposition.
- Damerau-Levenshtein Distance
- Longest common substring

There are two variants of the functions:
- Without the "_wchar" suffix, takes "char *" strings as parameters.
  These assumes 8-bit encodings, i.e. USASCII or ISO8859/Latin encodings.
  They do not work with UTF-8 strings.
- With the "_wchar" suffix, takes "wchar_t *" strings. Convert UTF-8 strings
  to wchar_t to use these.

References:
https://en.wikipedia.org/wiki/String_metric
https://en.wikipedia.org/wiki/Levenshtein_distance
https://en.wikipedia.org/wiki/Damerau-Levenshtein_distance
https://en.wikipedia.org/wiki/Wagner-Fischer_algorithm
https://en.wikipedia.org/wiki/Longest_common_substring_problem
