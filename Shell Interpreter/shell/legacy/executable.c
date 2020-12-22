#include "executable.h"
#include "utilities.h"

#include <unistd.h>

void executable__init(struct executable *self, const char **args) {
    self->_args = args;
}

int executable__spawn_process(struct executable *self, struct streams *redirect) {
    pid_t pid;

    if ((pid = fork ()) == 0) {
        int result;
        if ((result = __executable__redirect_stream(self, redirect, INPUT_STREAM)) < 0) {
            execution_error(self->_args[0], "Failed to duplicate input fd", result);
        }

        if ((result = __executable__redirect_stream(self, redirect, OUTPUT_STREAM)) < 0) {
            execution_error(self->_args[0], "Failed to duplicate output fd", result);
        }

        if ((result = __executable__redirect_stream(self, redirect, ERROR_STREAM)) < 0) {
            execution_error(self->_args[0], "Failed to duplicate error fd", result);
        }

        return execvp (self->_args[0], (char * const *)self->_args);
    }

    return pid;
}

void executable__destroy(struct executable *self) {
    self->_args = NULL;
}

int __executable__redirect_stream(struct executable *self, struct streams *redirect, int destination) {
    int redir_fd, ret_val = 0;

    switch (destination)
    {
    case INPUT_STREAM:
        if (redirect->fd_in != INPUT_STREAM) {
            redir_fd = redirect->fd_in;
            redirect->fd_in = -1;
        } else {
            return ret_val;
        }

        break;

    case OUTPUT_STREAM:
        if (redirect->fd_out != OUTPUT_STREAM) {
            redir_fd = redirect->fd_out;
            redirect->fd_out = -1;
        } else {
            return ret_val;
        }

        break;

    case ERROR_STREAM:
        if (redirect->fd_err != ERROR_STREAM) {
            redir_fd = redirect->fd_err;
            redirect->fd_err = -1;
        } else {
            return ret_val;
        }
        
        break;
    
    default:
        execution_error(self->_args[0], "Specified the inapropriate enum value", -1);
        break;
    }

    ret_val = dup2(redir_fd, destination);
    if (ret_val < 0) {
        execution_error(self->_args[0], "Failed to dup", ret_val);
    }

    ret_val = close(redir_fd);
    if (ret_val < 0) {
        execution_error(self->_args[0], "Failed to close", ret_val);
    }

    return ret_val;
}