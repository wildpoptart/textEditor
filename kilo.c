/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
void die(const char *s){ //error handling
     perror(s);
     exit(1);
}

void disableRawMode(){
     if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
          die("tscetattr");
}

void enableRawMode(){
     if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcgetattr");
     atexit(disableRawMode);

     struct termios raw = orig_termios;
     raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON); // disable ctrl-s and ctrl-q | fix ctrl-m
     raw.c_oflag &= ~(OPOST); //disable output processing
     raw.c_cflag |= (CS8);
     raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // disbale ctrl-c ctrl-z with ISIG | disbale ctrl-v with IEXTEN

     raw.c_cc[VMIN] = 0;
     raw.c_cc[VTIME] = 1;

     if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/
int main(){
     enableRawMode();

     while (1){
          char c = '\0';
          if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read"); 
          if(iscntrl(c)){
               printf("%d\r\n", c);
          }else{
               printf("%d ('%c')\r\n", c, c);
          }
          if(c=='q') break;
     };
     return 0;
}