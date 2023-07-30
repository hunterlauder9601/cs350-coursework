#include "types.h"
#include "fcntl.h"
#include "user.h"

#define SH_PROMPT "xvsh> "
#define NULL (void *)0


void bufParser(char *buf, char **argv);

int process_one_cmd(char* buf, char** args);
static int backNum = 0;

#define MAXLINE 256

int main(int argc, char *argv[])
{
    char buf[MAXLINE] = {0};
    char *args[64] = {0};
    int n;
    printf(1, SH_PROMPT);  /* print prompt (printf requires %% to print %) */

    while ( (n = read(0, buf, MAXLINE)) != 0)
    {
        if (n == 1)                           /* no input at all, we should skip */
        {
            printf(1, SH_PROMPT);
            continue;
        }

        bufParser(buf, args);

        process_one_cmd(buf, args);

        printf(1, SH_PROMPT);

        memset(buf, 0, sizeof(buf));
    }

    exit();
}

int exit_check(char **args) {
    // your implementation here
    if(strcmp(args[0], "exit") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int makeFork(void) {
    int pid = fork();
    if(pid < 0) {
        printf(2,"ERROR: forking child process failed\n");
        exit();
    }
    return pid;
}

int process_normal(char **argv, int isBGP)
{
    int pid = makeFork();
    if(pid == 0) {
        int pidno = getpid();
        if(isBGP) {
            printf(1,"[pid %d] runs as a background process\n",pidno);
            printf(1, SH_PROMPT);
        }
        if(exec(*argv, argv) < 0) {
            char *str = 0;
            str = argv[0];
            if(*str != 0) {
                printf(2,"Cannot run this command %s\n",argv[0]);
            }
            exit();
        }
    }
    else if(isBGP == 0){
        wait();
    }
    return 0;
}

int pipeFunct(char **argv, int inPipe, int outPipe) {
    int pfds[2];
    int pid1;
    int pid2;
    if(pipe(pfds) < 0) {
        printf(2,"ERROR: pipe creation failed\n");
        exit();
    }
    if((pid1 = makeFork()) == 0){
        close(1);
        dup(pfds[1]);
        close(pfds[0]);
        close(pfds[1]);
        if(exec(argv[inPipe], argv) < 0) {
            char *str = 0;
            str = argv[inPipe];
            if(*str != 0) {
                printf(2,"Cannot run this command %s\n",argv[inPipe]);
            }
            exit();
        }
    }
    if((pid2 = makeFork()) == 0){
        close(0);
        dup(pfds[0]);
        close(pfds[0]);
        close(pfds[1]);
        char *newImage[2];
        newImage[0] = argv[outPipe];
        newImage[1] = 0;
        if(exec(argv[outPipe], newImage) < 0) {
            char *str = 0;
            str = argv[outPipe];
            if(*str != 0) {
                printf(2,"Cannot run this command %s\n",argv[outPipe]);
            }
            exit();
        }
    }
    close(pfds[0]);
    close(pfds[1]);
    wait();
    wait();
    return 0;
}
int redirFunct(char **argv, int inRedir, int outRedir) {
    int fd;
    int pid;
    if((fd = open(argv[outRedir], O_WRONLY | O_CREATE)) < 0){
        printf(2, "open %s failed\n", argv[outRedir]);
        exit();
    }

    if((pid = makeFork()) == 0) {
        close(1);
        dup(fd);
        char *newImage[2];
        newImage[0] = argv[inRedir];
        newImage[1] = 0;
        if(exec(*argv, newImage) < 0) {
            char *str = 0;
            str = argv[inRedir];
            if(*str != 0) {
                printf(2,"Cannot run this command %s\n",argv[inRedir]);
            }
            exit();
        }
    }
    close(fd);
    wait();

    return 0;
}

int process_one_cmd(char* buf, char** argv)
{
    int i = 0;
    int isBGP = 0;
    int inPipe = 0;
    int outPipe = 0;
    int inRedir = 0;
    int outRedir = 0;
    /* check special symbols */
    while (argv[i] != 0) {
        if(strcmp(argv[i], "&") == 0) {
            argv[i] = 0;
            isBGP = 1;
            backNum++;
            break;
        } else if(strcmp(argv[i], "|") == 0) {
            argv[i] = 0;
            inPipe = i-1;
            outPipe = i+1;
            break;
        } else if(strcmp(argv[i], ">") == 0) {
            argv[i] = 0;
            inRedir = i-1;
            outRedir = i+1;
            break;
        }
        i++;
    }

    /*Check buid-in exit command */
    if (exit_check(argv))
    {
        /*some code here to wait till all children exit() before exit*/
        // your implementation here
        for(int v = 0; v < backNum; v++) {
            wait();
        }
        exit();
    }

    // your code to check NOT implemented cases

    /* to process one command */
    if(inPipe != outPipe) {
        pipeFunct(argv, inPipe, outPipe);
    } else if(inRedir != outRedir) {
        redirFunct(argv, inRedir, outRedir);
    } else {
        process_normal(argv, isBGP);
    }

    return 0;
}

void bufParser(char *buf, char **argv) {
    while(*buf != 0) {
        while (*buf == ' ' || *buf == '\t' || *buf == '\n') {
            *buf++ = 0;
        }
        *argv++ = buf;
        while (*buf != 0 && *buf != ' ' && *buf != '\t' && *buf != '\n') {
            buf++;
        }
    }
    *argv = 0;
}
