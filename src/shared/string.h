#ifndef TRCL_UTILS_H
#define TRCL_UTILS_H


#include <stddef.h>


char * str_alloc(size_t);
char * str_copy_alloc(char const *);
char * str_set_alloc(char *, char const *);
void str_destroy(char const *);
char * str_concat(char *, char const *);


#endif
