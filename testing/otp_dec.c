#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>
#include <stdbool.h>

//basic BasicArray
static const char BasicArray[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
// encryption for client
#define MAX_CHAR 90000
#define BASICLEN 28

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;
  char keygen[MAX_CHAR];
  char cipher_text[MAX_CHAR];
  int keygenlen;
  int cipher_textlen;
  int basicarraylen;
  // char EncryptedMessage[MAX_CHAR];

  if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[3]); exit(0); } // Check usage & args

  // Set up the server address struct
  //Counter program
  // printf("Argument counter normal file %s\n", argv[1]);
  // printf("Argument counter keyfile %s\n", argv[2]);
  // printf("Argument counter port number:  %s\n", argv[3]);



  //File checking for key and will transfer key to see how it works
  //lets check length for key and file for comparison and check all properties while reading
  FILE* file_pointer = fopen(argv[2], "r");
  // fgets(keygen, MAX_CHAR, file_pointer);
  char charactercounter;
  int count = 0;
  while((charactercounter = fgetc(file_pointer)) != EOF)
  {
    if(charactercounter > 32 && charactercounter < 65 && charactercounter < 90)
    {
      fprintf(stderr, "ERROR: Keygen file has invalid characters \n");
      exit(1);
    }

    keygen[count] = charactercounter;
    count++;
  }
  fclose(file_pointer);
  //need to get rid off new line and put a "\0"
  // this didn't works
  keygen[strcspn(keygen, "\n")] = '\0';
  // Keygen[count]='\0';
  keygenlen=strlen(keygen);
  // printf(" key:  %s\n", keygen);
  // File for the plain text
  FILE* file_pointer2 = fopen(argv[1], "r");
  // fgets(cipher_text, MAX_CHAR, file_pointer2);
  charactercounter='\0';
  count = 0;
  while((charactercounter = fgetc(file_pointer2)) != EOF)
  {
    if(charactercounter > 32 && charactercounter < 65 && charactercounter < 90)
    {
      fprintf(stderr, "ERROR: Plaintext, file has invalid characters \n");
      exit(1);
    }

    cipher_text[count] = charactercounter;
    count++;
  }
  // cipher_text[count]='\0';
  fclose(file_pointer2);
  cipher_text[strcspn(cipher_text, "\n")] = '\0';
  cipher_textlen=strlen(cipher_text);
  // printf(" Text:  %s\n", cipher_text);
  // Note that the key passed in must be at least as big as the plaintext.
  if (keygenlen < cipher_textlen){
    fprintf(stderr, "ERROR: Keygen is shorter than plain text\n");
    exit(1);
  }

  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
  if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

  // Set up the socket
  // Set up the socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (socketFD < 0) error("CLIENT: ERROR opening socket");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
  error("CLIENT: ERROR connecting");
  //sending keygen len
  // printf("Sending the length of keygen %d\n", keygenlen);
  int converted_number=htonl(keygenlen);
  charsWritten = send(socketFD, &converted_number, sizeof(converted_number), 0); // Write to the server
  if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
  if (charsWritten < sizeof(converted_number)) printf("CLIENT: WARNING: Not all data written to socket!\n");

  //sending ciphertext length
  // printf("sending cipher text len %d\n", cipher_textlen);
  converted_number=htonl(cipher_textlen);
  charsWritten = send(socketFD, &converted_number, sizeof(converted_number), 0); // Write to the server
  if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
  if (charsWritten < sizeof(converted_number)) printf("CLIENT: WARNING: Not all data written to socket!\n");

  // printf("Sending keygen length %d\n", strlen(keygen));
  charsWritten = send(socketFD, keygen, strlen(keygen), 0); // Write to the server
  if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
  if (charsWritten < strlen(keygen)) printf("CLIENT: WARNING: Not all data written to socket!\n");

  // printf("Sending Cipher text length %d\n", strlen(cipher_text));
  charsWritten = send(socketFD, cipher_text, strlen(cipher_text), 0); // Write to the server
  if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
  if (charsWritten < strlen(cipher_text)) printf("CLIENT: WARNING: Not all data written to socket!\n");

  int decryptlen=0;
  int received_int = 0;
  // reciving Ciphertext length
  charsRead = recv(socketFD, &received_int, sizeof(received_int), 0); // Read the client's message from the socket
  if (charsRead < 0) error("ERROR reading from socket");
  // printf("SERVER: I received this from the client: \"%d\"\n", ntohl(received_int));
  decryptlen=htonl(received_int);
  // printf("Recieved this length for decrypted message %d\n", decryptlen);
  char* DecryptedMessage=malloc(decryptlen);
  memset(DecryptedMessage, '\0', decryptlen);
  charsRead=0;
  int readBytes=0;
  int countbytes=0;
  int bytesRemain=decryptlen;
  while (readBytes != decryptlen){
    charsRead = recv(socketFD, DecryptedMessage+readBytes, decryptlen, 0);
    if (charsRead < 0) error("ERRROR READING KEYGEN");
      readBytes=readBytes+charsRead;
      bytesRemain=decryptlen-readBytes;
    // printf("Bytes reamining %d\n", bytesRemain);
  }
  printf("%s\n", DecryptedMessage);

  
  close(socketFD); // Close the socket
	return 0;
}
