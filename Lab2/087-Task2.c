#include<stdio.h>
#include <stdlib.h>
#include<time.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#define fileName "num.txt"

void write_nums_inAFile(){
    printf("Writing 1 to 100 numbers in a file\n");
    FILE *filePointer ; 
    filePointer = fopen(fileName,"w");
    for(int i=1; i<=100; i++)
        fprintf(filePointer, "%d ", i);
    fclose(filePointer);
    printf("Written\n\n");
}
char *child_code = "#include<stdio.h>\
\n#include <stdlib.h>\
\n#include<time.h>\
\n#include <stdbool.h>\
\n#define fileName \"num.txt\"\
\nvoid changeNum(int nums){ \
\n    printf(\"Changing numbers\\n\");\
\n    srand(time(0));\
\n    bool taken[101];\
\n    int i,r;\
\n    for(i=1; i<=100; i++) taken[i] = false;\
\n    i = 0;\
\n    while(i<nums){\
\n        r = (rand()%100)+1;\
\n        if(taken[r]) continue;\
\n        taken[r] = true;\
\n        i++;\
\n    }\
\n    FILE *filePointer, *filePointer2 ; \
\n    filePointer = fopen(fileName,\"r\");  \
\n    filePointer2 = fopen(\"tempfile.txt\",\"w\");\
\n    long int start, end;\
\n    int ti = 100;\
\n    start = 0;\
\n    while (!feof (filePointer)){\
\n        fscanf (filePointer, \"%d \", &i);\
\n        if(taken[i])\
\n            i = 0;   \
\n        fprintf(filePointer2, \"%d \", i);\
\n    }\
\n    fclose(filePointer);\
\n    fclose(filePointer2);\
\n    remove(fileName);\
\n    rename(\"tempfile.txt\", fileName);\
\n    printf(\"Changed numbers\\n\");\
\n}\
void main(){\
changeNum(10);\
}";

void check_nums(){
    FILE *filePointer;
    filePointer = fopen(fileName,"r");  
    int i;
    int num = 1;
    printf("\nNumbers below are replaced with 0 - \n");
    while (!feof (filePointer)){
        fscanf (filePointer, "%d ", &i);
        if(num != i){
            printf("%d\t",num);
        }
        num++;
    }
    printf("\n");
    fclose(filePointer);
}
void  execute(char **argv)
{
     pid_t  pid;
     int    status;
     if ((pid = fork()) < 0) {
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {
          if (execvp(*argv, argv) < 0) {
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     else {
          int wc = wait(NULL);
     }
}
void createChild(){
    FILE *filePointer;
    printf("Creating Child File\n");
    filePointer = fopen("087-Task2-Child.c","w");
    fprintf(filePointer, "%s", child_code);
    fclose(filePointer);  
    char *args[] = {"gcc","087-Task2-Child.c", "-o", "087-Task2-Child", NULL};
    printf("Compiling Child File\n");
    execute(args);
}
void runChild(){
    printf("Running Child Process\n");
    char *argstoRun[] = {"./087-Task2-Child", NULL};
    execute(argstoRun);
}
void main(){
    write_nums_inAFile(); 
    createChild();
    runChild();
    check_nums(); 
}