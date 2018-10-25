
#include "pch.h"

/* demo.c
 * Interactive demo of libdict.
 * Copyright (C) 2001-2011 Farooq Mela */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include "dict.h"

static const char appname[] = "test";

char *xstrdup(const char *str);

#if defined(__GNUC__) || defined(__clang__)
# define NORETURN	__attribute__((__noreturn__))
#else
# define NORETURN
#endif

void quit(const char *, ...) NORETURN;
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xdup(const void *ptr, size_t size);

static void
key_val_free(void *key, void *datum)
{
    free(key);
    free(datum);
}

int main()
{
    srand(11);

    dict_malloc_func = xmalloc;

    dict* dct = hb_dict_new((dict_compare_func)strcmp);

    if (!dct)
	quit("can't create container");

    int cnt = 1;
    for (;;++cnt) {
	dict_verify(dct);
	char buff[10];
	sprintf(buff, "%03d", rand() % 1000);
	if (rand()%2)
	{
	    dict_insert_result result = dict_insert(dct, xstrdup(buff));
	    if (result.inserted) {
		*result.datum_ptr = xstrdup(buff);
	    }
	} else {
	    dict_remove_result result = dict_remove(dct, buff);
	    if (result.removed) {
		free(result.key);
		free(result.datum);
	    }
	}
	bool ok = dict_verify(dct);
	if (!ok)
	{
	    printf("tree not ok after %d step\n", cnt);
	    break;
	}
	if ((cnt%50000) == 0)
	{
	    dict_itor *itor = dict_itor_new(dct);
	    dict_itor_first(itor);
	    printf("new dump, count at %d, size at %zu\n", cnt, dict_count(dct) );
	    for (int i=1; dict_itor_valid(itor); dict_itor_next(itor))
	    {
		if ((i % 7) == 0)
		    printf("\n");
		else if (i>1)
		    printf(" ");
		printf("{%s:%s}",
		       (char *)dict_itor_key(itor),
		       (char *)*dict_itor_datum(itor));
		++i;
	    }
	    printf("\n");
	    dict_itor_free(itor);
	}

    }

    dict_free(dct, key_val_free);
}

char *
xstrdup(const char *str)
{
    return xdup(str, strlen(str) + 1);
}

void
quit(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "%s: ", appname);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(EXIT_FAILURE);
}

void *
xmalloc(size_t size)
{
    void *p = malloc(size);
    if (!p) {
	fprintf(stderr, "out of memory\n");
	abort();
    }
    return p;
}

void *
xdup(const void *ptr, size_t size)
{
    return memcpy(xmalloc(size), ptr, size);
}
