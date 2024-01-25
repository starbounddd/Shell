#include <fcntl.h>
#include <libgen.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
//#include "./jobs.h"
#include <signal.h>
#include <time.h>


//job_list_t *job_list;

/**
 Method was used to check for if an argument provided was command, but i moved
 this functionality to execute_command for simplification purposes as it caused
 a bug
*/

int is_command(char *command) {
    if (strcmp(command, "cd") == 0 || strcmp(command, "ln") == 0 ||
        strcmp(command, "rm") == 0 || strcmp(command, "exit") == 0) {
        return 1;
    }
    return 0;
}

/*
Helper function for finding a redirection token that is the 0th position of
tokens array*/

int is_redirect(char *command) {
    if (strcmp(command, ">") == 0 || strcmp(command, ">>") == 0 ||
        strcmp(command, "<") == 0) {
        return 1;
    }
    return 0;
}
/*Helper functio for finding a redirection token in any position within tokens
 * array*/
int is_redirection(char *tokens[]) {
    int i;
    for (i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], "<") == 0 ||
            strcmp(tokens[i], ">>") == 0) {
            return 1;
        }
    }
    return 0;
}


//4.1 - 4.3 ignoring signals 
void set_signal_behavior(void (*sig)(int)) {
    sigset_t old;
    sigset_t full;
    sigfillset(&full);

    // Ignore signals while installing handlers by masking
    sigprocmask(SIG_SETMASK, &full, &old);

    // Set behavior--while signal can error it only errors based on invalid
    // signum, which will literally never happen here
    signal(SIGINT, sig);
    signal(SIGTSTP, sig);
    signal(SIGTTOU, sig);

    // Restore signal mask to previous value
    sigprocmask(SIG_SETMASK, &old, NULL);
}
/*
Provides the prompt for shell when the PROMPT macro is called when using
Makefile
*/

void printPrompt() {
#ifdef PROMPT
    if (printf("33sh> ") < 0) {
        fprintf(stderr, "Error");
    }
    if (fflush(stdout) < 0) {
        fprintf(stderr, "Error");
    }
#endif
}
/*Helepr function that runs child processes for external commands*/

void external_command(char *tokens[], char *argv[], int is_bg) {
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
    } else if (child_pid == 0) {
         int outputTokens = 0;
         int inputTokens = 0;
        for (int i = 0; tokens[i] != NULL; i++) {
            if (strcmp(tokens[i], ">") == 0) {
                outputTokens++;
                if (outputTokens >= 2) {
                    fprintf(stderr, "Too many output tokens\n");
                }
                if (tokens[i + 1] == NULL) {
                    fprintf(stderr, "Need output file path\n");
                } else {
                    close(1);
                    int file =
                        open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    if (file < 0) {
                        perror("open");
                        exit(1);
                    }
                    // dup2(file, STDOUT_FILENO);
                    // close(file);
                }

            }

            else if (strcmp(tokens[i], ">>") == 0) {
                outputTokens++;
                if (outputTokens >= 2) {
                    fprintf(stderr, "Too many output tokens\n");
                }
                if (tokens[i + 1] == NULL) {
                    fprintf(stderr, "Need output file path\n");
                } else {
                    // printf("check test 15");
                    close(1);
                    int file = open(tokens[i + 1],
                                    O_WRONLY | O_CREAT | O_APPEND, 0666);
                    if (file < 0) {
                        perror("open");
                        exit(1);
                    }
                    // dup2(file, STDOUT_FILENO);
                    // close(file);
                }

            } else if (strcmp(tokens[i], "<") == 0) {
                inputTokens++;
                if (inputTokens >= 2) {
                    fprintf(stderr, "Too many output tokens\n");
                }
                if (tokens[i + 1] == NULL) {
                    fprintf(stderr, "Need input file path\n");
                } else {
                    printf("check test 15");
                    close(0);
                    int file = open(tokens[i + 1], O_RDONLY);
                    if (file < 0) {
                        perror("open");
                        exit(1);
                    }
                    // dup2(file, STDIN_FILENO);
                    // close(file);
                }
            }
        }
        if (is_redirect(tokens[0]) == 1) {
            //   printf("Executing command: %s\n", basename(tokens[2]));
            execv(tokens[2], argv);
            perror("execv");
            exit(1);

        } else {
            execv(tokens[0], argv);
            perror("execv");
            exit(1);
        }

        if(!is_bg){

        }
    
    wait(0);
}
}

        // char *arguments[] = {tokens[0], argv};
          //4.3 error checking if there is a problem in setting the process id for a group of processes 
        // setpgid(0,0);
            // printf("Error in setting process ID");
            // exit(1);
        
        // if(is_bg != 1){
        //     //give terminal control to if its a foreground process
        //     if(tcsetpgrp(STDIN_FILENO,getpid()) < 0){
        //     printf("Error in sending terminal control to appropriate processes");
        //     //exit(1);
        //     }
        // }



        //4.2 - set up signal handlers to set the behavior of all the signals ignored in the 
        //parent process back to their default behavior
    
