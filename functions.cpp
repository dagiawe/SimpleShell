

#include "general.h"

PipeRedirect parse_command(int argc, char** argv, char** cmd1, char** cmd2,char** cmd3) {

  PipeRedirect result = NEITHER;


  int split = -1;
  int num=0;
  int splitt=-1;

  for (int i=0; i<argc; i++) {


   

    if (strcmp(argv[i], "|") == 0) {
      result = PIPE;
      
      num++;
      if(num>=2){
        splitt=i;
        num=0;
        
      }else{
      split = i;
      }
     

   
    } else if (strcmp(argv[i], ";") == 0) {
      result = COL;
      split = i;
     
    }
  }

  
  if (result != NEITHER) {
    
    
    for (int i=0; i<split; i++){
      cmd1[i] = argv[i];
  
    }
    

  if(splitt != -1)
  {
        int count = 0;
    for (int i=split+1; i<splitt; i++){
      cmd2[count] = argv[i];
          count++;
      
    }
        
       int coun = 0;
    for (int i=splitt+1; i<argc; i++) {
      cmd3[coun] = argv[i];
           coun++;
           
      

    }
   
      cmd1[split] = NULL;
     cmd2[count] = NULL;
      cmd3[coun] = NULL;
    }
    else{
      
      
      int count = 0;
      for (int i=split+1; i<argc; i++) {
      cmd2[count] = argv[i];
                  count++;
     }
   

      cmd1[split] = NULL;
     cmd2[count] = NULL;
    
    }
  }

  // Return an enum showing whether a pipe, redirect, or neither was found.
  return result;
}

// This pipes the output of cmd1 into cmd2.
void pipe_cmd(char** cmd1, char** cmd2,char** cmd3) {
  //  Create the two pipes
    int pi[2];
    pipe(pi);
    int pi2[2];

    //  First child
    if ( fork() == 0 )
    {
        //  Set up the first pipe to be written to
        //  and close the unneeded pipe endpoints
        close(pi[0]);
        dup2(pi[1], 1);
        close(pi[1]);
        execvp( cmd1[0],cmd1 );
    }

    
    pipe(pi2);

    //  Second child
    
    if (fork() == 0 )
    {
        //  Set up first pipe to be read from
        //  and close the unneeded pipe endpoints
        close(pi[1]);
        dup2(pi[0], 0);
        close(pi[0]);
        //  Set up second pipe to be written to
        //  and close the unneeded pipe endpoints
        close(pi2[0]);
        dup2(pi2[1], 1);
        close(pi2[1]);
        execvp( cmd2[0],cmd2 );
    }
    //  Close original pipe endpoints so they aren't
    //  duplicated into the third child (where they would
    //  need to be closed).
    wait (NULL);
    close(pi[1]);
    close(pi[0]);

    //  Third child
    if ( fork() == 0 )
    {
        //  Set up second pipe to be read from
        //  and close the unneeded pipe endpoints
        close(pi2[1]);
        dup2(pi2[0], 0);
        close(pi2[0]);
        execvp( cmd3[0],cmd3 );
    }

    //  Close parent's copy of second pipes
    close(pi2[1]);
    close(pi2[0]);
      cout<<endl;
      }

// This will get input from the user, split the input into arguments, insert
// those arguments into the given array, and return the number of arguments as
// an integer.
int read_args(char **argv) {
  char *cstr;
  string arg;
  int argc = 0;

  // Read in arguments till the user hits enter
  while (cin >> arg) {
    // Let the user exit out if their input suggests they want to.
    
    if (want_to_quit(arg)) {
     
      exit(0);
    }

    // Convert that std::string into a C string.
    cstr = new char[arg.size()+1];
    strcpy(cstr, arg.c_str());
    argv[argc] = cstr;

    // Increment our counter of where we're at in the array of arguments.
    argc++;

    // If the user hit enter, stop reading input.
    if (cin.get() == '\n')
      break;
  }

  // Have to have the last argument be NULL so that execvp works.
  argv[argc] = NULL;

  // Return the number of arguments we got.
  return argc;
}

void redirect_cmd(char** cmd, char** file) {
  int fds[2]; // file descriptors
  int count;  // used for reading from stdout
  int fd;     // single file descriptor
  char c;     // used for writing and reading a character at a time
  pid_t pid;  // will hold process ID; used with fork()

  pipe(fds);

  
  if (fork() == 0) {
    
    fd = open(file[0], O_RDWR | O_CREAT, 0666);

    
    if (fd < 0) {
      printf("Error: %s\n", strerror(errno));
      return;
    }

    dup2(fds[0], 0);

       close(fds[1]);

    
    while ((count = read(0, &c, 1)) > 0)
      write(fd, &c, 1); // Write to file.

   
    execlp("echo", "echo", NULL);

  
  } else if ((pid = fork()) == 0) {
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

  // error
  if (pid < 0){
    perror("Error (pid < 0)");
  }  // child process
  else if (pid == 0) {
    // If the last argument is an ampersand, that's a special flag that we
    // don't want to pass on as one of the arguments.  Catch it and remove
    // it here.
    if (found_amp) {
      argv[argc-1] = NULL;
      argc--;
    }

    execvp(argv[0], argv);
    perror("execvp error");

  // parent process
  } else if (!found_amp)
    waitpid(pid, NULL, 0); // only wait if no ampersand
}


bool want_to_quit(string choice) {
  // Lowercase the user input
  for (unsigned int i=0; i<choice.length(); i++)
    choice[i] = tolower(choice[i]);

  return (choice == "quit" || choice == "exit");
}
