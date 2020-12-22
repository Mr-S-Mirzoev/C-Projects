#include "command.h"
#include "utilities.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

int spawn_proc (int in, int out, struct command *cmd)
{
    pid_t pid;

    if ((pid = fork ()) == 0) { // in a child process
        if (in != 0) {
            dup2 (in, 0);
            close (in);
        }

        if (out != 1) {
            dup2 (out, 1);
            close (out);
        }

        return execvp (cmd->argv [0], (char * const *)cmd->argv); // never returns if exec doesn't fail
    }

    return pid; // pid of a child or -1 if fork failed
}

int command__init(struct command *self, const char *string) {
    if (self == NULL) {
        fprintf(stderr, "Pointer to a command was not allocated\n");
        return -1;
    }

    if (string == NULL) {
        fprintf(stderr, "Pointer to a string was not allocated\n");
        return -2;
    }

    size_t buf_len = strlen(string);

    if (buf_len == 0) {
        fprintf(stderr, "The passed string is empty\n");
        return -2;
    }

    int quote = 0; // ""
    int figure = 0; // {}
    int prev_cut = 0;

    cvector_vector_type(const char *) args = NULL;

    for (size_t index = 0; index < buf_len; ++index) {
        switch (string[index])
        {
        case ' ':
            //printf("%d %d %d %d\n", buf_len, prev_cut, quote, figure);
            
            if (!quote && !figure) {
                if (prev_cut != index) {
                    char *sub = new(char, index - prev_cut + 1);
                    snprintf(sub, index - prev_cut + 1, "%.*s", index - prev_cut, string + prev_cut);
                    cvector_push_back(args, sub);

                    prev_cut = index + 1;
                } else {
                    ++prev_cut;
                }
            }
            break;

        case '{':
            if (figure) {
                fprintf(stderr, "Error during parse of string\n");
                cvector_free(args);
                return -2;
            } else {
                if (index != buf_len - 1) {
                    prev_cut = index;
                    figure = 1;
                } else {
                    return -2;
                }
            }
            break;

        case '}':
            if (figure) {
                figure = 0;

                char *sub = new(char, index - prev_cut + 2);
                snprintf(sub, index - prev_cut + 2, "%.*s", index - prev_cut + 1, string + prev_cut);
                cvector_push_back(args, sub);

                prev_cut = index + 1;
            } else {
                fprintf(stderr, "Error during parse of string\n");
                cvector_free(args);
                return -2;
            }
            break;

        case '\"':
            if (!quote) {
                if (figure) {
                    fprintf(stderr, "Error during parse of string\n");
                    cvector_free(args);
                    return -2;
                } else {
                    prev_cut = index + 1;
                    quote = 1;
                }
            } else {
                if (figure) {
                    fprintf(stderr, "Error during parse of string\n");
                    cvector_free(args);
                    return -2;
                } else {
                    quote = 0;
                    char *sub = new(char, index - prev_cut + 1);
                    snprintf(sub, index - prev_cut + 1, "%.*s", index - prev_cut, string + prev_cut);
                    cvector_push_back(args, sub);

                    prev_cut = index + 1;
                }
            }
            break;
        
        default:
            break;
        } // switch
    } // for

    if (quote || figure) {
        fprintf(stderr, "Error during parse of string\n");
        cvector_free(args);
        return -2;
    }

    if (buf_len != prev_cut && buf_len != prev_cut + 1) {
        char *sub = new(char, buf_len - prev_cut);
        snprintf(sub, buf_len - prev_cut, "%.*s", buf_len - prev_cut - 1, string + prev_cut);
        cvector_push_back(args, sub);
    }

    cvector_push_back(args, NULL);

    self->argv = args;
}

int command__exec(struct command *self) {
    pid_t pid;

    if ((pid = fork ()) == 0) {
        int result;
    /*  
        if ((result = __executable__redirect_stream(self, redirect, INPUT_STREAM)) < 0) {
            execution_error(self->argv[0], "Failed to duplicate input fd", result);
        }

        if ((result = __executable__redirect_stream(self, redirect, OUTPUT_STREAM)) < 0) {
            execution_error(self->argv[0], "Failed to duplicate output fd", result);
        }

        if ((result = __executable__redirect_stream(self, redirect, ERROR_STREAM)) < 0) {
            execution_error(self->argv[0], "Failed to duplicate error fd", result);
        }
    */
        result = execvp (self->argv[0], (char * const *)self->argv);
        
        // Executes only if execvp fails.

        command__destroy(self);

        return result;
    }

    command__destroy(self);

    return pid;
}

void command__destroy(struct command *self) {
    if (self->argv) {
        const char **it;
        int i = 0;
        for (it = cvector_begin(self->argv); it != cvector_end(self->argv); ++it) {
            free(*it);
        }

        cvector_free(self->argv);
    }

    self->argv = NULL;
}