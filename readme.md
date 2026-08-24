# CS330 Shell

A simple Unix shell implemented in C for the CS330 Operating Systems course.

## Features

* Displays the roll number and current working directory in the prompt
* Supports built-in commands:

  * `cd`
  * `pwd`
  * `echo`
  * `kill`
  * `exit`
* Executes external commands using `fork()` and `execvp()`
* Uses `waitpid()` for process synchronization
* Handles `SIGQUIT` for shell termination
* Supports exiting the shell using `Ctrl+\`
* Supports one command with zero or more arguments
* Does not implement pipes, redirection, or background execution

## Compilation

```bash
gcc shell.c -o 24110055
```

## Running

```bash
./24110055
```

Example:

```text
Shell started.
Press Ctrl+\ to exit the shell.

24110055:~/cs330-home/$ pwd
/home/ace/cs330-home

24110055:~/cs330-home/$ echo Hello World
Hello World

24110055:~/cs330-home/$ cd ..
24110055:~/$ pwd
/home/ace

24110055:~/$ ls
```

Press `Ctrl+\` to exit the shell.

## Built-in Commands

### `cd`

Changes the current working directory.

```text
cd <directory>
```

Example:

```text
24110055:~/$ cd /tmp
24110055:/tmp/$
```

### `pwd`

Displays the current working directory.

```text
pwd
```

### `echo`

Prints the given arguments.

```text
echo <text>
```

Example:

```text
24110055:~/$ echo Hello World
Hello World
```

### `kill`

Sends `SIGTERM` to the specified process.

```text
kill <pid>
```

### `exit`

Terminates the shell.

```text
exit
```

## External Commands

Commands that are not built-ins are executed using a child process.

The shell uses:

* `fork()` to create a child process
* `execvp()` to execute the requested command
* `waitpid()` to wait for the child process to finish

For example:

```text
24110055:~/$ ls -l
```

The shell creates a child process, the child executes `ls`, and the parent waits for the child to finish before displaying the next prompt.

## Signal Handling

The shell handles `SIGQUIT`. Pressing `Ctrl+\` sends `SIGQUIT` and terminates the shell.

```text
24110055:~/$
^\
Exiting shell...
Shell terminated.
```

## Concepts Used

* Process creation using `fork()`
* Program execution using `execvp()`
* Parent-child synchronization using `waitpid()`
* Signal handling using `SIGQUIT`
* Directory management using `chdir()` and `getcwd()`
* Command parsing using `strtok()`
* Process signaling using `kill()`

## Author

**Aryan Kumar**
Roll No: 24110055
