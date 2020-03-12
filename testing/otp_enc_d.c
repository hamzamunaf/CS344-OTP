#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

static const char BasicArray[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
// encryption for client
#define MAX_CHAR 80000
#define BASICLEN 28
// daemon server for encryption
// This child process of otp_enc_d must first check to make sure it is communicating with otp_enc (see otp_enc, below). After verifying that the connection to otp_enc_d is coming from otp_enc, then this child receives from otp_enc plaintext and a key via the communication socket (not the original listen socket). The otp_enc_d child will then write back the ciphertext to the otp_enc process that it is connected to via the same communication socket. Note that the key passed in must be at least as big as the plaintext.
//There are two ways we can recive the message from the client either I get the size of mesage and recieve the message or I can break it into two different things
// Not sure what to do :()
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	// char enc_text[MAX_CHAR];
	int keygenlen=0;
	int enc_textlen=0;
	char buffer[10000];
	// char keygen[MAX_CHAR];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
	error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections


	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");



	int received_int = 0;

	// reciving keygen length
	charsRead = recv(establishedConnectionFD, &received_int, sizeof(received_int), 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	// printf("SERVER: I received this from the client: \"%d\"\n", ntohl(received_int));
	keygenlen=htonl(received_int);
	// printf("Keygen length is %d\n", keygenlen);


	// recieving plaintext length
	received_int=0;
	charsRead = recv(establishedConnectionFD, &received_int, sizeof(received_int), 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	// printf("SERVER: I received this from the client: \"%d\"\n", ntohl(received_int));
	enc_textlen=htonl(received_int);
	printf("encrypted text length is %d\n", enc_textlen);
	// Get the message from the client and display it

	// reciveing keygen
	char keygen[keygenlen];
	memset(keygen, '\0', keygenlen);
	charsRead=0;
	while (charsRead == 0){
		charsRead = recv(establishedConnectionFD, keygen, keygenlen, 0);

	} // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	// printf("SERVER: I received this from the client: \"%s\"\n", keygen);


	// recieving plaintext
	char enc_text[enc_textlen];
	memset(enc_text, '\0', enc_textlen);
	charsRead=0;
	int readBytes=0; //how much data was read
	int countbytes=0; //how much data is left to read
	int bytesRemain=enc_textlen; //total length of the bytes that supposed to be read
	while (readBytes != enc_textlen){
		charsRead = recv(establishedConnectionFD, enc_text+readBytes, bytesRemain, 0);
		if (charsRead < 0) error("ERROR reading from socket");
			bytesRemain=enc_textlen-readBytes;
			readBytes=readBytes+charsRead;
			// countbytes=strlen(enc_text);
			printf("Bytes remaining %d\n", bytesRemain);
			if (bytesRemain == 0){
				break;
			}
	} // Read the client's message from the socket
	// printf("I got this much message %s\n", strlen(enc_text));
	printf("SERVER: I received this from the client: \"%d\"\n", strlen(enc_text));
	// printf("%s\n", enc_text);
	if (strlen(enc_text) != enc_textlen){
		printf("ERRORRRRR\n");
	}

	// sending encrypted text lenght
	// int converted_number=htonl(enc_textlen);
	// charsRead = send(establishedConnectionFD, &converted_number, sizeof(converted_number), 0); // Write to the server
	// if (charsRead < 0) error("CLIENT: ERROR writing to socket");
	// if (charsRead < sizeof(converted_number)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	// // sendering message back to client
	//
	// // sending encrypted message
	// charsRead = send(establishedConnectionFD, enc_text, strlen(enc_text), 0); // Write to the server
	// if (charsRead < 0) error("CLIENT: ERROR writing to socket");
	// if (charsRead < strlen(enc_text)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	//
	// printf("Server: I recieved this much length %d\n", strlen(enc_text));
	// Send a Success message back to the client
	// charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0;
}
