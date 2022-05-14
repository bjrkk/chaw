#include "util.h"
#include <ctype.h>

int chaw_strcasecmp(char const *lhs, char const *rhs) {
    char lc, rc;
    for (; *lhs != '\0' && *rhs != '\0'; lhs++, rhs++) {
        lc = tolower(*lhs);
        rc = tolower(*rhs);
        if (lc != rc) {
            break;
        }
    }

    return (unsigned char)lc - (unsigned char)rc;
}

int chaw_strncasecmp(char const *lhs, char const *rhs, size_t n) {
    char lc, rc;
    for (size_t i = 0; i < n; i++) {
        lc = tolower(lhs[i]);
        rc = tolower(rhs[i]);
        if (lc == '\0' || rc == '\0') {
			break;
		}
		if (lc != rc) {
            break;
        }
    }

    return (unsigned char)lc - (unsigned char)rc;
}