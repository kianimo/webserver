#include <stdlib.h>
#include <stdio.h>

/**
* Returns the content of a given file as *char.
* If file could not be read return NULL.
* 
* @param path - the path to the file to read.
* @return *char - needs to be freed whit free(void *ptr);
*/
char *fileToString(const char *path){
	FILE *fp;

	fp = fopen(path,"r"); //read mode
	if(fp == NULL){ 
		fprintf(stderr, "Error while opening %s\n", path);
		return NULL;
	}else{
		char ch;
		int i = 0, size;

		fseek(fp, 0L, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0L, SEEK_SET); // seek back to file start

		char *str = malloc(size+1);
		while((ch = fgetc(fp)) != EOF){
			str[i] = ch;	
			++i;
		}
		str[i] = 0;
		fclose(fp);
		return str;
	}
}

//int main(void){
//	char *str = fileToString("test.txt");
//	printf("%s", str);
//	free(str);
//	return EXIT_SUCCESS;
//}
