#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CHAR 80000

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

}
