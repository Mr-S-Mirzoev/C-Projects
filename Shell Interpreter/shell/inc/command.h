#ifndef COMMAND_H
#define COMMAND_H

#include "cvector.h"

/*! \struct command 
 * 
 *  \brief Structure to implement operations and save arguments for executable.
 * 
 */
struct command {
    const char **argv; 
};

/*! \fn    int spawn_proc (int in, int out, struct command *cmd)
 *  \brief This function creates a process which executes the designated programm
 * 
 *  \param in The file descriptor from which we have to get input stream.
 *  \param out The file descriptor to which we have to redirect output stream.
 *  \param cmd The pointer to struct \a command which stores it's arguments
 *  \return -1 if anything goes wrong.
 */
int spawn_proc (int in, int out, struct command *cmd);

/*! \fn    int command__init(struct command *self, const char *string)
 *  \brief This function creates a valid struct command from a string. 
 *  
 *  Note that memory for self parameter should be preallocated.
 * 
 *  \param self The file descriptor from which we have to get input stream.
 *  \param string The string to be parsed into a command type
 * 
 *  \return -1 if anything goes wrong with command -2 if something is wrong with string, 0 else
 */
int command__init(struct command *self, const char *string);

int command__exec(struct command *self);

void command__destroy(struct command *self);

#endif