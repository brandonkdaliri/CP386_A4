/**
 * CP386: Operating Systems
 * Assignment 4
 * 
 * Name: Brandon Daliri
 * GitHub: brandonkdaliri
 * Student ID: 180639510
 * 
 * Name: Joseph Power
 * GitHub: josephpow
 * Student ID: 181948500
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>

int numResources, numCustomers;
int *available, **max, **allocation, **need;
int *sequence;
int safe;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Bad input! \n");
        return -1;
    }

    return 0;
}

/**
 * Parses user input.
 * Returns an array of integers where the
 * first value tells us what type of input the user
 * has requested (RQ, RL, *), and the rest of the array
 * is its parameters.
 * 
 * Exit = -2
 * Error = -1
 * RQ = 0
 * RL = 1
 * (*) = 2
 * Run = 3
**/
int parseArguments(char *s, int *arr) {
  int len = strlen(s);
  if (len == 0){
    arr[0] = -1;
    return -1;
  }
  
  // clean the string
  if (s[len - 1] == '\n')
    s[len - 1] = '\0';

  char *tok = strtok(s, " ");
  if (strcmp(tok, "RQ") == 0) {
    arr[0] = 0;
  } else if (strcmp(tok, "RL") == 0) {
    arr[0] = 1;
  } else if (strcmp(tok, "*") == 0) {
    arr[0] = 2;
  } else if (strcmp(tok, "Run") == 0){
    arr[0] = 3;
  } else if (strcmp(tok, "Exit") == 0){
    arr[0] = -2;
  } else {
    arr[0] = -1;
    return -1;
  }
  
  int i = 0;
  tok = strtok(NULL, " ");
  while(tok != NULL && i < numResources + 1){
    arr[i + 1] = atoi(tok);
    // printf("adding %d to index %d\n", atoi(tok), i + 1);
    i++;
    tok = strtok(NULL, " ");
  }

  return i;
}

int **readFile(char *filename) {
  FILE *file = fopen(filename, "r");
	if(!file) {
		printf("Error in opening input file...exiting with error code -1\n");
		return NULL;
	}

  // Read in file
  struct stat st;
	fstat(fileno(file), &st);
	char* fileContent = (char*)malloc(((int)st.st_size+1)* sizeof(char));
	fileContent[0]='\0';	
	while(!feof(file)) {
		char line[100];
		if(fgets(line,100,file)!=NULL) {
			strncat(fileContent,line,strlen(line));
		}
	}
	fclose(file);

  // Create copy of file to tokenize
  char *cmd = NULL;
  char *fileCopy = (char *)malloc( sizeof(char) * (strlen(fileContent) + 1) );
  
  strcpy(fileCopy, fileContent);
  cmd = strtok(fileCopy, "\r\n");
  // get number of customers
  while (cmd != NULL) {
    numCustomers++;
    cmd = strtok(NULL, "\r\n");
  }

  strcpy(fileCopy, fileContent);

  char *resources[numCustomers];
  cmd = strtok(fileCopy, "\r\n");
  

  int i = 0;
  while(cmd != NULL) {
    resources[i] = malloc( sizeof(cmd) * sizeof(char) );
    strcpy(resources[i], cmd);
    i++;
    cmd = strtok(NULL, "\r\n");
  }

  // Parse every individual number and create 2D max array
  int **temp_max = malloc( sizeof(int *) * numCustomers );
  for (i = 0; i < numCustomers; i++) {
    int *row = (int *)malloc( sizeof(int) * numResources );
    char *tok = strtok(resources[i], ",");
    int j = 0;

    while (tok != NULL) {
      row[j] = atoi(tok);
      // printf("adding %d to row[%d]\n", row[j], j);
      j++;
      tok = strtok(NULL, ",");
    }
    temp_max[i] = row;
  }

  return temp_max;
}

void print2DArray(int **data, int height, int width) {
  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
      printf("%d ", data[i][j]);
    }
    printf("\n");
  }
}

void printArray(int *data, int n) {
  for(int i = 0; i < n; i++){
    printf("%d ", data[i]);
  }
  printf("\n");
}