/*
===============================================================================
    Sockets Client Process

   Name: client.c

   Written by: Shane Damien Widanagama

   Date written: 4/2/2020

   Purpose: Makes use of sockets to send a filepath to a server, receive a copy
   of the file from the file from the server, and store it in an output file.

   Usage: ./client

   Description of parameters:
   None except for ./client

   Libraries Required
     See the include statements.

   Modification History:
     4/2/2020 - documentation and testing
     4/1/2020 - input file reception and storage
     3/31/2020 - socket creation, and connection

   References:
      Completed with the help of class slides on sockets and Jacques Beland

===============================================================================
*/

//structures and protocols for socket creation and connection
#include <netinet/in.h>
#include <string.h> //function to get length of strings
#include <stdio.h>  //uses printf function and FILE filetype
#include <stdlib.h> //uses exit function

int main(int argc, char *argv[])
{
  int socketd;  //client's socket descriptor
  struct sockaddr_in srv; //holds server socket address information
  char buf[256] = "/home/COIS/3380/lab2_document.txt";
  FILE *outputfp; //address of input file
  int outputfstatus;  //stores whether output file closed successfully


  // 1) Socket Creation
    if((socketd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket call failed");
      exit(1);
    }

  // 2) connection to server

    //get IPv4 address family
    srv.sin_family = AF_INET;

    // connect socket to port 51428
    srv.sin_port = htons(51428);

    //connect to loki's IP Address
    srv.sin_addr.s_addr = inet_addr("192.197.151.116");

    //connect call to server
    if(connect(socketd, (struct sockaddr*) &srv, sizeof(srv)) < 0) {
      perror("Client: connect call failed");
      exit(1);
    }

    //The code below was supposed to ask the user for a filepath to
    //send to the server, but the server's fopen() couldn't open the file,
    //even if the filepath inputted was identical to what is stored in buf.

    // printf("Input a lower case letter (or 0 to stop) => ");
    //   fgets(buf, 256, stdin);
    //   printf("User inputted: %s\n",buf );

    //sends file path to server
    int wbytes, rbytes;
    wbytes = write(socketd, buf, sizeof(buf));
    printf("Client: Wrote %d bytes to the server\n\tbuffer=%s\n",wbytes,buf);

    //opens to file to copy buf's contents into
    outputfp = fopen("/home/shanewidanagama/lab6/lab_sourcefile_local_clone","w");
    if( outputfp == NULL ){
      printf("Client: Error IN Opening File ");
      exit(1);
     }

     //reads file chunks, and stores them into the output file
     while( (rbytes= read(socketd,buf,sizeof(buf)) ) > 0 ) {
       printf("Client: Received %d bytes from the server\n\tbuf=%s\n",rbytes,buf);
       fputs(buf, outputfp);
     }

      printf("Client: File copied successfully !! \n");

      //closes acccess to the outputfile. If successful, a message is sent to
      //the server about the client's disconnection
      outputfstatus = fclose(outputfp); //closes access to output file
      if( outputfstatus == 0){
        strcpy(buf,"Client: I am disconnecting from your socket");
        wbytes = send(socketd, buf, sizeof(buf),0);
        printf("Wrote %d bytes to the server\n\tbuf=%s\n",wbytes,buf);
        close(socketd); //closes socket
        exit(0);
       }
      else {
        printf("Client: Error closing output file");
        exit(1);
      }

     exit(0); //terminates client process
   }
