#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main (int argc, char* argv[]) {
	int loopcounter;
	int int_lengthVal = atoi(argv[1]);
	//string of key used through out file
	char char_keyValue[int_lengthVal+1];
	//applying time as a seed value to random generation
	srand(time(0));

	// while loop used for random value assigning to a key
	loopcounter=0;
	while ( loopcounter<int_lengthVal ) {
		int randm_value = rand() % 27;
		char_keyValue[loopcounter] = sourceArray[randm_value];
		loopcounter++;
	}
	// append end of character file
	char_keyValue[int_lengthVal] = '\0';
	printf("%s\n", char_keyValue);

	return 0;
}
