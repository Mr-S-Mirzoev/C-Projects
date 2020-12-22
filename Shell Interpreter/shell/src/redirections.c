#include "redirections.h"
#include "utilities.h"

#include <string.h>
#include <stdio.h>

char *get_redirect (char *string, int type) {
    char *pos_sign, *pos;
    if (string == NULL)
        return NULL;
    
    switch (type)
    {
    case INPUT_STREAM:
        pos_sign = strstr(string, "<");
        if (pos_sign) {
            pos = pos_sign + 1;
        } else {
            pos = NULL;
        }
        break;
    
    case OUTPUT_STREAM:
        pos_sign = strstr(string, ">");
        if (pos_sign) {
            pos = pos_sign + 1;
        } else {
            pos = NULL;
        }
        break;

    case APPEND_STREAM:
        pos_sign = strstr(string, ">>");
        if (pos_sign) {
            pos = pos_sign + 2;
        } else {
            pos = NULL;
        }
        break;

    default:
        pos = NULL;
        break;
    }

    if (pos) {
        while (*pos == ' ' && *pos) {
            ++pos;
        }

        if (!(*pos))
            return NULL;

        char *start = pos;

        while (*pos != ' ' && *pos) {
            ++pos;
        }

        size_t count = (pos - start);
        char *sub = new(char, count + 1);
        
        
        snprintf(sub, count + 1, "%.*s", count, start);
        remove_substr(string, (size_t) (pos_sign - string), (size_t) (pos - pos_sign));
        return sub;
    } else {
        return NULL;
    }
}