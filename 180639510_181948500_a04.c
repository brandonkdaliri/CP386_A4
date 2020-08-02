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
#include <semaphore.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_INPUT_SIZE 128
#define FILENAME "sample4_in.txt"

int numResources, numCustomers;
int *available, **max, **allocation, **need;
int *sequence;
int safe;

int **readFile(char *filename);
void print2DArray(int **data, int width, int height);
void printArray(int *data, int n);
int parseArguments(char *s, int *arr);
int *getSequence();
void *threadRun(void *t);


int main (int argc, char *argv[]) {
  char *cmd = malloc(sizeof(char) * MAX_INPUT_SIZE);

  if (argc < 2) {
		printf("Parameters missing, exiting with error code -1\n");
		return -1;
  }

  // Create available array
  numResources = argc - 1;
  available = (int *)malloc(sizeof(int) * numResources);
  for (int i = 0; i < numResources; i++)
    available[i] = atoi(argv[i + 1]);


  // Read file and assign maximum demand of each process 
  max = readFile(FILENAME);
  if (max == NULL) return -1;

  // Create allocation and need arrays
  allocation = malloc(sizeof(int *) * numCustomers);
  need = malloc(sizeof(int *) * numCustomers);
  for (int i = 0; i < numCustomers; i++) {
    allocation[i] = malloc(sizeof(int) * numResources);
    need[i] = malloc(sizeof(int) * numResources);
  }

  // Display initial status
  printf("Number of Customers: %d\n", numCustomers);
  printf("Currently Available resources: ");
  printArray(available, numResources);
  printf("Maximum resources from file:\n");
  print2DArray(max, numCustomers, numResources);

  // Begin prompting user for input
  int *args = (int *)malloc(sizeof(int) * numResources + 2);
  int numArgs;
  while(1) {
    // get input from user
    printf("Enter Command: ");
    fgets(cmd, MAX_INPUT_SIZE, stdin);

    numArgs = parseArguments(cmd, args);

    // REQUEST RESOURCES ("RQ")
    if (args[0] == 0) {
      // Check that input is valid
      if (numArgs == numResources + 1 && args[1] < numCustomers && args[1] >= 0) {
        int customer = args[1];
        
        for (int i = 0; i < numResources; i++) {
          allocation[customer][i] = args[i + 2]; 
          need[customer][i] = max[customer][i] - allocation[customer][i];
          
          if (need[customer][i] < 0) need[customer][i] = 0;
        }
        // free input array
        free(args);
        
        // check safety algo and get safe sequence
        sequence = getSequence();
        if (sequence[0] == -1) {
          safe = 0;
          printf("Request is satisfied but unsafe (fix before running)\n");
        } else {
          safe = 1;
          printf("Request is satisfied and safe\n");
        }
      } else {
        printf("Error: RQ [customer] [resource 1] [resource 2] ... [resource n]\n");
      }
    }
    // RELEASE RESOURCES ("RL") 
    else if (args[0] == 1) {
      if (numArgs == numResources + 1 && args[1] < numCustomers && args[1] >= 0) {
        int customer = args[1];
        
        for (int i = 0; i < numResources; i++) {
          if (args[i + 2] <= allocation[customer][i]) {
            allocation[customer][i] = allocation[customer][i] - args[i + 2];
            need[customer][i] = max[customer][i] - allocation[customer][i];
          } else {
            printf("Error: Cannot release more resources than allocated\n");
            break;
          }
        }

        // free input array
        free(args);
        
        // check safety algo and get safe sequence
        sequence = getSequence();

        if (sequence[0] == -1) {
          safe = 0;
          printf("Request is satisfied but unsafe (fix before running)\n");
        } else {
          safe = 1;
          printf("Request is satisfied and safe\n");
        }
      } else {
        printf("Error: RL [customer] [resource 1] [resource 2] ... [resource n]\n");
      }
    }
    // PRINT DATA ("*")
    else if (args[0] == 2) {
      printf("Available Resources:\n");
      printArray(available, numResources);
      printf("Maxmium Resources:\n");
      print2DArray(max, numCustomers, numResources);
      printf("Allocated Resources:\n");
      print2DArray(allocation, numCustomers, numResources);
      printf("Need Resources:\n");
      print2DArray(need, numCustomers, numResources);
    } 
    // RUN THREADS ("Run")
    else if (args[0] == 3) {
      sequence = getSequence();
      safe = sequence[0] == -1 ? 0 : 1;

      printf("Safe Sequence is: ");
      printArray(sequence, numCustomers);

      if (safe == 1) {
        for (int i = 0; i < numCustomers; i++) {
          int thread = sequence[i];
          pthread_t tid;
          pthread_attr_t attr;
          pthread_attr_init(&attr);
          pthread_create(&tid, &attr, threadRun, (void *)&thread);
          pthread_join(tid, NULL);
        }
      } else {
        printf("Error: Unsafe state must be fixed before running\n");
      }
    }
    // EXIT PROGRAM
    else if (args[0] == -2) {
      break;
    } else {
      printf("Invalid input, use one of RQ, RL, *, Run, Exit\n");
    }
  }
}

void *threadRun(void *t){
  int *tid = (int *)t;
  printf("--> Customer/Thread %d\n", *tid);

  printf("    Allocated resources:  ");
  printArray(allocation[*tid], numResources);
  
  printf("    Needed: ");
  printArray(need[*tid], numResources);

  printf("    Available:  ");
  printArray(available, numResources);

  printf("    Thread has started\n");
  sleep(1);
  printf("    Thread has finished\n");
  sleep(1);
  printf("    Thread is releasing resources\n");
  sleep(1);
  printf("    New Available:  ");

  for (int i = 0; i < numResources; i++){
    available[i] = available[i] + allocation[*tid][i];
    printf("%d ", available[i]);
  }
  
  printf("\n");
  pthread_exit(NULL);
}


int *getSequence() {
  int *seq = malloc(sizeof(int) * numCustomers);
  int *work = malloc(sizeof(int) * numResources);
  int *finished = malloc(sizeof(int) * numCustomers);
  int i, j;
  // init work = available
  for (i = 0; i < numResources; i++) {
    work[i] = available[i];
  }

  // init finished = 0 (false)
  for (i = 0; i < numCustomers; i++) {
    finished[i] = 0;
  }

  int count = 0;
  while (count < numCustomers) {
    int flag = 0;
    for (i = 0; i < numCustomers; i++) {
      if (finished[i] == 0) {
        int temp = 1;
        j = 0;
        while (j < numResources && temp == 1) {
          if (need[i][j] > work[j]) {
            temp = 0;
          }
          j++;
        }

        // append customer to sequence
        if (temp == 1) {
          seq[count] = i;
          finished[i] = 1;
          flag = 1;
          count++;
          for (int j = 0; j < numResources; j++) {
            work[j] = work[j] + allocation[i][j];
          }
        }
      }
    }
    // If no safe sequence found, set all elements to -1
    if (flag == 0) {
        for (int k = 0; k < numCustomers; k++) {
          seq[k] = -1;
        }
        free(work);
        free(finished);
        return seq;
    }
  }

  free(work);
  free(finished);
  return seq;
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