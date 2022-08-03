















#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;


const int MAX_ARGS = 500;
enum PipeRedirect {PIPE, REDIRECTIN,REDIRECTOUT, APPEND,COL,NEITHER};
PipeRedirect parse_command(int, char**, char**, char**,char**);
void pipe_cmd(char**, char**,char**);
int read_args(char**);
void redirect_cmd(char**, char**);
void run_cmd(int, char**);
bool want_to_quit(string);
