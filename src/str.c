#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "str.h"

void str_resize_length(str* ps, size_t n);
str* str_pop(str* ps, size_t n, int type);
str* str_grow(str *ps, size_t length);


#define POPRIGHT 0
#define POPLEFT 1

/**
 * Create a new str from a char*
 * 
 * Note that it expects a null terminated string. strlen terminates on any \x00
 * Use str_new_raw with a fixed length for a byte safe alternative
*/
str* str_new(char *init) {
    return str_new_raw(init, strlen(init));
}

/**
 * Create a binary safe str from a char*, expects a length 
 * 
*/
str* str_new_raw(char *init, size_t length) {
    str* ps = (str*) malloc(sizeof(str));

    ps->length = length;
    ps->capability = length + 1;
    ps->buf = malloc(ps->length);

    memcpy(ps->buf, init, length + 1);
    
    return ps;
}

/**
 * Empty str with a length
*/
str* str_new_rawempty(size_t length) {
    str* ps = (str*) malloc(sizeof(str));
    ps->length = length;
    ps->capability = length + 1;
    ps->buf = malloc(ps->length);
    return ps;
}


str* str_cpy(str *ps) {
    return str_new_raw(ps->buf, ps->length);
}

str* str_grow(str *ps, size_t length) {
    size_t newcap = length + ps->length;
    if (ps->capability > (newcap)) { return ps; }

    ps->buf = (char *)realloc(ps->buf, newcap);
    
    return ps;
}
/**
 * Concat str with another prst, modified in place
*/
str* str_cat_str(str *ps1, str* ps2) {
    return str_cat_raw(ps1, ps2->buf, ps2->length);
}

str* str_cat_raw(str *ps1, char *cs, size_t length) {
    if (length == 0) {
        return ps1;
    }
    ps1 = str_grow(ps1, length + 1);

    /* Copy new str including nul byte */
    memcpy(ps1->buf + ps1->length, cs, length + 1);
    ps1->length = ps1->length + length; 
    return ps1;
}

/**
 * Concates str ps1 with a char*, expects a null terminated char*
 * modified in place
*/
str* str_cat(str *ps1, char* cs) {
    return str_cat_raw(ps1, cs, strlen(cs));
}


/**
 * Print str, non printable characters will be displayed as hex (as \xFF)
 * does not include null terminator \x00
*/
void str_print(str *ps) {
    if (ps == NULL || ps->buf == NULL) {
        printf("<str: NULL>\n");
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


void str_pprint(str *ps) {
    if (ps == NULL || ps->buf == NULL) {
        printf("<str: NULL>\n");
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
 * see str_popright(), str_popleft()
*/
str* str_pop(str *ps, size_t n, int type) {
    if (n > ps->length) {
        n = ps->length;
    }

    char *poped = (char *)malloc(n+1);
    if (poped == NULL) {
        return NULL;
    }

    if (type == POPRIGHT) {
        memmove(poped, ps->buf+ps->length-n, n+1);
        str_resize_length(ps, ps->length-n);
    } else {
        memmove(poped, ps->buf, n);
        poped[n+1] = '\00';

        memmove(ps->buf, ps->buf+n, ps->length-n);
        str_resize_length(ps, ps->length-n);
    }

    return str_new_raw(poped, n);
}

/**
 * right pop n length str
 * 
 * If a str1 containing "ABCDEF" and calls pop(ps, 2), the str1 
 * will be modified in place so that it will contain "ABCD" (with null terminator) and a 
 * new, str2, containing "EF" returns
 * 
*/
str* str_popright(str *ps, size_t n) {
    return str_pop(ps, n, POPRIGHT);
}

/**
 * left pop n length str
 * 
 * If a str1 containing "ABCDEF" and calls pop(ps, 2), the str1 
 * will be modified in place so that it will contain "ABCD" (with null terminator) and a 
 * new, str2, containing "AB" returns
 * 
*/
str* str_popleft(str *ps, size_t n) {
    return str_pop(ps, n, POPLEFT);
}

/**
 * Compare two str
 * 
 * return 0 if length and buff is equal, else -1
*/
int str_cmp(str *ps1, str *ps2) {
    if (str_len(ps1) == str_len(ps2)) {
        return memcmp(ps1->buf, ps2->buf, str_len(ps1));
    }
    return -1;
}

void str_resize_length(str *ps, size_t length) {
    ps->length = length;
    ps->buf[length] = '\00';
}

size_t str_len(str *ps) {
    return ps->length; 
}

/**
 * Find the first occurence of a substring 
 * 
 * Function is binary safe and will not threat any null byte as terminator
 * Return -1 if no match found
*/
int str_find(str* ps, str* subps) {
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
 * Returns a bundle str, [str1][str2][str3]...
 * 
 * Each str is then freed
*/
str* str_load_macro(str* ps1, ...) {
    va_list args;
    va_start(args, ps1);

    str* val = str_new("");
    str* ps = ps1;
    do {
        str_cat_str(val, ps);
        str_free(ps);
        ps = va_arg(args, str*);
    } while(ps != NULL);

    va_end(args);
    return val;
}


void str_free(str *ps) {
    if (ps == NULL || ps->buf == NULL) { return; }
    free(ps->buf);
}