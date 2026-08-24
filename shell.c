#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <limits.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

volatile sig_atomic_t shell_running = 1;

/* Ctrl+\ handler */
void handle_exit(int sig)
{
    shell_running = 0;
    write(STDOUT_FILENO, "\nExiting shell...\n", 18);
}

/* Built-in: pwd */
void builtin_pwd()
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("pwd");
}

/* Built-in: echo */
void builtin_echo(char *args[])
{
    for (int i = 1; args[i] != NULL; i++)
    {
        printf("%s", args[i]);

        if (args[i + 1] != NULL)
            printf(" ");
    }

    printf("\n");
}

/* Built-in: cd */
void builtin_cd(char *args[])
{
    char *path;

    if (args[1] == NULL)
    {
        /* cd with no argument -> home directory */
        path = getenv("HOME");

        if (path == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
    }
    else
    {
        path = args[1];
    }

    if (chdir(path) != 0)
        perror("cd");
}

/* Built-in: kill */
void builtin_kill(char *args[])
{
    if (args[1] == NULL)
    {
        printf("Usage: kill <pid>\n");
        return;
    }

    pid_t pid = atoi(args[1]);

    if (kill(pid, SIGTERM) == -1)
        perror("kill");
}

/* Check and execute built-in commands */
int execute_builtin(char *args[])
{
    if (args[0] == NULL)
        return 1;

    if (strcmp(args[0], "cd") == 0)
    {
        builtin_cd(args);
        return 1;
    }

    if (strcmp(args[0], "pwd") == 0)
    {
        builtin_pwd();
        return 1;
    }

    if (strcmp(args[0], "echo") == 0)
    {
        builtin_echo(args);
        return 1;
    }

    if (strcmp(args[0], "kill") == 0)
    {
        builtin_kill(args);
        return 1;
    }

    if (strcmp(args[0], "exit") == 0)
    {
        shell_running = 0;
        return 1;
    }

    return 0;
}

/* Tokenize input */
int parse_command(char *input, char *args[])
{
    int argc = 0;

    char *token = strtok(input, " \t\n");

    while (token != NULL && argc < MAX_ARGS - 1)
    {
        args[argc++] = token;
        token = strtok(NULL, " \t\n");
    }

    args[argc] = NULL;

    return argc;
}

/* Execute external command */
void execute_external(char *args[])
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        /* Child process */
        execvp(args[0], args);

        /* execvp only returns if there is an error */
        perror("exec");
        exit(EXIT_FAILURE);
    }
    else
    {
        /* Parent process */
        int status;

        if (waitpid(pid, &status, 0) == -1)
            perror("waitpid");
    }
}

/* Print shell prompt */
void print_prompt()
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return;
    }

    char *home = getenv("HOME");

    if (home != NULL &&
        strncmp(cwd, home, strlen(home)) == 0)
    {
        char display_path[PATH_MAX];

        snprintf(
            display_path,
            sizeof(display_path),
            "~%s",
            cwd + strlen(home)
        );

        printf("24110055:%s/$ ", display_path);
    }
    else
    {
        printf("24110055:%s/$ ", cwd);
    }

    fflush(stdout);
}

int main()
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_exit;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGQUIT, &sa, NULL);

    /*
     * SIGQUIT can also be used as an exit signal.
     * Ctrl+\ normally generates SIGQUIT.
     */

    printf("Shell started.\n");
    printf("Press Ctrl+\\\\ to exit the shell.\n");

    while (shell_running)
    {
        print_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        if (!shell_running)
            break;

        if (strlen(input) == 0)
            continue;

        int argc = parse_command(input, args);

        if (argc == 0)
            continue;

        /*
         * Built-in commands are executed directly
         * by the shell process.
         */
        if (execute_builtin(args))
            continue;

        /*
         * Everything else is treated as an external command.
         */
        execute_external(args);
    }

    /*
     * Restore terminal software flow control.
     */
    system("stty ixon");

    printf("Shell terminated.\n");

    return 0;
}