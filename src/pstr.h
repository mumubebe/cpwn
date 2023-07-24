#include <sys/types.h>

#ifndef PSTR_H
#define PSTR_H

typedef struct
{
    char*    buf;             /* string */
    size_t   length;      /* length of string */
    size_t   capability;     /* malloc size */
} pstr;

pstr* pstr_new(char *init);
pstr* pstr_new_raw(char *init, size_t length);
pstr* pstr_cat(pstr *ps, char *cs);
pstr* pstr_cat_raw(pstr *ps, char *cs, size_t length);
pstr* pstr_pop(pstr *ps, size_t n);
size_t pstr_len(pstr *ps);
void pstr_free();
void pstr_print(pstr *ps);

#endif 