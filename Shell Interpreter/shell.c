#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>

enum { O_NOFL = 0, O_TRNC = 1, O_APND = 2, O_RDFL = 4, O_AFKM = 8 };

struct parsed_string
{
    char **x;
    int k;
};

void manual(void)
{
    printf("Welcome to shell by Kozhukh Polina 218\n");
    printf("Feel free to type in your commands after the appearance of\n");
    printf("$hell: \n");
    printf("When you're done type in 'exit' command and this will terminate shell.\n");
    printf("$hell: ");
}
//function to split user's string into commands
int getnparse(struct parsed_string *t)
{
    char *s = calloc(1002, sizeof(char));
    char **x = calloc (30, sizeof(char *));
    if (!fgets(s, 1000, stdin)) { //get user's string from standard input
        exit(0);
    }
    int l = strlen(s), k = 0, j = 0, i = 0;
    while (s[l - 1] == '\n' || isspace(s[l - 1])) { //loop to delete insignificant symbols in the end
        s[l - 1] = '\0';
        l--;
    }
    if (!strcmp(s,"exit")) { //exit from shell
        return 0;
    }
    x[k] = calloc(32, sizeof(char)); //current command
    while (i < l) { //loop through all elements of the input string
        if (isspace(s[i])) { //if space => end of current command
            x[k][j] = '\0'; //add the end of current command
            k++; //increase counter of commands on one
            j = 0;
            while (i < l && isspace(s[i])) { //loop through insignificant symbols
                i++;
            }
            x[k] = calloc(32, sizeof(char)); //new current command
        } else { //if not a space
            x[k][j] = s[i]; //put new symbol into current command
            j++;
            i++;
        }
    }
    if (!isspace(s[l - 1])) x[k][j] = '\0';
    k++;
    t->x = x; //put array of commands to struct
    t->k = k; //put number of commads to struct
    return 1;
}

//fuction to understand type of command
unsigned char get_type (struct parsed_string *x)
{
    unsigned char flags = O_NOFL; //'no flags' flag
    if (x->k < 2) return flags;
    if (!strcmp(x->x[1],"<")) { //'read from file' flag
        flags |= O_RDFL;
    }
    if (!strcmp(x->x[x->k - 1],"&")) { //'background' flag
        flags |= O_AFKM;
        x->k --;
    }
    if (x->k < 3) return flags;
    if (!strcmp(x->x[x->k - 2],">")) { //'write to file' flag
        flags |= O_TRNC;
    } else if (!strcmp(x->x[x->k - 2],">>")) { //'write to the end of file' flag
        flags |= O_APND;
    }
    return flags;
}

int kpr (struct parsed_string x)
{
    int k = 0;
    for (int i = 0; i < x.k; i++) {
        k += (strcmp(x.x[i], "|") == 0);
    }
    return k + 1;
}

void killall(pid_t *KidsPID, int k)
{
    for (int i = 0 ; i < k; i++) {
        kill(KidsPID[i],SIGKILL);
    }
    free(KidsPID);
}

void execute(struct parsed_string x, int a, int b)
{
    char *argv[20];
    for (int j = 0; j < (b - a); j ++) {
        argv[j] = x.x[a + j];
    }
    argv[b - a] = NULL;
    execvp(argv[0], argv);
}

