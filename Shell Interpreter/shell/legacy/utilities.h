#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdlib.h>
#include <stdio.h>

#define new(type, count) (type) malloc(count * sizeof(type))
#define execution_error(executable_name, message, return_value) \
                                        fprintf(stderr, "EXCEPTION from %s: %s\n", executable_name, message); \
                                        return return_value;

#endif // UTILITIES_H