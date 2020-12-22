#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include "streams.h"

struct executable {
    char **_args;
};

// Public methods
void executable__init(struct executable *self, const char **args);
int executable__spawn_process(struct executable *self, struct streams *redirect);
void executable__destroy(struct executable *self);

// Private methods
int __executable__redirect_stream(struct executable *self, struct streams *redirect, int destination);

#endif // EXECUTABLE_H