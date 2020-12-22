#include "utilities.h"

#include <string.h>
#include <stdio.h>

cvector_vector_type(const char *) parse_string(const char *string, const char *delimiter) {
    if (string == NULL || delimiter == NULL) {
        fprintf(stderr, "Parameter passed to parse_string is NULL\n");
        return NULL;
    }
    
    size_t len = strlen(string);
    char *string_copy = new(char, len);
    if (strcpy(string_copy, string) == NULL) {
        fprintf(stderr, "Failed to perform strcpy\n");
        return NULL;
    }

    cvector_vector_type(const char *) v = NULL;

    char *token = strtok(string_copy, delimiter);

    // loop through the string to extract all other tokens
    while (token != NULL) {
        //printf(".%s.\n", token);
        cvector_push_back(v, token);
        token = strtok(NULL, delimiter);
    }

    return v;
}

char *replace_multi_space_with_single_space(char *string)
{
    /* Destination to copy to */
    char *dest, *str;

    str = new(char, strlen(string));
    strcpy(str, string);
    dest = str;

    /* While we're not at the end of the string, loop... */
    while (*str != '\0')
    {
        /* Loop while the current character is a space, AND the next
         * character is a space
         */
        while (*str == ' ' && *(str + 1) == ' ')
            str++;  /* Just skip to next character */

       /* Copy from the "source" string to the "destination" string,
        * while advancing to the next character in both
        */
       *dest++ = *str++;
    }

    /* Make sure the string is properly terminated */    
    *dest = '\0';
}

void remove_substr(char *s, size_t p, size_t n)
{
    // ensure that we're not being asked to access
    // memory past the current end of the string.
    // Note that if p is already past the end of
    // string then p + n will, necessarily, also be
    // past the end of the string so this one check
    // is sufficient.
    if(p + n > strlen(s))
        return;

    // Offset n to account for the data we will be
    // skipping.  
    n += p;

    // We copy one character at a time until we 
    // find the end-of-string character
    while(s[n] != 0)
        s[p++] = s[n++];

    // And make sure our string is properly terminated.  
    s[p] = 0;
}