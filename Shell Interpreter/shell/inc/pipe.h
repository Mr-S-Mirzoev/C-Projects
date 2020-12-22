#ifndef PIPE_H
#define PIPE_H

#include "command.h"
#include "redirections.h"

/*! \fn int fork_pipes (int n, struct command *cmd)
 * 
 *  \brief A function which starts a pipeline
 * 
 *  \param n Parameter which states number of elements in a pipeline.
 *  \param cmd Pointer to a command structure.
 */
int fork_pipes (int n, struct command *cmd, struct streams *redir);

#endif // PIPE_H