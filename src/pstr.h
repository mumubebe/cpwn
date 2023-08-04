#include <sys/types.h>

#ifndef PSTR_H
#define PSTR_H

typedef struct
{
    char*    buf;             /* string */
    size_t   length;      /* length of string */
    size_t   capability;     /* malloc size */
} pstr;

pstr*   pstr_new(char *init);
pstr*   pstr_new_raw(char *init, size_t length);
pstr*   pstr_cat(pstr *ps, char *cs);
pstr*   pstr_cat_raw(pstr *ps, char *cs, size_t length);
pstr*   pstr_cat_pstr(pstr *ps, pstr* cs);
pstr*   pstr_popleft(pstr *ps, size_t n);
pstr*   pstr_popright(pstr *ps, size_t n);
pstr*   pstr_cpy(pstr* ps);
size_t  pstr_len(pstr *ps);
pstr*   pstr_new_rawempty(size_t length);
int     pstr_find(pstr *ps, pstr* subps);
void    pstr_free();
void    pstr_print(pstr *ps);
void    pstr_pprint(pstr *ps);
int     pstr_cmp(pstr *ps1, pstr *ps2);
pstr*   pstr_load_macro(pstr* ps1, ...);

#define pstr_load(ps1, ...) pstr_load_macro(ps1, __VA_ARGS__, NULL)


#endif 