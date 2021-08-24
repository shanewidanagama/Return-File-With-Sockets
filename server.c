/*
===============================================================================
    Sockets Server Process

   Name: server.c

   Written by: Shane Damien Widanagama

   Date written: 4/2/2020

   Purpose: Makes use of sockets to receive a filepath from the client, access
   it, and send a copy back to the client.

   Usage: ./server &

   Description of parameters:
   The & sign tells the kernel to execute this process in the background.

   Libraries Required
     See the include statements.

   Modification History:
     4/2/2020 - documentation and testing
     4/1/2020 - input file processing and copying
     3/31/2020 - socket creation, binding, listening, accepting

   References:
      Completed with the help of class slides on sockets and Jacques Beland

===============================================================================
*/

#include <netinet/in.h> //structures and special IP addresses for binding
#include <string.h> //function to get length of strings
#include <stdio.h>  //uses printf function and FILE filetype
#include <stdlib.h> //uses exit function
#include <sys/wait.h> //uses SIGPIPE signal

void catcher(int sig);  //catcher function prototype

int newfd;  //socket file descriptor to connect to client

int main(int argc, char *argv[])
{
  int socketd;   //server's socket descriptor
  struct sockaddr_in srv;      //holds server socket address information
  struct sockaddr_in cli;      //holds client socket address information
  int cli_len = sizeof(cli);  //stores length of cli for use in accept function
  char buf[256]; // buffer for information exchanged thru sockets
  int nbytes; //number of bytes in the buffer
  FILE *inputfp;  //address of input file

  signal(SIGPIPE, catcher); //calls catcher function if socket disconnects

  // 1) Socket Creation
    if((socketd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket call failed");
      exit(1);
    }

  // 2) Socket binding to port
    srv.sin_family = AF_INET; //get IPv4 address family
    srv.sin_port = htons(51428); //bind socket to port 51428

    //let client bind to any addresses
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    //the actual binding of socket
    if(bind(socketd, (struct sockaddr *) &srv, sizeof(srv)) < 0) {
      perror("bind call failed");
      exit(1);
    }

  // 3) Listen for incoming connections
    if(listen(socketd, 5) < 0) {
      perror("listen call failed");
      exit(1);
    }

  // 4) accept incoming connection
    newfd = accept(socketd, (struct sockaddr*) &cli, &cli_len);
    if(newfd < 0) {
      perror("accept call failed");
      exit(1);
    }
    else
    printf("Server: Accept call successful.\n");

  // 5) read client data
    if((nbytes = read(newfd, buf, sizeof(buf))) < 0) {
      perror("read");
       exit(1);
     }
     else
     printf("Server: Got client's filepath.\tbytes read=%d\n\tbuffer=%s\n",nbytes,buf);

     // 6) opens input file using filepath sent from client
     inputfp=fopen(buf,"r");
     if( inputfp == NULL ){
      printf(" Server: Error IN Opening File .. \n");
      exit(1);
     }
     else
     printf("Server: Opened input file successfully\n");

     //7) copies info from input file into buf, and sends it to client in chunks
     while ( fgets(buf,sizeof(buf),inputfp) != NULL )
     {
      nbytes=strlen(buf);
      printf("Server: Read %d bytes from the input file\n\ttext=%s\n",nbytes,buf);
      nbytes = write(newfd, buf, sizeof(buf));
      printf("Server: wrote %d bytes into the socket\n",nbytes);
     }

      fclose (inputfp);       // close the input file

      //The commented code below was supposed to receive a disconnect message
      //from the client to close the socket

      // if((nbytes = recv(newfd, buf, sizeof(buf),0)) < 0) {
      //   perror("receive");
      //    exit(1);
      //  }
      //  else {
      //    printf("%s",buf);
      //    exit(0);
      //    printf("Server: Client has closed their file and exited.\tbytes read=%d\n\tbuffer=%s\n",nbytes,buf);
      //    close(newfd); //close the socket
      //  }

     close(newfd); //closes the socket to the client
  }

  /*
  ===============================================================================

     Name: catcher

     Written by: Shane Damien Widanagama

     Date written: 4/1/2020

     Purpose: Closes the socket to the client if it disconnects

     Usage:     i.e. void catcher(int sig);

     Description of parameters:
     sig accepts the SIGPIPE signal type from main

     Libraries Required
     <stdlib.h>, <sys/wait.h>, and <unistd.h>

  ===============================================================================
  */
  void catcher(int sig)
  {
    close(newfd);
    exit(0);
  }
