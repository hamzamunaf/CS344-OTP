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
#define BASICLEN 27
// daemon server for encryption
// This child process of otp_enc_d must first check to make sure it is communicating with otp_enc (see otp_enc, below). After verifying that the connection to otp_enc_d is coming from otp_enc, then this child receives from otp_enc plaintext and a key via the communication socket (not the original listen socket). The otp_enc_d child will then write back the ciphertext to the otp_enc process that it is connected to via the same communication socket. Note that the key passed in must be at least as big as the plaintext.
//There are two ways we can recive the message from the client either I get the size of mesage and recieve the message or I can break it into two different things
// Not sure what to do :()
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten;
	socklen_t sizeOfClientInfo;
	int keygenlen=0;
	int plain_textlen=0;
	char buffer[10000];
	int readBytes=0;
	int countbytes=0;
	int bytesRemain=0;
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

	while(1){
	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");
	// printf("Server: Connected client at port %d\n", ntohs(clientAddress.sin_port));


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
	plain_textlen=htonl(received_int);
	// printf("encrypted text length is %d\n", plain_textlen);
	// Get the message from the client and display it

	// reciveing keygen
	//lets do the same for keygen as we did to recieve plain_text
	char* keygen=malloc(keygenlen);
	memset(keygen, '\0', keygenlen);
	charsRead=0;
	int readBytes=0;
	int countbytes=0;
	int bytesRemain=keygenlen;
	while (readBytes != keygenlen){
		charsRead = recv(establishedConnectionFD, keygen, bytesRemain, 0);
		if (charsRead < 0) error("ERRROR READING KEYGEN");
			readBytes=readBytes+charsRead;
			bytesRemain=keygenlen-readBytes;
		// printf("Bytes reamining %d\n", bytesRemain);
	}
	printf("Keygen : %s\n", keygen);
	// just for checking
	// FILE *fptr1 = fopen("KeygenPlaintext", "wb");
	// fprintf(fptr1, "%s\n", keygen);
	// fclose(fptr1);
	// recieving plaintext
	char* plain_text=malloc(plain_textlen);
	memset(plain_text, '\0', plain_textlen);
	charsRead=0;
 	readBytes=0; //how much data was read
 	countbytes=0; //how much data is left to read
 	bytesRemain=plain_textlen; //total length of the bytes that supposed to be read
	while (readBytes != plain_textlen){
		charsRead = recv(establishedConnectionFD, plain_text, bytesRemain, 0);
		if (charsRead < 0) error("ERROR reading from socket");
				readBytes=readBytes+charsRead;
			bytesRemain=plain_textlen-readBytes;

			// countbytes=strlen(plain_text);
			// printf("Bytes remaining %d\n", bytesRemain);
			// if (bytesRemain == 0){
			// 	break;
			// }
	} // Read the client's message from the socket
	printf("I got this plain message %s\n", plain_text);
	// printf("SERVER: I received this from the client: \"%d\"\n", strlen(plain_text));
	if (strlen(plain_text) != plain_textlen){
			error("Something is wrong, plaintext was not recieved properly");
	}
	char Encrypted_text[plain_textlen];
	memset(Encrypted_text, '\0', plain_textlen);
	//lets encrypted it
	int keyintArray[keygenlen]; //converting key to int
	int plaintIntArray[plain_textlen]; //converting plaintext to int
	int i=0;
	int j=0;
	for (i=0; i<keygenlen; i++){
		for (j=0; j<27; j++){
			if (keygen[i] == BasicArray[j])
			keyintArray[i] = j;
		}
	}
	for (i=0; i<plain_textlen; i++){
		for (j=0; j<27; j++){
			if (plain_text[i] == BasicArray[j])
				plaintIntArray[i] = j;
		}
	}
	int EncryptedTextArray[plain_textlen];
	for (i=0; i<plain_textlen; i++){
		int messagekey=0;
		messagekey=plaintIntArray[i]+keyintArray[i];
		EncryptedTextArray[i] = ((messagekey) % 27);
	}
	for (i=0; i<plain_textlen; i++){
		Encrypted_text[i]=BasicArray[EncryptedTextArray[i]];
	}
	//sending encrypted Text length
	int encryptedtextlen = strlen(Encrypted_text);
	int converted_number=htonl(encryptedtextlen);
	charsWritten = send(establishedConnectionFD, &converted_number, sizeof(converted_number), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < sizeof(converted_number)) printf("CLIENT: WARNING: Not all data written to socket!\n");


	charsWritten = send(establishedConnectionFD, Encrypted_text, encryptedtextlen, 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < sizeof(converted_number)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	//lets send the encrypted text
	// printf("length of encrypted Message %d\n", strlen(Encrypted_text));
	// FILE *fptr1 = fopen("EncryptedText", "wb");
	// fprintf(fptr1, "%s\n", Encrypted_text);
	// fclose(fptr1);

	//lets send it back to our client

	close(establishedConnectionFD); // Close the existing socket which is connected to the client
}
	close(listenSocketFD); // Close the listening socket
	return 0;
}
