#include <stdio.h>
#include <stdlib.h>


char *fileToString(const char *path){
	FILE *fp;

	fp = fopen(path,"r"); //read mode
	if(fp == NULL){ 
		perror("Error while opening file.\n");
		exit(EXIT_FAILURE);
	}
	
	char ch;
	int i = 0, size;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET); // seek back to file start
	char *str = malloc(size);

	while((ch = fgetc(fp)) != EOF){
		str[i] = ch;	
	}
	return str;
}

//int main(void){
//	char *str = fileToString("test.txt");
//	printf("\n %s", str);
//	free(str);
//	return EXIT_SUCCESS;
//}
