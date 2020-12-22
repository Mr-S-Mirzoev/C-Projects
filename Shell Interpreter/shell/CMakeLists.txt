cmake_minimum_required(VERSION 2.6.0)

# here we specify that the project is C language only, so the default
# C compiler on the system will be used
project(shell LANGUAGES C)

set(TARGETS src/shell.c 
            src/command.c 
            src/pipe.c
            src/utilities.c 
            src/redirections.c)

add_executable(shell ${TARGETS})

target_include_directories(shell PUBLIC inc/)
target_include_directories(shell PUBLIC dep/)