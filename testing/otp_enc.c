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
// otp_enc plaintext1 mykey 57171 > ciphertext1
//otp_enc plaintext key port, need three arguments to make it work


void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char keygen[MAX_CHAR];
  char plain_text[MAX_CHAR];
	int keygenlen;
	int plain_textlen;
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
  // fgets(plain_text, MAX_CHAR, file_pointer2);
	charactercounter='\0';
	count = 0;
	while((charactercounter = fgetc(file_pointer2)) != EOF)
	{
		if(charactercounter > 32 && charactercounter < 65 && charactercounter < 90)
		{
			fprintf(stderr, "ERROR: Plaintext, file has invalid characters \n");
			exit(1);
		}

		plain_text[count] = charactercounter;
		count++;
	}
	// plain_text[count]='\0';
  fclose(file_pointer2);
	plain_text[strcspn(plain_text, "\n")] = '\0';
	plain_textlen=strlen(plain_text);
	// printf(" Text:  %s\n", plain_text);
// Note that the key passed in must be at least as big as the plaintext.
	if (keygenlen < plain_textlen){
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
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");



//Sending the keygen length
	int converted_number=htonl(keygenlen);
	charsWritten = send(socketFD, &converted_number, sizeof(converted_number), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < sizeof(converted_number)) printf("CLIENT: WARNING: Not all data written to socket!\n");


//sending the plaintext length
	converted_number=htonl(plain_textlen);
	charsWritten = send(socketFD, &converted_number, sizeof(converted_number), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < sizeof(converted_number)) printf("CLIENT: WARNING: Not all data written to socket!\n");

//sending keygen
// printf("Sending this much keygen lenght %d\n", strlen(keygen));
	charsWritten = send(socketFD, keygen, strlen(keygen), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(keygen)) printf("CLIENT: WARNING: Not all data written to socket!\n");


//sending plain text
// printf("Sending this much character %d\n", strlen(plain_text));
	charsWritten = send(socketFD, plain_text, strlen(plain_text), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(plain_text)) printf("CLIENT: WARNING: Not all data written to socket!\n");


// receiving encrypted text length
	int encryptedMessageLen=0;
	int received_int=0;
	charsRead = recv(socketFD, &received_int, sizeof(received_int), 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	// printf("SERVER: I received this from the client: \"%d\"\n", ntohl(received_int));
	encryptedMessageLen=htonl(received_int);
	//
	// // Get return message from server
	// //recieving encrypted text
	char* encryptedMessage=malloc(encryptedMessageLen);
	memset(encryptedMessage, '\0', encryptedMessageLen);
	charsRead=0;
	int readBytes=0;
	int countBytes=0;
	int bytesRemain = encryptedMessageLen;
	while (readBytes != encryptedMessageLen){
			charsRead = recv(socketFD, encryptedMessage+readBytes, encryptedMessageLen, 0);
			if (charsRead < 0) error("ERROR recieving the encrypted Message\n");
			readBytes=readBytes+charsRead;
			bytesRemain=encryptedMessageLen-readBytes;
			// printf("Bytes remaining %d\n", bytesRemain);
	}
	//lets see what we recieved;
	if (strlen(encryptedMessage) != encryptedMessageLen){
		error("error encrypted message not recieved perfectly\n");
	}
	// printf("length of encrypted Message %d\n", strlen(Encrypted_text));
	// FILE *fptr1 = fopen("EncryptedText", "wb");
	// fprintf(fptr1, "%s\n", encryptedMessage);
	// fclose(fptr1);
	printf("%s\n", encryptedMessage);

	close(socketFD); // Close the socket
	return 0;
}
