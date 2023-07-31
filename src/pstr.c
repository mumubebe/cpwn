#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "pstr.h"

void pstr_resize_length(pstr* ps, size_t n);
pstr* pstr_pop(pstr* ps, size_t n, int type);
pstr* pstr_grow(pstr *ps, size_t length);


#define POPRIGHT 0
#define POPLEFT 1

/**
 * Create a new pstr from a char*
 * 
 * Note that it expects a null terminated string. strlen terminates on any \x00
 * Use pstr_new_raw with a fixed length for a byte safe alternative
*/
pstr* pstr_new(char *init) {
    return pstr_new_raw(init, strlen(init));
}

/**
 * Create a binary safe pstr from a char*, expects a length 
 * 
*/
pstr* pstr_new_raw(char *init, size_t length) {
    pstr* ps = (pstr*) malloc(sizeof(pstr));

    ps->length = length;
    ps->capability = length + 1;
    ps->buf = malloc(ps->length);

    memcpy(ps->buf, init, length + 1);
    
    return ps;
}


pstr* pstr_cpy(pstr *ps) {
    return pstr_new_raw(ps->buf, ps->length);
}

pstr* pstr_grow(pstr *ps, size_t length) {
    size_t newcap = length + ps->length;
    if (ps->capability > (newcap)) { return ps; }

    ps->buf = (char *)realloc(ps->buf, newcap);
    
    return ps;
}
/**
 * Concat pstr with another prst, modified in place
*/
pstr* pstr_cat_pstr(pstr *ps1, pstr* ps2) {
    return pstr_cat_raw(ps1, ps2->buf, ps2->length);
}

pstr* pstr_cat_raw(pstr *ps1, char *cs, size_t length) {
    if (length == 0) {
        return ps1;
    }
    ps1 = pstr_grow(ps1, length + 1);

    /* Copy new str including nul byte */
    memcpy(ps1->buf + ps1->length, cs, length + 1);
    ps1->length = ps1->length + length; 
    return ps1;
}

/**
 * Concates pstr ps1 with a char*, expects a null terminated char*
 * modified in place
*/
pstr* pstr_cat(pstr *ps1, char* cs) {
    return pstr_cat_raw(ps1, cs, strlen(cs));
}


/**
 * Print pstr, non printable characters will be displayed as hex (as \xFF)
 * does not include null terminator \x00
*/
void pstr_print(pstr *ps) {
    if (ps == NULL || ps->buf == NULL) {
        printf("<pstr: NULL>\n");
        return;
    }
    int n = 0;
    while(n < ps->length) {
        unsigned char c = ps->buf[n];
        switch(c) {
            case '\n': printf("\\n"); break;
            case '\t': printf("\\t"); break;
            case '\v': printf("\\v"); break;
            case '\b': printf("\\b"); break;
            case '\r': printf("\\r"); break;
            case '\f': printf("\\f"); break;
            case '\a': printf("\\a"); break;
            case '\\': printf("\\\\"); break;
            default:
                if (isprint(c)) {
                    putchar(c);
                } else {
                    printf("\\x%02x", c);
                }
        }
        n++;
    }
    putchar('\n');
}


void pstr_pprint(pstr *ps) {
    if (ps == NULL || ps->buf == NULL) {
        printf("<pstr: NULL>\n");
        return;
    }
    int n = 0;
    while (n < ps->length) {
        unsigned char c = ps->buf[n];
        putchar(c);
        n++;
    }
    putchar('\n');
}

/**
 * see pstr_popright(), pstr_popleft()
*/
pstr* pstr_pop(pstr *ps, size_t n, int type) {
    if (n > ps->length) {
        n = ps->length;
    }

    char *poped = (char *)malloc(n+1);
    if (poped == NULL) {
        return NULL;
    }

    if (type == POPRIGHT) {
        memmove(poped, ps->buf+ps->length-n, n+1);
        pstr_resize_length(ps, ps->length-n);
    } else {
        memmove(poped, ps->buf, n);
        poped[n+1] = '\00';

        memmove(ps->buf, ps->buf+n, ps->length-n);
        pstr_resize_length(ps, ps->length-n);
    }

    return pstr_new_raw(poped, n);
}

/**
 * right pop n length pstr
 * 
 * If a pstr1 containing "ABCDEF" and calls pop(ps, 2), the pstr1 
 * will be modified in place so that it will contain "ABCD" (with null terminator) and a 
 * new, pstr2, containing "EF" returns
 * 
*/
pstr* pstr_popright(pstr *ps, size_t n) {
    return pstr_pop(ps, n, POPRIGHT);
}

/**
 * left pop n length pstr
 * 
 * If a pstr1 containing "ABCDEF" and calls pop(ps, 2), the pstr1 
 * will be modified in place so that it will contain "ABCD" (with null terminator) and a 
 * new, pstr2, containing "AB" returns
 * 
*/
pstr* pstr_popleft(pstr *ps, size_t n) {
    return pstr_pop(ps, n, POPLEFT);
}

/**
 * Compare two pstr
 * 
 * return 0 if length and buff is equal, else -1
*/
int pstr_cmp(pstr *ps1, pstr *ps2) {
    if (pstr_len(ps1) == pstr_len(ps2)) {
        return memcmp(ps1->buf, ps2->buf, pstr_len(ps1));
    }
    return -1;
}

void pstr_resize_length(pstr *ps, size_t length) {
    ps->length = length;
    ps->buf[length] = '\00';
}

size_t pstr_len(pstr *ps) {
    return ps->length; 
}

/**
 * Find the first occurence of a substring 
 * 
 * Function is binary safe and will not threat any null byte as terminator
 * Return -1 if no match found
*/
int pstr_find(pstr* ps, pstr* subps) {
    // If the substring is longer than the string, it can't be found
    if (subps->length > ps->length) {
        return -1;
    }

    for (size_t i = 0; i <= ps->length - subps->length; ++i) {
        // Check if substr->buf matches str1->buf starting at position i
        if (memcmp(ps->buf + i, subps->buf, subps->length) == 0) {
            // Match found
            return i;
        }
    }

    // No match found
    return -1;
}

/**
 * Returns a bundle pstr, [pstr1][pstr2][pstr3]...
 * 
 * Each pstr is then freed
*/
pstr* pstr_load_macro(pstr* ps1, ...) {
    va_list args;
    va_start(args, ps1);

    pstr* val = pstr_new("");
    pstr* ps = ps1;
    do {
        pstr_cat_pstr(val, ps);
        pstr_free(ps);
        ps = va_arg(args, pstr*);
    } while(ps != NULL);

    va_end(args);
    return val;
}


void pstr_free(pstr *ps) {
    if (ps == NULL || ps->buf == NULL) { return; }
    free(ps->buf);
}