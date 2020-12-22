#ifndef REDIRECTIONS_H
#define REDIRECTIONS_H

struct streams {
    char *_in;
    char *_out;
    char *_app;
};

enum stream_type {
    INPUT_STREAM,
    OUTPUT_STREAM,
    APPEND_STREAM
};

char *get_redirect (char *string, int type);

#endif // REDIRECTIONS_H