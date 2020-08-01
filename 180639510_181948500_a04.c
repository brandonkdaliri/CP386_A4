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

#define MAX_LINE_LEN 128

int resourceCount;
int *resourceAvailable;
int **max;
char *FILE_NAME = "sample4_in.txt";
int numCustomers;
int safe;
int **customerAllocate;
int **customerNeed;
int **fileRead(char *file);
void printCustomerData(int **data, int m, int n);
void printResourceData(int *data, int m);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Bad input! \n");
        return -1;
    }
    resourceCount = argc - 1;
    resourceAvailable = malloc(sizeof(int) * resourceCount);
    for (int i = 0; i < argc; i++)
    {
        resourceAvailable[i - 1] = atoi(argv[i]);
    }
    /*
    for (int i = 0; i < resourceCount; i++)
    {
        printf("%d,", resourceAvailable[i]);
    }
    */
    setbuf(stdout, NULL);
    max = fileRead(FILE_NAME);

    customerAllocate = malloc(sizeof(int *) * numCustomers);
    customerNeed = malloc(sizeof(int *) * numCustomers);

    for (int i = 0; i < numCustomers; i++)
    {
        customerAllocate[i] = malloc(sizeof(int) * resourceCount);
        customerNeed[i] = malloc(sizeof(int) * resourceCount);
    }
    safe = 0;

    printf("Number of Customers: %d\n", numCustomers);
    printf("Currently Available resources: ");
    printResourceData(resourceAvailable, resourceCount);
    printf("Maximum resources from file:\n");
    printCustomerData(max, numCustomers, resourceCount);
    char *userInput = malloc(sizeof(char) * MAX_LINE_LEN);
    while (1)
    {
        printf("Enter Command: ");
        fgets(userInput, MAX_LINE_LEN, stdin);
        if (strlen(userInput) > 0 && userInput[strlen(userInput) - 1] == '\n')
        {
            userInput[strlen(userInput) - 1] = '\0';
        }
        printf("%s\n", userInput);
        if (strstr(userInput, "0"))
        {
            break;
        }
        else if (strstr(userInput, "RQ"))
        {
            //Split input by spaces
            int n = 0;
            int *arr = malloc(sizeof(int) * (resourceCount + 1));
            char *chp = NULL;
            chp = strtok(userInput, " ");
            while (chp != NULL)
            {
                if (n > 0)
                {
                    arr[n - 1] = atoi(chp);
                }
                chp = strtok(NULL, " ");
                n++;
            }

            int customer = arr[0];
            // Insert into allocation array
            if (customer < numCustomers && n == resourceCount + 2)
            {
                for (int i = 0; i < resourceCount; i++)
                {
                    customerAllocate[customer][i] = arr[i + 1];
                    customerNeed[customer][i] = max[customer][i] - customerAllocate[customer][i];
                    // Don't let need values become negative
                    if (customerNeed[customer][i] < 0)
                    {
                        customerNeed[customer][i] = 0;
                    }
                }
            }
            else
            {
                if (customer >= numCustomers)
                {
                    printf("Thread out of bounds, please try again.\n");
                }
                else
                {
                    printf("Incorrect parameter count, please try again.\n");
                }
            }
            free(arr);
            // Determine if request would be satisfied or denied with safety algorithm
        }
        //Input cases, RQ, RL, *, Run, Exit
    }
    return 0;
}

int **fileRead(char *file)
{
    FILE *input = fopen(file, "r");
    if (!input)
    {
        printf("Error opening file.\n");
        return NULL;
    }
    struct stat st;
    fstat(fileno(input), &st);
    char *content = (char *)malloc(((int)st.st_size + 1) * sizeof(char));
    content[0] = '\0';
    while (!feof(input))
    {
        char line[MAX_LINE_LEN];
        if (fgets(line, MAX_LINE_LEN, input) != NULL)
        {
            strncat(content, line, strlen(line));
        }
    }
    fclose(input);
    char *command = NULL;
    char *copy = (char *)malloc((strlen(content) + 1) * sizeof(char));
    strcpy(copy, content);
    //Using strtok to filter newlines in array
    command = strtok(copy, "\r\n");
    while (command != NULL)
    {
        command = strtok(NULL, "\r\n");
        numCustomers++;
    }
    char *lines[numCustomers];
    strcpy(copy, content);
    command = strtok(copy, "\r\n");
    int c;
    while (command != NULL)
    {
        lines[c] = (char *)malloc(sizeof(command) * sizeof(char));
        strcpy(lines[c], command);
        c++;
        command = strtok(NULL, "\r\n");
    }
    int **maximum = malloc(sizeof(int *) * numCustomers);
    for (int i = 0; i < numCustomers; i++)
    {
        int *temp = malloc(sizeof(int) * resourceCount);
        int j = 0;
        char *tok = NULL;
        tok = strtok(lines[i], ",");
        while (tok != NULL)
        {
            temp[j] = atoi(tok);
            j++;
            tok = strtok(NULL, ",");
        }
        maximum[i] = temp;
    }
    return maximum;
}
void printCustomerData(int **data, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d", data[i][j]);
            if (j < n - 1)
                printf(" ");
        }
        printf("\n");
    }
}

void printResourceData(int *data, int m)
{
    for (int i = 0; i < m; i++)
    {
        printf("%d", data[i]);
        if (i < m - 1)
            printf(" ");
    }
    printf("\n");
}