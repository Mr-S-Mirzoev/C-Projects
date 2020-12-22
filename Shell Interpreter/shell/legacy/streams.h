#ifndef STREAMS_H
#define STREAMS_H

struct streams {
    int fd_in;
    int fd_out;
    int fd_err;
};

enum stream_type {
    INPUT_STREAM,
    OUTPUT_STREAM,
    ERROR_STREAM
};

#endif // STREAMS_H