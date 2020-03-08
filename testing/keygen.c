#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char sourceArray[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

int main (int argc, char* argv[]) {
	int loopcounter;
	int lengthCounter = atoi(argv[1]);
	//string of key used through out file
	char char_keyValue[lengthCounter+1];
	//applying time as a seed value to random generation
	srand(time(0));

	// while loop used for random value assigning to a key
	loopcounter=0;
	while ( loopcounter<lengthCounter ) {
		int randm_value = rand() % 27;
		char_keyValue[loopcounter] = sourceArray[randm_value];
		loopcounter++;
	}
	// append end of character file
	char_keyValue[lengthCounter] = '\0';
	printf("%s\n", char_keyValue);

	return 0;
}
