#include "pipe.h"
#include "redirections.h"
#include "utilities.h"

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE 1032

int test_pipe () {
    const char *ls[] = { "ls", "-l", 0 };
    const char *awk[] = { "awk", "{print $1}", 0 };
    const char *sort[] = { "sort", 0 };
    const char *uniq[] = { "uniq", 0 };

    struct command cmd [] = { {ls}, {awk}, {sort}, {uniq} };

    return fork_pipes (4, cmd, NULL);
}

int test_redirect() {
    printf("%ld\n", (long) get_redirect("ls -l > file.txt >> wow.ls", INPUT_STREAM));
    printf("%s\n", get_redirect("ls -l > file.txt >> wow.ls", OUTPUT_STREAM));
    printf("%s\n", get_redirect("ls -l > file.txt >> wow.ls", APPEND_STREAM));

    char s[] = "ls -l > file.txt >> wow.ls";
    printf("%ld %s\n", (long) get_redirect(s, INPUT_STREAM), s);
    printf("%s\n", get_redirect(s, OUTPUT_STREAM));
    printf("%s\n", s);
    printf("%s\n", get_redirect(s, APPEND_STREAM));
    printf("%s\n", s);
}

int main_loop()
{
    int ret = 0;
    // Allocation
    size_t bufsize = BUF_SIZE;
    char *buffer = new(char, bufsize);
    if (buffer == NULL) {
        fprintf(stderr, "Cannot allocate memory for a buffer\n");
        return 1;
    }

    size_t characters;

    printf("sergey@$hell: ");
    characters = getline(&buffer, &bufsize, stdin);
    //printf("%zu characters were read.\n", characters);
    //printf("Executing the string: '%s'\n", buffer);

    cvector_vector_type(const char *) args = parse_string(buffer, "|");
    if (args == NULL) {
        fprintf(stderr, "Some error occured: failed to parse arguments\n");
        ret = 1;
        goto clear_buff;
    }

    struct command *comm = new(struct command, cvector_size(args));
    struct streams *str = new(struct streams, 1);
    {
        int i = 0;
        char *s = new(char, strlen(args[i]));
        if (strcpy(s, args[i]) == NULL) {
            ret = 1;
            goto clear_all;
        }
        str->_in = get_redirect(s, INPUT_STREAM);

        command__init(&comm[i], s);
    }

    {
        int i = cvector_size(args) - 1;
        char *s = new(char, strlen(args[i]));
        if (strcpy(s, args[i]) == NULL) {
            ret = 1;
            goto clear_all;
        }
        str->_out = get_redirect(s, OUTPUT_STREAM);
        str->_app = get_redirect(s, APPEND_STREAM);

        command__init(&comm[i], s);
    }

    // TODO: make sure there are no programs with stream redirections

    ret = fork();
    if (ret == -1) {
        ret = 1;
        goto clear_all;
    } else if (ret == 0) {
        fork_pipes(cvector_size(args), comm, str);
    } else {
        wait(NULL);
        ret = 0;
    }

    // Dealloation
clear_all: 
    cvector_free(args);
clear_buff: 
    free(buffer);
    return ret;
}

int main(int argc, char const *argv[])
{
    int ret = 0;
    while (!ret) {
        ret = main_loop();
    }
    printf("%d\n", ret);
    fflush(stdout);
    exit(ret);
}
