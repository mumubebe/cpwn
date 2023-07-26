#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pstr.h"

void pstr_resize_length(pstr* ps, size_t n);
pstr* pstr_pop(pstr* ps, size_t n, int type);

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
 * Create a pstr,
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

pstr* pstr_grow(pstr *ps, size_t length) {
    size_t newcap = length + ps->length;
    if (ps->capability > (newcap)) { return ps; }

    ps->buf = (char *)realloc(ps->buf, newcap);
    
    return ps;
}

pstr* pstr_cat_pstr(pstr *ps, pstr* cs) {
    return pstr_cat_raw(ps, cs->buf, cs->length);
}

pstr* pstr_cat_raw(pstr *ps, char * cs, size_t length) {
    if (length == 0) {
        return ps;
    }
    ps = pstr_grow(ps, length + 1);

    /* Copy new str including nul byte */
    memcpy(ps->buf + ps->length, cs, length + 1);
    ps->length = ps->length + length; 
    return ps;
}

/**
 * Concates pstr with a char*, expects a null terminated char*
*/
pstr* pstr_cat(pstr *ps, char* cs) {
    return pstr_cat_raw(ps, cs, strlen(cs));
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
    } else {
        memmove(poped, ps->buf, n);
        poped[n+1] = '\00';
    }
    pstr_resize_length(ps, ps->length-n);

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


void pstr_free(pstr *ps) {
    if (ps == NULL || ps->buf == NULL) { return; }
    free(ps->buf);
}