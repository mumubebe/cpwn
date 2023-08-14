#include <sys/types.h>

#ifndef str_H
#define str_H

typedef struct
{
    char*    buf;             /* string */
    size_t   length;      /* length of string */
    size_t   capability;     /* malloc size */
} str;

extern str*   str_new(char *init);
extern str*   str_new_raw(char *init, size_t length);
extern str*   str_cat(str *ps, char *cs);
extern str*   str_cat_raw(str *ps, char *cs, size_t length);
extern str*   str_cat_str(str *ps, str* cs);
extern str*   str_popleft(str *ps, size_t n);
extern str*   str_popright(str *ps, size_t n);
extern str*   str_cpy(str* ps);
extern size_t  str_len(str *ps);
extern str*   str_new_rawempty(size_t length);
extern int     str_find(str *ps, str* subps);
extern void    str_free();
extern void    str_print(str *ps);
extern void    str_pprint(str *ps);
extern int     str_cmp(str *ps1, str *ps2);
extern str*   str_load_macro(str* ps1, ...);

#define str_load(ps1, ...) str_load_macro(ps1, __VA_ARGS__, NULL)


#endif 