//         signal(SIGINT,SIG_DFL);
//         signal(SIGTTOU,SIG_DFL);
//         signal(SIGTSTP,SIG_DFL);

      
//         execv(tokens[0], argv);
//         // perror("execv");
//         exit(1);
//     }
//     wait(0);
// }

/*
Helper function used in execute_command responsible for changing directories
when "cd" is used as an argument
*/
void change_directory(char *arg1, char *arg2) {
    if (arg1 == NULL) {
        fprintf(stderr, "Please enter a correct path\n");
    } else if (arg2 != NULL) {
        fprintf(stderr, "Syntax error\n");
    } else {
        if (chdir(arg1) != 0) {
            perror("chdir");
        }
    }
}
/*Helper function used in execute_command responsible for exiting out of shell
 * when "exit" is used as an argument*/
void exit_command() { exit(0); }
/*Helper function used in execute_command responsible for linking a path to a
 * file when "ln" is used as an argument*/
void link_command(char *arg1, char *arg2, char *arg3) {
    if (arg1 == NULL || arg2 == NULL || arg3 != NULL) {
        fprintf(stderr, "Syntax error\n");
    } else {
        if (link(arg1, arg2) == -1) {
            perror("link");
        }
    }
}
/*Helper function used in execute_command responsible for removing files when
 * "rm" is used as an argument*/
void remove_command(char *arg1, char *arg2) {
    if (arg1 == NULL || arg2 != NULL) {
        fprintf(stderr, "Syntax error\n");
    } else {
        if (unlink(arg1) == -1) {
            perror("rm");
        }
    }
}


/*Overarching function that uses the helper functions to identiy and execute
specific built in functions of shell when a specific command is produced*/

void execute_command(char *argv[512],char *tokens[], int is_bg ) {
    if (strcmp(argv[0], "cd") == 0) {
        change_directory(argv[1], argv[2]);
    } else if (strcmp(argv[0], "ln") == 0) {
        link_command(argv[1], argv[2], argv[3]);
    } else if (strcmp(argv[0], "rm") == 0) {
        remove_command(argv[1], argv[2]);
    } else if (strcmp(argv[0], "exit") == 0) {
        exit_command();
    }
//    else if (strcmp(argv[0], "jobs") == 0) {
//         jobs(job_list);
//         return;
//     }
    else{
        external_command(tokens,argv, is_bg);
    }
}
/*
Main function for doing the several functionalities of different redirection
tokens including append, output, and input*/
// void redirection_command(char *tokens[], char *argv[]) {
// }
void parse(char buffer[1024], char *tokens[512], char *argv[512], int is_bg) {
    int tokenPointer;
    int argvPointer;
    char *tokenBuffer =
        strtok(buffer, " \t");  // This returns the first token in the buffer
    if (tokenBuffer != NULL) {
        tokens[0] = tokenBuffer;

        char *pointer_after_backslash = strrchr(tokenBuffer, '/');
        if (pointer_after_backslash == NULL && (is_redirect(tokens[0]) == 0)) {
            argv[0] = tokens[0];
        } else {  // This is incrementing the pointer to the character that
                  // comes after the backslash
            argv[0] = ++pointer_after_backslash;
            if (pointer_after_backslash == NULL) {
                argv[0] = "";
            }
        }
    } else  // Null check, then there is nothing to parse, so the function ends
    {
        return;
    }
    if (is_redirect(tokens[0]) == 1) {
        int i = 0;
        int j = 0;

        tokenBuffer = strtok(NULL, " \t");
        i++;                                // increment token index
        tokens[i] = tokenBuffer;            // add file to token array
        tokenBuffer = strtok(NULL, " \t");  // skip to third element
        i++;                                // third position of tokens index
        char *temp = basename(tokenBuffer);
        argv[0] = temp;  // adds third element to argv but we need to get the
                         // last thing
        tokens[i] = tokenBuffer;  // adds that path to tokens array
        j++;                      // start argv at 1
        argvPointer = j;
        tokenPointer = i;
    } else {
        argvPointer = 1;
        tokenPointer = 1;
    }

    // Populating both arrays with tokens
    //    int i = 1;  // the pointer for the tokens array
    //    int j = 1;  // the pointer for the argv array

    if (is_redirect(tokens[0]) == 1) {
        tokenPointer = 3;
    }

    tokenBuffer = strtok(NULL, " \t");
    while (tokenBuffer != NULL) {
        if (strcmp(tokenBuffer, ">") == 0 || strcmp(tokenBuffer, "<") == 0 ||
            strcmp(tokenBuffer, ">>") == 0) {
            tokens[tokenPointer] = tokenBuffer;
            tokenPointer++;
            // skip the file name after redirect token
            tokenBuffer = strtok(NULL, " \t");
            if (tokenBuffer != NULL) {
                tokens[tokenPointer] = tokenBuffer;
                tokenPointer++;
            } else {
                fprintf(stderr, "error \n");
                exit(1);
            }
        } else {
            tokens[tokenPointer] = tokenBuffer;
            argv[argvPointer] = tokenBuffer;
            tokenPointer++;
            argvPointer++;
        }
        tokenBuffer = strtok(NULL, " \t");
    }
    if (tokenBuffer == NULL && strcmp(tokens[tokenPointer -1],"&") == 0){
        //checking for if a bg process exists 
        is_bg = 1;
        tokens[tokenPointer -1] = NULL;
        argv[argvPointer -1] = NULL;

    }

    argv[argvPointer] = NULL;
     if ((strcmp(tokens[0], "/bin/rm") != 0)) {
            execute_command(argv,tokens,is_bg);  }

}

