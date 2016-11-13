#ifndef WUNG_H
#define WUNG_H

#include <sys/types.h>
#include <inttypes.h>
#include <stdint.h>

#include <stdlib.h>
#include<string.h>
#include<stdio.h>

typedef uint64_t wung_ulong;
typedef struct _wung_string{
    int len;
    char val[0];
    wung_ulong h;
} wung_string;

static wung_string * wung_string_init(char * str, int len) {
    wung_string *s =  (wung_string*) malloc(sizeof(wung_string) + len+10);
    memcpy(s->val, str, len);
    s->len = len;
    s->val[len] = '\0';
    s->h = 0;
    return s;
}

static wung_ulong wung_hash_func(const char * str, int len) {
    long hash = 5381;
    for (; len >= 8; len -= 8) {
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
    }
    switch (len) {
        case 7: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 6: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 5: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 4: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 3: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 2: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 1: hash = ((hash << 5) + hash) + *str++; break;
        case 0: break;
    }
    return hash | 0x8000000000000000;
}

static wung_ulong wung_hash_string(wung_string * str) {
    if (str->h) {
        return str->h;
    }
    str->h = wung_hash_func(str->val, str->len);
    return str->h;
}
# define MAX_LENGTH_OF_LONG 20

#endif

