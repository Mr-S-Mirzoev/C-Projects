#ifndef UTILITIES_H
#define UTILITIES_H

#include "cvector.h"

/*! \fn cvector_vector_type(const char *) parse_string(const char *string, const char delimiter)
 * 
 *  \brief Here we parse a string by delimiter and return a vector of new strings.
 * 
 *  The vector is non-owning - the allocation and freing of memory is laid upon the user.
 * 
 *  \param string The string to be delimited.
 *  \param delimiter A string which contains a delimiter
 */
cvector_vector_type(const char *) parse_string(const char *string, const char *delimiter);

char *replace_multi_space_with_single_space(char *str);

void remove_substr(char *s, size_t p, size_t n);

#define new(type, count) (type *) malloc(count * sizeof(type))

#endif // UTILITIES_H