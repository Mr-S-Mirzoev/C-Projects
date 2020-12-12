#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int MainPid, semid;
int *a;

// Just print it beautiful!
void BeautifulPrint(int kids, pid_t mykids[100], char *dirpath, double message)
{   
    struct sembuf x[1] = {{0,-1,0}};
    semop(semid, x, 1);
    int i;
    printf("Path = %s PID = %d PPID = %d", dirpath, getpid(), getppid());
    if (kids) {
        printf(" [");
    } else {
        printf(" %.2f\n", message);
    }
    for (i = 0; i < kids - 1; i++) {
        printf("%d ", mykids[i]);
    }
    if (kids) printf("%d] %.2f\n", mykids[kids - 1], message);
    struct sembuf y[1] = {{0,1,0}};
    semop(semid, y, 1);
}

// Generates coefficients for linear combination
int *generate_l_c (int k, int *sum)
{
    int *a = calloc(k + 1,sizeof(*a)); // The array of coefficients
    int i = 0, s = 0, max = INT_MAX / k; // i - indices; s - sum;
    // max is used to avoid overflow
    srand((unsigned) ((long long) time(NULL) * getpid() * (INT_MAX - getppid())));
    // setting the seed with the knowledge of proccessing time being too small for seed to vary
    for (; i < k; i++) {
        a[i] = rand() % max;
        if (a[i] < 0) a[i] = - a[i];
        s += a[i];
    }
    (*sum) = s;
    return a;
}

// Count the value by formula
double count (double *arr, int k)
{
    double x = 0; // The value in the numerator
    int s, i; // s - sum of coefficients; i - indices
    int *coef = generate_l_c(k,&s); // The array of coefficients
    for (i = 0; i < k; i++) {
        x += coef[i] * arr[i];
    }
    x /= s;
    free(coef);
    return x;
}

//Traversing the current directry and creating the process-tree function
void forktree (char *dirpath, int tof, int frf, int j)
// tof - pipe to send to father, frf - pipe to read from father
{
    //Openning the current directory
    DIR *d = opendir (dirpath);
    
    pid_t mykids [100];
    int kpipe[100][2]; //pipes to father
    int neopipe[100][2];
    int kids = 0, files = 0;
    if (!d) {
        fprintf(stderr,"Error opening directory %s.\n", dirpath);
        fflush(stderr);
        exit(1);
    }
    struct dirent *dd;
    
    //Reading the contains of the current directory.
    while ((dd = readdir(d))) {
        if (!strcmp(dd->d_name, ".") || !strcmp(dd->d_name, "..")) continue;
        char path[PATH_MAX];
        strcpy(path,dirpath);
        strcat(path,"/");
        strcat(path, dd->d_name);
        struct stat ifdir;
        stat (path,&ifdir);
        files ++;
        if (ifdir.st_mode & S_IFDIR) {
            //son-process
            int fd[2];
            pipe(fd);
            pipe(kpipe[kids]);
            pipe(neopipe[kids]);
            if (!fork()) {
                close(fd[0]);
                close(kpipe[kids][0]);
                close(neopipe[kids][1]);
                pid_t MyPid = getpid();
                write(fd[1],&MyPid,sizeof(pid_t));
                forktree(path,kpipe[kids][1],neopipe[kids][0], j);
                exit(0);
            } else {
                close(neopipe[kids][0]);
                close(fd[1]);
                close(kpipe[kids][1]);
                read(fd[0],&mykids[kids],sizeof(pid_t));
                kids++;
            }
        }
    }
    int flg = strcmp(".", dirpath), ind = 0, i,q;
    double x = (double) files;
    double *arr = calloc(kids + 2, sizeof(*arr));
    for (q = 0; q < j; q ++) {
        ind = 0;
        if (flg) write(tof,&x,sizeof(double));
        for (i = 0; i < kids; i++) {
            write(neopipe[i][1],&x,sizeof(double));
        }
        if (flg) {
            read(frf,&(arr[ind]),sizeof(double));
            ind ++;
        }
        for (i = 0; i < kids; i++) {
            read(kpipe[i][0],&(arr[ind]),sizeof(double));
            ind++;
        }
        arr[ind] = x;
        ind ++;
        x = count(arr,ind);
    }
    free(arr);
    if (flg) close(tof);
    if (flg) close(frf);
    BeautifulPrint(kids,mykids,dirpath, x);
    while (wait(NULL) >= 0);
    if (MainPid != getpid()) exit(0);
}

int main (int argc, char *argv[] )
{
    MainPid = getpid();
    char *endptr;
    int j = (int) strtol (argv[1],&endptr,10);
    srand(time(NULL));
    key_t key = rand();
    semid = semget(key, 1, IPC_CREAT);
    struct sembuf x[1] = {{0,1,0}};
    semop(semid, x, 1);
    forktree (".", 1, 1, j);
    while (wait(NULL) != -1) {
        // wait for all kids
    }
    semctl(semid, 0, IPC_RMID);
    return 0;
}
