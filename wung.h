#ifndef WUNG_H
#define WUNG_H

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#if HAVE_STDINT_H
# include <stdint.h>
#endif
#include <stdlib.h>
#include<string.h>
#include<stdio.h>

typedef int wung_ulong;
typedef struct _wung_string{
    int len;
    char val[0];
} wung_string;

static wung_string * wung_string_init(char * str, int len) {
    wung_string *s =  (wung_string*) malloc(sizeof(wung_string) + len+10);
    memcpy(s->val, str, len);
    s->len = len;
    s->val[len] = '\0';
    return s;
}


#endif

