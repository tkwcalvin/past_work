/*
    Fall 2024
    Simplified Linux Shell (MyShell)

    Name:Tang, Kewei
    Email:ktangaj@connect.ust.hk

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> // For constants that are required in open/read/write/close syscalls
#include <sys/wait.h> // For wait() - suppress warning messages
#include <fcntl.h>    // For open/read/write/close syscalls
#include <signal.h>   // For signal handling

// Define template strings so that they can be easily used in printf
//
// Usage: assume pid is the process ID
//
//  printf(TEMPLATE_MYSHELL_START, pid);
//
#define TEMPLATE_MYSHELL_START "Myshell (pid=%d) starts\n"
#define TEMPLATE_MYSHELL_END "Myshell (pid=%d) ends\n"
#define TEMPLATE_MYSHELL_TERMINATE "Myshell (pid=%d) terminates by Ctrl-C\n"

// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LENGTH 256

// Assume that we have at most 8 arguments
#define MAX_ARGUMENTS 8

// Assume that we only need to support 2 types of space characters:
// " " (space) and "\t" (tab)
#define SPACE_CHARS " \t"

// The pipe character
#define PIPE_CHAR "|"

// Assume that we only have at most 8 pipe segements,
// and each segment has at most 256 characters
#define MAX_PIPE_SEGMENTS 8

// Assume that we have at most 8 arguments for each segment
// We also need to add an extra NULL item to be used in execvp
// Thus: 8 + 1 = 9
//
// Example:
//   echo a1 a2 a3 a4 a5 a6 a7
//
// execvp system call needs to store an extra NULL to represent the end of the parameter list
//
//   char *arguments[MAX_ARGUMENTS_PER_SEGMENT];
//
//   strings stored in the array: echo a1 a2 a3 a4 a5 a6 a7 NULL
//
#define MAX_ARGUMENTS_PER_SEGMENT 9

// Define the standard file descriptor IDs here
#define STDIN_FILENO 0  // Standard input
#define STDOUT_FILENO 1 // Standard output

// This function will be invoked by main()
// This function is given
int get_cmd_line(char *command_line)
{
    int i, n;
    if (!fgets(command_line, MAX_CMDLINE_LENGTH, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(command_line);
    command_line[--n] = '\0';
    i = 0;
    while (i < n && command_line[i] == ' ')
    {
        ++i;
    }
    if (i == n)
    {
        // Empty command
        return -1;
    }
    return 0;
}

// read_tokens function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *pipe_segments[MAX_PIPE_SEGMENTS]; // character array buffer to store the pipe segements
// int num_pipe_segments; // an output integer to store the number of pipe segment parsed by this function
// char command_line[MAX_CMDLINE_LENGTH]; // The input command line
//
// Sample usage:
//
//  read_tokens(pipe_segments, command_line, &num_pipe_segments, "|");
//
void read_tokens(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}





void process_pipe_segment(char* pipe_segment){
    int argument_num = 0;
    char* arguments[MAX_ARGUMENTS_PER_SEGMENT];
    read_tokens(arguments, pipe_segment, &argument_num, SPACE_CHARS);
    arguments[argument_num]=NULL;
    //printf("%d\n", argc);
    for (int i=0; i<argument_num; i++){
        if (!strcmp(arguments[i],"<")){
            //printf("1\n");
            int fd = open(arguments[i+1], O_RDONLY, S_IRUSR | S_IWUSR);
            dup2(fd, STDIN_FILENO);
            close(fd);
            arguments[i]=NULL;
        }
        else if (!strcmp(arguments[i],">")){
            int fd = open(arguments[i+1], O_CREAT|O_WRONLY, S_IRUSR | S_IWUSR);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            arguments[i]=NULL;
        }
    }
    
    if (execvp(arguments[0],arguments)==-1){
        exit(1);
    }

}



void process_cmd(char *command_line){
    //printf("Debug: The command line is [%s]\n", command_line);
    char* pipe_segments[MAX_PIPE_SEGMENTS];
    int num_pipe_segments;
    read_tokens(pipe_segments, command_line, &num_pipe_segments, PIPE_CHAR);

    int pfds[2];
    for (int i = 0; i < num_pipe_segments; i++) {
        if (i==num_pipe_segments-1){
            process_pipe_segment(pipe_segments[i]);
        }
        else {
            pipe(pfds);
            pid_t pid = fork();
            if (pid==0){
                close(STDOUT_FILENO);
                dup2(pfds[1], STDOUT_FILENO);
                close(pfds[0]);
                process_pipe_segment(pipe_segments[i]);
            }
            else{
                close(STDIN_FILENO);
                dup2(pfds[0], STDIN_FILENO);
                close(pfds[1]);
                wait(0);
            }
        }
        
    }
}



void signal_callback(int sig){
    printf(TEMPLATE_MYSHELL_TERMINATE, getpid());
    exit(1);
}





int main()
{

    
    // You should replace it with your id
    char *prompt = "ktangaj";
    char command_line[MAX_CMDLINE_LENGTH];

    printf(TEMPLATE_MYSHELL_START, getpid());
    signal(SIGINT, signal_callback);

    // The main event loop
    while (1)
    {

        printf("%s> ", prompt);
        if (get_cmd_line(command_line) == -1)
            continue; /* empty line handling */

        if (strcmp(command_line, "exit") == 0) {
            printf(TEMPLATE_MYSHELL_END, getpid());
            break; // Exit the loop
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // the child process handles the command
            process_cmd(command_line);
        }
        else
        {
            // the parent process simply wait for the child and do nothing
            wait(0);
        }
    }

    return 0;
}
