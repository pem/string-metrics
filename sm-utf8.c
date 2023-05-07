/*
** pem 2023-05-07
**
** Minimal UTF-8 functions needed to test the string metrics library.
**
*/

#include <stddef.h>
#include <wchar.h>

static unsigned
utf8towchar(const char *u, wchar_t *w)
{
    if (u[0] == 0)
    {
        *w = (wchar_t)0;
        return 1;
    }
    if ((u[0] & 0x80) == 0) /* 0xxxxxxx */
    {
        *w = (wchar_t)(u[0] & 0xFF); /* Beware of sign extension */
        return 1;
    }
    if ((u[0] & 0xE0) == 0xC0) /* 110xxxxx 10xxxxxx */
    {
        if ((u[1] & 0xC0) != 0x80)
            return 0;
        *w = (wchar_t)(((u[0] & 0x1F) <<  6) |
                       (u[1]  & 0x3F));
        return 2;
    }
    if ((u[0] & 0xF0) == 0xE0) /* 1110xxxx 10xxxxxx 10xxxxxx */
    {
        if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80)
            return 0;
        *w = (wchar_t)(((u[0] & 0x0F) << 12) |
                       ((u[1] & 0x3F) <<  6) |
                       (u[2]  & 0x3F));
        return 3;
    }
    if ((u[0] & 0xF8) == 0xF0) /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    {
        if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80 ||
            (u[3] & 0xC0) != 0x80)
            return 0;
        *w = (wchar_t)(((u[0] & 0x07) << 18) |
                       ((u[1] & 0x3F) << 12) |
                       ((u[2] & 0x3F) <<  6) |
                       (u[3]  & 0x3F));
        return 4;
    }
    return 0;
}

size_t
utf8towstr(const char *u, wchar_t *wstr)
{
    wchar_t *w = wstr;

    while (*u != '\0')
        u += utf8towchar(u, w++);
    *w = (wchar_t)0;
    return w-wstr;
}

size_t
utf8len(const char *utf8)
{
    size_t count = 0;

    while (*utf8 != '\0')
    {
        wchar_t w;

        utf8 += utf8towchar(utf8, &w);
        count += 1;
    }
    return count;
}
