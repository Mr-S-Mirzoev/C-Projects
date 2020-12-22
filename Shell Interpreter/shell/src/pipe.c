#include "pipe.h"

#include <unistd.h>
#include <fcntl.h>

int fork_pipes (int n, struct command *cmd, struct streams *redir)
{
    int i;
    pid_t pid;
    int in, fd [2];

    if (redir && redir->_in) {
        int inp = open(redir->_in, O_RDONLY);
        dup2(inp,0);
        close(inp);
    }

    /* The first process should get its input from the original file descriptor 0.  */
    in = 0;

    /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
    for (i = 0; i < n - 1; ++i) {
        pipe (fd);

        /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
        spawn_proc (in, fd [1], cmd + i);

        /* No need for the write end of the pipe, the child will write here.  */
        close (fd [1]);

        /* Keep the read end of the pipe, the next child will read from there.  */
        in = fd [0];
    }

    /* Last stage of the pipeline - set stdin be the read end of the previous pipe
        and output to the original file descriptor 1. */  
    if (in != 0)
        dup2 (in, 0);

    if (redir) {
        if (redir->_app) { // флаг на дозапись
            int out = open(redir->_app, O_CREAT | O_WRONLY | O_APPEND, 0666);
            dup2(out,1);
            close(out);
        } else if (redir->_out) { // флаг на перезапись
            int out = open(redir->_out, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            dup2(out,1);
            close(out);
        }
    }

    /* Execute the last stage with the current process. */
    return execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}