// double for loop to remove redirection characters outside of
// for(int i = 0; i < 510;i ++){
//   if (argv[i] == ">" || argv[i] == ">>" || argv[i] == "<" ){
//     argv[i] == NULL;
//     argv[i+1] == NULL;
//   }
// }
// void reap() {
//     pid_t curr_pid;
//     int job_jid;
//     int status;

//     // waitpid on all processes to check statuses
//     while ((curr_pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) >
//            0) {
//         if (WIFEXITED(status)) {
//             // If exited, remove from job list and print
//             job_jid = get_job_jid(job_list, curr_pid);
//             remove_job_pid(job_list, curr_pid);
//             printf("[%d] (%d) terminated with exit status %d\n", job_jid,
//                    curr_pid, WEXITSTATUS(status));
//         } else if (WIFSIGNALED(status)) {
//             // If signaled, remove from job list and print
//             job_jid = get_job_jid(job_list, curr_pid);
//             remove_job_pid(job_list, curr_pid);
//             printf("[%d] (%d) terminated by signal %d\n", job_jid, curr_pid,
//                    WTERMSIG(status));
//         } else if (WIFCONTINUED(status)) {
//             // If continued, update job status and print
//             update_job_pid(job_list, curr_pid, status);
//             job_jid = get_job_jid(job_list, curr_pid);
//             printf("[%d] (%d) resumed\n", job_jid, curr_pid);
//         } else if (WIFSTOPPED(status)) {
//             // If stopped, update job status and print
//             update_job_pid(job_list, curr_pid, status);
//             job_jid = get_job_jid(job_list, curr_pid);
//             printf("[%d] (%d)  suspended by signal %d\n", job_jid, curr_pid,
//                    WSTOPSIG(status));
//         }
//     }
// }

int main() {
    set_signal_behavior(SIG_IGN);
    char buf[1024];
    char *tokens[512];
    char *argv[512];
    size_t count = 1024;
   // int jobid_count = 0;
    int is_bg = 0;
    printPrompt();
   // job_list_t jobs_list = init_job_list();

    while (read(0, buf, count) > 0) {
       // reap();
        buf[strcspn(buf, "\n")] = '\0';
        // memset(buf, 0, 1024 * sizeof(char *));
        memset(tokens, 0, 1024 * sizeof(char *));
        //    memset(argv, 0, 1024 * sizeof(char *));
        parse(buf, tokens, argv,is_bg);
        printPrompt();
      
        if (argv[0] == NULL) {
            continue;
        }
       
   //    pid_t process = fork();


    // //    if(!is_bg){
    //         //turn over terminal control
    //        setpgid(process,process);
    //             //fprintf(stderr, "Setting process group ID error\n");
    //      //   }
    //        tcsetpgrp(STDIN_FILENO,process);
    //          //   fprintf(stderr, "Error in turning over terminal control to proccesses\n");
    //       //  }
    //         //status indicator 
    //         int s = 0;
    //        waitpid(process,&s, WCONTINUED | WUNTRACED);
    //           //  fprintf(stderr, "Error: Child processes have not completed to maintain in fg\n");
    //      //   }
    //        tcsetpgrp(STDIN_FILENO, getpid());
    //      //       fprintf(stderr, "Error: Cannot turnover terminal control to shell\n");
    //      //   }

    //         char *output[100];
    //         jobid_count++;
            // if(WIFEXITED(s) != 0){
            //     sprintf(output)
            // }

        //}
    }
    return 0;
}
