#include <sys/types.h>

#ifndef str_H
#define str_H

typedef struct
{
    char*    buf;             /* string */
    size_t   length;      /* length of string */
    size_t   capability;     /* malloc size */
} str;

str*   str_new(char *init);
str*   str_new_raw(char *init, size_t length);
str*   str_cat(str *ps, char *cs);
str*   str_cat_raw(str *ps, char *cs, size_t length);
str*   str_cat_str(str *ps, str* cs);
str*   str_popleft(str *ps, size_t n);
str*   str_popright(str *ps, size_t n);
str*   str_cpy(str* ps);
size_t  str_len(str *ps);
str*   str_new_rawempty(size_t length);
int     str_find(str *ps, str* subps);
void    str_free();
void    str_print(str *ps);
void    str_pprint(str *ps);
int     str_cmp(str *ps1, str *ps2);
str*   str_load_macro(str* ps1, ...);

#define str_load(ps1, ...) str_load_macro(ps1, __VA_ARGS__, NULL)


#endif 