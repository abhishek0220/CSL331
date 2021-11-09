#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h>
#include <stdbool.h> 
#include<sys/wait.h> 

struct node {
   char* command;
   char* cName;
   struct node *next;
};

struct node *stack1 = NULL;

void append(struct node **head, char *data) {
    """
    Insert data in the end of the node
    """
    struct node *newNode = (struct node*) malloc(sizeof(struct node));
    int i=0;
    while(data[i]!= '\0'){
        if(data[i] == ' ')break;
        i++;
    }
    char *cName = (char*)(malloc(sizeof(char)*(i+1))); // command name
    strncpy(cName, data, i);
    cName[i] = '\0';
    newNode->command = data; // complete command
    newNode->next = NULL;
    newNode->cName = cName; // only command name
    if(*head == NULL)
        *head = newNode;
    else{
        struct node *ptr = *head;
        while(ptr->next != NULL) ptr = ptr->next;
        ptr->next = newNode;
    }
}

void print(struct node **head, bool con) {
    """
    Print all the nodes 
    if con -> print the full command
    else -> print only the command name
    """
    int i=1;
    struct node *ptr = *head;
    while(ptr != NULL) {
        printf("%d. ",i);
        if(con)
            printf("%s",ptr->command);
        else
            printf("%s",ptr->cName);
        printf("\n");
        ptr = ptr->next;
        i++;
    }
    printf("\n");
}


void clearStack(struct node **head) {
    """
    Clear the stack
    """
    struct node *ptr = *head;
    if(ptr == NULL) return;
    struct node *next;
    while(ptr != NULL) {
        next = ptr->next;
        free(ptr->cName);
        free(ptr->command);
        free(ptr);
        ptr = next;
    }
}

//----------Utility Funtions--------------------
char *trimwhitespace(char *comstr){
    """
    Remove the white spaces from the string
    """
    char *str = comstr;
    char *end;
    while(*str == ' ') str++;
    if(*str == 0)
    return str;
    end = str + strlen(str) - 1;
    while(end > str && *end==' ') end--;
    end[1] = '\0';
    return str;
}

bool startsWith(const char *pre, const char *str)
{
    """
    Check if the string starts with the prefix
    """
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

bool isNum(char *str){
    """
    Check if the string is a number
    """
    int i=0;
    while(str[i] != '\0'){
        if(str[i] < 48 || str[i] > 57) return false;
        i++;
    }
    return true;
}

//------------------To Execute command using args array
void  execute(char **argv)
{
    """
    Execute the command
    """
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

//------------------To call execute funtion from single line-------
void executeFromLine(char *lineWithSpace){
    """
    Execute the command from the line
    """
    char *line = trimwhitespace(lineWithSpace);
    int numberOfArgs=1;
    int i=0;
    int len = 0;
    while(line[i] != '\0' && line[i] != '\n'){
        if(line[i] == ' ') numberOfArgs++;
        i++;
        len++;
    }
    char *command = (char*)(malloc(sizeof(char)*(len+1)));
    strncpy(command, line, len);
    command[len] = '\0';
    append(&stack1, command);
    char *args[numberOfArgs];
    int start = 0;
    i = -1;
    int j=0;
    do{
        i++;
        if(line[i] == ' ' || line[i] == '\0' || line[i] == '\n'){
            int length = i-start;
            if(length > 0){
                args[j] = (char*)(malloc(sizeof(char)*(length+1)));
                strncpy(args[j], line+start, length );
                args[j][length] = '\0';
                j++;
            }
            start = i+1;
        }
    }while(line[i] != '\0');
    args[j] = NULL;
    execute(args);
    i = 0;
    while(i<j){
        free(args[i]);
        i++;
    }
}

//------------------Extract Line from File and call excuteFromLine-------
void executeFromFile(char file_loc[]){
    """
    Execute the command from the file
    """
    FILE *filePointer; 
    filePointer = fopen(file_loc, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, filePointer)) != -1) {
        executeFromLine(line);
    }
    free(line);
    fclose(filePointer);
}

//------------------Run from ID(INT)-------
void runCommandByID(struct node **head, int num){
    """
    Run the command from the ID
    """
    struct node *ptr = *head;
    bool con = true;
    while(ptr!=NULL && num > 0){
        if(num == 1){
            executeFromLine(ptr->command);
            con = false;
            break;
        }
        ptr = ptr->next;
        num--;
    }
    if(con)
        printf("Command not found\n");
}

//------------------Run By Name-------
void runCommandByName(struct node **head, char *command){
    """
    Run the command from the name
    """
    struct node *ptr = *head;
    bool con = true;
    while(ptr!=NULL){
        if(strcmp(ptr->cName, command) == 0){
            executeFromLine(ptr->command);
            con = false;
            break;
        }
        ptr = ptr->next;
    }
    if(con)
        printf("Command not found\n");
}

void main(int argc, char** argv){
    for(int i=1; i<argc; i++)
        executeFromFile(argv[i]);
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int i=5;
    print(&stack1, true);
    do{
        printf("Please enter your command:\n");
        nread = getline(&line, &len, stdin);
        line[nread-1] = '\0';
        char *comGot;
        comGot = trimwhitespace(&line[0]);
        if(strcmp(comGot,"HISTORY BRIEF") == 0)
            print(&stack1, false);
        else if(strcmp(comGot, "HISTORY FULL")==0)
            print(&stack1, true);
        else if(startsWith("EXEC ",comGot)){
            comGot = &comGot[5];
            bool num = isNum(comGot);
            if(num){
                int a = atoi(comGot);
                runCommandByID(&stack1,a);
            }
            else
                runCommandByName(&stack1, comGot);
        }
        else if(startsWith("STOP",line)){
            clearStack(&stack1);
            printf("Exiting normally, bye\n");
            break;
        }
        else printf("Invalid Command\n");
    }while(1);
}
