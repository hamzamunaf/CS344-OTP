#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CHAR 80000

static const char BasicArray[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
int main(int argc, char *argv[]){
  // lets work on otp here and see how it reacts
  char keygen[MAX_CHAR];
  char enc_text[MAX_CHAR];
  int keygenlen;
  int enc_textlen;

  FILE* file_pointer = fopen(argv[2], "r");
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
  keygen[strcspn(keygen, "\n")] = '\0';
  keygenlen=strlen(keygen);
  FILE* file_pointer2 = fopen(argv[1], "r");
  charactercounter='\0';
  count = 0;
  while((charactercounter = fgetc(file_pointer2)) != EOF)
  {
    if(charactercounter > 32 && charactercounter < 65 && charactercounter < 90)
    {
      fprintf(stderr, "ERROR: Plaintext, file has invalid characters \n");
      exit(1);
    }

    enc_text[count] = charactercounter;
    count++;
  }
  fclose(file_pointer2);
  enc_text[strcspn(enc_text, "\n")] = '\0';
  enc_textlen=strlen(enc_text);

  printf("Key value: %s\n", keygen);
  printf("Plain text value: %s\n", enc_text);

  // Lets encrypt it and see how that works out
  // first we will contact all characters in keygen and plaintext to integer
  //then we are going to add the keygen value from the Plaintext
  // If a number is larger than 26, then the remainder, after subtraction of 26, is taken [as the result]. This simply means that if the computations "go past" Z, the sequence starts again at A.
  //then we will mod 26
  //we have the length of both strings
  //lets setup an integer array for that
  int keyintArray[keygenlen];
  int encIntArray[enc_textlen];
  int i=0;
  int j=0;
  for (i=0; i< keygenlen; i++){
    for(j=0; j<27; j++){
      if (keygen[i] == BasicArray[j]){
    keyintArray[i] = j;
  }}
  }
  // for (i=0; i<enc_textlen; i++){
  //   encIntArray[i] = enc_text[i];
  // }
  for (i=0; i< enc_textlen; i++){
    for(j=0; j<27; j++){
      if (enc_text[i] == BasicArray[j]){
        encIntArray[i]= j;
  }}
  }
  // converted
  printf("------------Encrypted Numbers ------------\n");
  int EncryptIntarray[enc_textlen];
  for (i=0; i<enc_textlen; i++){
    int counter=0;
    counter=encIntArray[i]+keyintArray[i];
    EncryptIntarray[i]=((counter) % 27);
    printf("Here is the number %d\n", EncryptIntarray[i]);
  }
  char EncryptedMessage[enc_textlen];
  for (i=0; i<enc_textlen; i++){
    EncryptedMessage[i]=BasicArray[EncryptIntarray[i]];
    // sprintf(EncryptedMessage[i], "%d", EncryptIntarray[i]);
    // printf("Here is each character that is encrypted %s\n", EncryptedMessage[i]);
  }

  printf("Here is the encrypted message %s\n", EncryptedMessage);
// --------------------- Decryption -----------------
// gotta a weird encrypted text but not sure if its right until I decrypt it
  int decryptlen=strlen(EncryptedMessage);
  int decryptIntArray[decryptlen];
  // for (i=0; i< decryptlen; i++){
  //   decryptIntArray[i] = EncryptedMessage[i];
  // }

  for (i=0; i< decryptlen; i++){
    for(j=0; j<27; j++){
      if (EncryptedMessage[i] == BasicArray[j]){
    decryptIntArray[i] = j;
  }}
  }
  int DecryptIntArray2[decryptlen];
printf("------------Decrypted Numbers ------------\n");
  for (i=0; i<decryptlen; i++){
    int counter=0;
    counter=decryptIntArray[i]-keyintArray[i];
    if (counter < 0){
      counter+=27;
    }
      DecryptIntArray2[i]=counter;
      printf("Here is the number %d\n", DecryptIntArray2[i]);
  }
  char DecryptedMessage[decryptlen];
  for (i=0; i<decryptlen; i++){
    DecryptedMessage[i] = BasicArray[DecryptIntArray2[i]];
  }
  DecryptedMessage[decryptlen] = '\0';
  printf("Here is the Decrypted message %s\n", DecryptedMessage);
  //for decryption
  // we will do the saame thing as encryption but we are going to subtract the keygen value from the plaintext
  // Similar to the above, if a number is negative then 26 is added to make the number zero or higher.
}