int conveyor (struct parsed_string x)
{
    int kproc = kpr(x);
    int flags = get_type(&x);
    if (flags & O_AFKM) {
        if (!(flags & (O_RDFL))) {
            close(0);
        }
        if (!(flags & (O_APND | O_TRNC))) {
            close(1);
        }
    }
    if (kproc == 0) {
        exit(0);
    } else if (kproc == 1) {
        pid_t pid;
        int q = x.k;
        if (flags & (O_APND | O_TRNC)) {
            q -= 2;
        }
        if (flags & O_RDFL) {
            q -= 2;
        }
        if (!(pid = fork())) {
            execute(x, 0, q);
        } else if (pid == -1) {
            exit(1);
        } else {
            wait(NULL);
            exit(0);
        }
    } else {
        int tube[2], tube_o;
        if (pipe(tube) == -1) {
            _exit(1);
        }
        int *KidsPID = calloc(kproc + 2, sizeof(*KidsPID));
        int k = 0;
        if (!KidsPID) {
            close(tube[0]);
            close(tube[1]);
            _exit(1);
        }
        pid_t pid;
        int i = 0, a = i, b;
        while (i < x.k && strcmp(x.x[i],"|")) {
            i++;
        }
        b = i;
        i++;
        if (!(pid = fork())) {
            if (flags & O_RDFL) {
                b -= 2;
            }
            if (close(tube[0]) == -1) {
                close(tube[1]);
                free(KidsPID);
                _exit(1);
            } else if ((dup2(tube[1], 1) == -1)) {
                close(tube[1]);
                free(KidsPID);
                _exit(1);
            } else if (close (tube[1])) {
                free(KidsPID);
                _exit(1);
            } else {
                execute(x, a, b);
            }
        } else if (pid == -1) {
            free(KidsPID);
            exit(1);
        }
        if (close(tube[1]) == -1) {
            free(KidsPID);
            kill(pid,SIGKILL);
            wait(NULL);
            exit(1);
        } else {
            KidsPID[k] = pid;
            k++;
        }
        tube_o = tube[0];
        for (int j = 1; j < kproc - 1; j++) {
            fflush(stdout);
            a = i;
            while (i < x.k && strcmp(x.x[i],"|")) {
                i++;
            }
            b = i;
            i++;
            if (pipe(tube) == -1) {
                killall(KidsPID,k);
                while (wait(NULL) >= 0) {
                    //
                }
                _exit(1);
            }
            if (!(pid = fork())) {
                if ((close(tube[0]) == -1) || (dup2(tube_o,0) == -1) || (close(tube_o) == -1)) {
                    killall(KidsPID,k);
                    _exit(1);
                }
                if ((dup2(tube[1],1) == -1) || (close(tube[1]) == -1)) {
                    killall(KidsPID,k);
                    _exit(1);
                }
                execute(x, a, b);
            } else if (pid == -1) {
                killall(KidsPID,k);
                while (wait(NULL) >= 0) {
                    //
                }
                _exit(1);
            }
            if ((close(tube[1]) == -1) || (close(tube_o) == -1)) {
                killall(KidsPID,k);
                _exit(1);
            }
            KidsPID[k] = pid;
            k++;
            tube_o = tube[0];
        }
        if (!(pid = fork())) {
            b = x.k;
            if (flags & (O_APND | O_TRNC)) {
                b -= 2;
            }
            if ((dup2(tube_o,0) == -1) || (close(tube_o) == -1)) {
                free(KidsPID);
                _exit(1);
            }
            execute(x, i, b);
        } else if (pid == -1) {
            killall(KidsPID,k);
            while (wait(NULL) >= 0);
            exit(1);
        } else {
            KidsPID[k] = pid;
            k++;
            if (close(tube_o) == -1) {
                killall(KidsPID,k);
                while (wait(NULL) >= 0);
                exit(1);
            }
        }
    }
    int pid;
    while ((pid = wait(NULL)) != -1);
    exit(0);
}

//function to run a specific command
void request (struct parsed_string *x)
{
    unsigned char flags = get_type(x);
    pid_t pid;
    if ((pid = fork())) { //create new process
        //wait all processes
        while (wait(NULL)>=0);
    } else { //son-process
        if (flags == O_NOFL) {
            conveyor(*x); //run processes
        } else {
            if (flags & O_RDFL) {
                int inp = open(x->x[2], O_RDONLY);
                dup2(inp,0);
                close(inp);
            }
            if (flags & O_APND) {
                int out = open(x->x[x->k - 1], O_CREAT | O_WRONLY | O_APPEND, 0666);
                dup2(out,1);
                close(out);
            } else if (flags & O_TRNC) {
                int out = open(x->x[x->k - 1], O_CREAT | O_WRONLY | O_TRUNC, 0666);
                dup2(out,1);
                close(out);
            }
            conveyor(*x);
        }
    }
}

int main (void)
{
    manual();
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    sigaction(SIGCHLD, &(struct sigaction){ .sa_handler = SIG_DFL, .sa_flags = SA_NOCLDWAIT }, NULL);
    struct parsed_string x; //array of user's commads with number of them
    while ((getnparse(&x)) != 0) { //loop while not 'exit'
        if (get_type(&x) & O_AFKM) {
            if (!fork()) {
                if (!fork()) {
                    signal (SIGINT,SIG_IGN);
                    request(&x);
                    exit(0);
                }
                exit(0);
            }
        } else {
            request(&x);
        }
        signal (SIGINT,SIG_DFL);
        printf("$hell: "); //new in
    }
    return 0;
}
