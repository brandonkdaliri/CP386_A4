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

int resourceCount;
int *resourceAvailable;
int **max;

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
        printf("%d, ", resourceAvailable[i]);
    }
    */
    return 0;
}