#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

// maximum buffer size
#define MAXBUF 255

// maximum number of threads
#define MAXTHREADS 4000

#define MAX_LENGTH 80

#define SHM_SIZE 4096
#define READ 0
#define WRITE 1

pid_t pid;

void sigHandler(int sigNum);

//function used to check if input word is nonalphabetical
int allalpha(const char *str)
{
   size_t i;
   for(i=0;str[i];i++)
     if(!isalpha(str[i]))
        return 0;
   return 1;
}

//main function
int main(int argc, char* argv[]) {

    signal(SIGINT, sigHandler);

    char search_text[MAX_LENGTH];
    char yn = 'n';
    char files[10][256];
    int count = 1;
    pid_t pid, pid1, who, who2;
    int pi[2];
    int ip[2];
    int pvc[2];
    int k;
    char temp;
    char buf[1026], *eof = "#EOF";

     //while loop that keeps the searchText function going
     while(1) {
        //checks if pvc pipe is created
        if (pipe(pvc) < 0) {
            perror ("pvc pipe error\n");
            exit(1);
        }

        printf("pvc pipe created.\n");
        //checks is pi pipe is created
        if (pipe(pi) < 0) {
            perror ("pi pipe error\n");
            exit(1);
        }

        printf("pi pipe created.\n");
        //checks is ip pipe is created
        if (pipe(ip) < 0) {
            perror ("ip pipe error\n");
            exit(1);
        }

        printf("ip pipe created.\n");
        
        printf("Number of arguments passed: %d\n", argc);

        printf("Enter the text to be searched: \n");
        scanf("%s", search_text);

        //checks if search_text is non-alphabetical and kills all processes if it is
        if(!allalpha(search_text)) {
            kill(0,SIGTERM);
            exit(0);
        }

        printf("The parent PID is: %d\n\n", getpid());

        //writes the search_text inside the child
        write(pi[1], &search_text, sizeof(search_text));

        //for loop creates the child processes
        for(k = 1; k < argc; k++) {
            //printf("argc value: %d\n", argc);
            pid = fork();
            close(pi[1]);
            close(ip[1]);
            if(pid < 0) {
                printf("fork creation error\n");
                return 1;
            }
            else if(pid == 0) {

                char* currentFile = argv[k];
                int numOfTimes = 0;
                FILE *fp;
                char child_search[MAX_LENGTH];
                read(pi[0], &child_search, sizeof(child_search));
                //close(pi[0]);
                printf("child_search var %s, read...\n");
                int c;
                ssize_t read;
                size_t len = 0;
                fp = fopen(currentFile, "r");
                printf("file %s opened\n\n", currentFile);
                printf("child process %d is running file scan...\n", getpid());
                
                if (fp == NULL) {
                    exit(EXIT_FAILURE);
                }
                const size_t line_size = 5096;
                int tempcount = 0;
                int j, m;
                char* line = malloc(line_size);
                while(fscanf(fp, "%s", line) != EOF)  {
                    //printf("Text file: \n\n");
                   // printf(line);
                    if(strcmp(line, child_search) == 0) {
                        numOfTimes++;

                    }

                }
                printf("Num of times: %d\n", numOfTimes);

                free(line);
   
                fclose(fp);
                //read(pvc[0], numOfTimes, 3);
                write(pvc[1], &numOfTimes, sizeof(numOfTimes));
                //printf("Write - numOfTimes: %d\n", numOfTimes);
                //
                //free(child_search);
                //close(pvc[0]);
                //sending text occurence to parent
                //write(pi[1], numOfTimes, len(search_text));
                pid = getpid();
                int status2;
                //who2 = wait(&status2);
                //exit(1);
                //close(pvc[1]);
            }
            else if (pid != 0) {

               /* while(wait(NULL) > 0) {
                    printf("Child completed.");
                }*/

                int finalSearch;


                //read text values from child, close pvc[Write]
                close(pvc[1]);
                read(pvc[0], &finalSearch, sizeof(finalSearch));
                printf("Parent read finalSearch = %d: \n", finalSearch);
                int status;
                //who = wait(&status);
                printf("The parent PID is %d\n", getpid());
                printf("the parent of child %d is %d\n", pid, getpid());
                printf("Child process text answer is %d\n\n", finalSearch);
                k = argc;

            }


        }

    }
    

}
void sigHandler(int sigNum) {
        printf("\nChildren process successfully killed.\n");
        exit(0);
    }