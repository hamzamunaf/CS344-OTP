#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>


// encryption for client
#define MAX_CHAR 80000
// otp_enc plaintext1 mykey 57171 > ciphertext1
//otp_enc plaintext key port, need three arguments to make it work
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char keygen[MAX_CHAR];
  char enc_text[MAX_CHAR];
	int keygenlen;
	int enc_textlen;

	if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
  //Counter program
	  printf("Argument counter normal file %s\n", argv[1]);
		printf("Argument counter keyfile %s\n", argv[2]);
  	printf("Argument counter port number:  %s\n", argv[3]);



//File checking for key and will transfer key to see how it works
//lets check length for key and file for comparison
  FILE* file_pointer = fopen(argv[2], "r");
  fgets(keygen, MAX_CHAR, file_pointer);
  fclose(file_pointer);
	keygenlen=strlen(keygen);
// File for the plain text
  FILE* file_pointer2 = fopen(argv[1], "r");
  fgets(enc_text, MAX_CHAR, file_pointer2);
  fclose(file_pointer2);
	enc_textlen=strlen(enc_text);

	if (keygenlen < enc_textlen){
		printf("Error, key is shorter than the text to encrypt\n");
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
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Send message to server
	charsWritten = send(socketFD, keygen, strlen(keygen), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(keygen)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	charsWritten = send(socketFD, enc_text, strlen(enc_text), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(enc_text)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	// Get return message from server
	// memset(buffer2, '\0', sizeof(buffer2)); // Clear out the buffer again for reuse
	// charsRead = recv(socketFD, buffer2, sizeof(buffer2) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	// printf("CLIENT: I received this from the server: \"%s\"\n", buffer2);

	close(socketFD); // Close the socket
	return 0;
}
