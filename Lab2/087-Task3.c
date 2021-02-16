#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include <stdbool.h>
#include<time.h>
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
void changeNum(int nums){
    printf("Changing numbers\n");
    srand(time(0));
    bool taken[101];
    int i,r;
    for(i=1; i<=100; i++) taken[i] = false;
    i = 0;
    while(i<nums){
        r = (rand()%100)+1;
        if(taken[r]) continue;
        taken[r] = true;
        i++;
    }
    FILE *filePointer, *filePointer2 ; 
    filePointer = fopen(fileName,"r");  
    filePointer2 = fopen("tempfile.txt","w");  
    long int start, end;
    int ti = 100;
    start = 0;
    while (!feof (filePointer)){
        fscanf (filePointer, "%d ", &i);
        if(taken[i])
            i = 0;   
        fprintf(filePointer2, "%d ", i);
    }
    fclose(filePointer);
    fclose(filePointer2);
    remove(fileName);
    rename("tempfile.txt", fileName);
    printf("Changed numbers\n");
}
void check_nums(){
    FILE *filePointer;
    filePointer = fopen(fileName,"r");  
    int i;
    int num = 1;
    printf("Numbers below are replaced with 0 - \n");
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

int main() 
{ 
	int p1[2];
	int p2[2];
	pid_t pid; 
	if (pipe(p1)==-1 || pipe(p2)==-1){ 
		fprintf(stderr, "Pipe Failed" ); 
		exit(1);
	} 
    if ((pid = fork()) < 0) {
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if(pid == 0){
        char msg_child[100];
        close(p1[1]);
        close(p2[0]);
		read(p1[0], msg_child, 100); 
        close(p1[0]);
        changeNum(10);
        write(p2[1], "DONE", 5); 
		close(p2[1]); 
        exit(0);
    }
    else{
        char msg_par[100]; 
		close(p1[0]);
        close(p2[1]); 
        write_nums_inAFile();
		write(p1[1], "START", 6); 
		close(p1[1]); 
        read(p2[0], msg_par, 100);
        check_nums();
        close(p2[0]); 
    }
} 
