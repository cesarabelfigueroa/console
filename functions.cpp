#include "core.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>  
#include <iostream>

using namespace std;


PipeRedirect parse_command(int argc, char** argv, char** cmd1, char** cmd2,  char** err) {
  PipeRedirect result = NEITHER;
  int split = -1;

  for (int i=0; i<argc; i++) {
    if (strcmp(argv[i], "|") == 0) {
      result = PIPE;
      split = i;
    } else if (strcmp(argv[i], ">>") == 0) {
      result = REDIRECT;
      split = i;
    } else if (strcmp(argv[i], ">") == 0) {
      result = REDIRECT_OVERWRITE;
      split = i;
    }
  }

  if (result != NEITHER) {

    for (int i=0; i<split; i++){
      cmd1[i] = argv[i];
    }

    int count = 0;
    int count2 = 0;
    for (int i=split+1; i<argc; i++) {
      cmd2[count] = argv[i];
      count++;
    }
    for (int i=count+split; i<argc; i++) {
      err[count2] = argv[i];
      count2++;
    }
    if(strcmp(cmd2[0], err[0])==0){
      err[0]= (char*)"x";
    }
    cmd1[split] = NULL;
    cmd2[count] = NULL;
    err[count2] = NULL;
  }

  return result;
}

void pipe_cmd(char** cmd1, char** cmd2) {
  int fds[2];
  pipe(fds);
  pid_t pid;

  if (fork() == 0) {

    dup2(fds[0], 0);
    close(fds[1]);

    execvp(cmd2[0], cmd2);
    perror("execvp failed");

  } else if ((pid = fork()) == 0) {
    dup2(fds[1], 1);

    close(fds[0]);

    execvp(cmd1[0], cmd1);
    perror("execvp failed");

  } else{
    waitpid(pid, NULL, 0);
  }
}


int read_args(char **argv) {
  char *cstr;
  string arg;
  int argc = 0;

  while (cin >> arg) {
    if (quit(arg)) {
      cout << "Goodbye!\n";
      exit(0);
    }

    cstr = new char[arg.size()+1];
    strcpy(cstr, arg.c_str());
    argv[argc] = cstr;
    argc++;

    if (cin.get() == '\n')
      break;
  }

  argv[argc] = NULL;

  return argc;
}

void redirect_cmd(char** cmd, char** file, char** err) {
  int fds[2]; 
  int count;  
  int fd;     
  char c[2];     
  pid_t pid; 

  pipe(fds);

  if (fork() == 0) {

    fd = open(file[0], O_RDWR | O_CREAT, 0666);

    if (fd < 0) {
      printf("Error: %s\n", strerror(errno));
      return;
    }

    dup2(fds[0], 0);
    
    close(fds[1]);
    read(fd, c, 10000);
     while ((count = read(0, c, 1)) > 0){
      write(fd, c, 1); 
      cout<< "Hey";
     }

    execlp("echo", "echo", NULL);

 } else if ((pid = fork()) == 0) {
    
    if(strcmp(err[0], "2>&1")==0){
      dup2(fds[1], 2);
    }
    
    dup2(fds[1], 1);
    
    close(fds[0]);

    execvp(cmd[0], cmd);
    perror("execvp failed");

  } else {
    waitpid(pid, NULL, 0);
    close(fds[0]);
    close(fds[1]);
  }
}

void redirect_overwrite_cmd(char** cmd, char** file, char** err) {
  int fds[2]; 
  int count;  
  int fd;     
  char c[1];     
  pid_t pid; 
  
  pipe(fds);

  if (fork() == 0) {
    remove(file[0]);
    fd = open(file[0], O_RDWR | O_CREAT, 0666);

    if (fd < 0) {
      printf("Error: %s\n", strerror(errno));
      return;
    }

    dup2(fds[0], 0);
    
    close(fds[1]);
    write(fd, c, 0); 
     while ((count = read(0, c, 1)) > 0)
      write(fd, c, 1); 

    execlp("echo", "echo", NULL);

 } else if ((pid = fork()) == 0) {
    if(strcmp(err[0], "2>&1")==0){
      dup2(fds[1], 2);
    }
    dup2(fds[1], 1);
    close(fds[0]);

    execvp(cmd[0], cmd);
    perror("execvp failed");

  } else {
    waitpid(pid, NULL, 0);
    close(fds[0]);
    close(fds[1]);
  }
}

void run_cmd(int argc, char** argv) {
  pid_t pid;
  const char *amp;
  amp = "&";
  bool found_amp = false;

  if (strcmp(argv[argc-1], amp) == 0)
    found_amp = true;

  pid = fork();

  if (pid < 0)
    perror("Error (pid < 0)");

  else if (pid == 0) {
   
    if (found_amp) {
      argv[argc-1] = NULL;
      argc--;
    }

    execvp(argv[0], argv);
    perror("execvp error");

  } else if (!found_amp)
    waitpid(pid, NULL, 0); 
}


bool quit(string choice) {
  for (unsigned int i=0; i<choice.length(); i++)
    choice[i] = tolower(choice[i]);

  return (choice == "quit" || choice == "exit");
}