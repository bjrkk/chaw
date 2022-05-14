#pragma once
#define CHAW_COUNTOF(a) ( sizeof( (a) ) / sizeof( (a)[0] ) )

int chaw_strcasecmp(char const *lhs, char const *rhs);
int chaw_strncasecmp(char const *lhs, char const *rhs, size_t n);