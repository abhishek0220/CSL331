#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h> 
#include<string.h>
#include <stdbool.h> 
#include<sys/wait.h> 
#include <sys/utsname.h>
#include <pwd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <fcntl.h>
#include<signal.h> 

struct node {
   char* command;
   char* cName;
   pid_t pid;
   bool completed;
   bool foreGround;
   struct node *next;
   struct node *prev;
};

struct stack
{
    struct node *head;
    struct node *end;
};

struct stack stack1;

void append(struct stack *stack_, char *data, pid_t pid, bool running) {
    struct node *newNode = (struct node*) malloc(sizeof(struct node));
    int i=0;
    while(data[i]!= '\0'){
        if(data[i] == ' ')break;
        i++;
    }
    char *cName = (char*)(malloc(sizeof(char)*(i+1)));
    strncpy(cName, data, i);
    cName[i] = '\0';
    newNode->command = data;
    newNode->next = NULL;
    newNode->cName = cName;
    newNode->pid = pid;
    newNode->completed = !running;
    newNode->foreGround = !running;
    newNode->prev = stack_->end;
    if(stack_->end != NULL)
        stack_->end->next = newNode;
    stack_->end = newNode;
    if(stack_->head == NULL)
        stack_->head = newNode;
}

void print(struct stack *stack_, bool fullName) {
    int i=1;
    struct node *ptr = stack_->head;
    while(ptr != NULL) {
        printf("%d. ",i);
        char tem = '&';
        if(ptr->foreGround) tem = '\0';
        if(fullName)
            printf("%c%s",tem,ptr->command);
        else
            printf("%c%s",tem,ptr->cName);
        printf("\n");
        ptr = ptr->next;
        i++;
    }
}

bool markComplete(struct stack *stack_, pid_t pid, char **name) {
    struct node *ptr = stack_->head;
    while(ptr != NULL) {
        if(ptr->pid == pid){
            *name = ptr->cName;
            if(ptr->completed) return false;
            ptr->completed = true; return true;
        }
        ptr = ptr->next;
    }
    return false;
}

void printProcess(struct stack *stack_, char type[]){
    struct node *ptr = stack_->head;
    bool printAll;;
    if(strcmp(type, "all")==0) printAll = true;
    else if(strcmp(type, "current")==0) printAll = false;
    else{
        printf("please use correct argument for pid\n");
        return;
    }
    while(ptr != NULL){
        if(printAll || !ptr->completed)
            printf("command name: %s\tprocess id : %d\n",ptr->cName, ptr->pid);
        ptr = ptr->next;
    }
    
}

void printlastN(struct stack *stack_, int N){
    printf("Printing last %d commands entered\n",N);
    if(N < 1) return;
    struct node *ptr = stack_->end;
    int i=1;
    while(N>0 && ptr != NULL){
        char tem = '&';
        if(ptr->foreGround) tem = '\0';
        printf("%d. ",i++);
        printf("%c%s\n",tem,ptr->cName);
        ptr = ptr->prev;
        N--;
    }
}

void clearStack(struct stack *stack_) {
    struct node *ptr = stack_->head;
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

char *trimwhitespace(char *comstr)
{
    char *str = comstr;
    char *end;
    while(*str == ' ' || *str == '\t') str++;
    if(*str == 0)
    return str;
    end = str + strlen(str) - 1;
    while(end > str && (*end==' ' || *end == '\t')) end--;
    end[1] = '\0';
    return str;
}

bool isNum(char *str){
    int i=0;
    while(str[i] != '\0'){
        if(str[i] < 48 || str[i] > 57) return false;
        i++;
    }
    return true;
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

pid_t  execute(char **argv, bool towait, bool *running)
{
     pid_t pid;
     int status;
     if ((pid = fork()) < 0) {
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {
         if(towait == false){
             int fd = open("/dev/null", O_WRONLY);
             dup2(fd, 1);
             dup2(fd, 2);
             close(fd);    
         }
          if (execvp(*argv, argv) < 0) {
              printf("Error: command not found\n");
               exit(1);
          }
     }
     else {
         if(towait){
             waitpid(pid, NULL, 0);
             *running = false;
         }  
     }
     return pid;
}

void executeFromLine(char *lineWithSpace, bool towait){
    char *line = trimwhitespace(lineWithSpace);
    int numberOfArgs=1;
    int i=0;
    bool gotSpace;
    while(line[i] != '\0' && line[i] != '\n'){
        if(line[i] == ' ' || line[i] == '\t'){
            gotSpace =  true;
        }
        else{
            if(gotSpace) numberOfArgs++;
            gotSpace = false;
        }
        i++;
    }
    char *command = (char*)(malloc(sizeof(char)*(i+1)));
    strncpy(command, line, i);
    command[i] = '\0';
    char *args[numberOfArgs+1];
    int start = 0;
    i = -1;
    int j=0;
    do{
        i++;
        if(line[i] == ' ' || line[i] == '\t' || line[i] == '\0' || line[i] == '\n'){
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
    bool running = true;
    pid_t process_pid = execute(args, towait, &running);
    append(&stack1, command, process_pid, running);
    i = 0;
    while(i<j){
        free(args[i]);
        i++;
    }
}

void runCommandByID(struct stack *stack_, int num){
    struct node *ptr = stack_->head;
    bool con = true;
    while(ptr!=NULL && num > 0){
        if(num == 1){
            executeFromLine(ptr->command, ptr->foreGround);
            con = false;
            break;
        }
        ptr = ptr->next;
        num--;
    }
    if(con)
        printf("Command not found\n");
}

void runCommandByName(struct stack *stack_, char *command){
    struct node *ptr = stack_->head;
    bool con = true;
    while(ptr!=NULL){
        if(strcmp(ptr->cName, command) == 0){
            executeFromLine(ptr->command, ptr->foreGround);
            con = false;
            break;
        }
        ptr = ptr->next;
    }
    if(con)
        printf("Command not found\n");
}

void runlastN(struct stack *stack_, int N){
    if(N < 1) return;
    struct node *ptr = stack_->end;
    N--;
    while(N>0 && ptr != NULL){
        ptr = ptr->prev;
        N--;
    }
    if(ptr != NULL) executeFromLine(ptr->command, ptr->foreGround);
}

void handler(int sig){
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    char *name;
    bool stat = markComplete(&stack1, pid, &name);
    if(stat)
        printf("\nProcess having PID: %d running command \"%s\" exited with status %d\n",pid, name, status);
}

void main(int argc, char** argv){
    signal(SIGCHLD, handler);
    stack1.head = NULL;
    stack1.end = NULL;
    int t=15;
    struct passwd *pws;
    char ht[1024];
    ht[1023] = '\0';
    char cwd[PATH_MAX];
    char par_path[PATH_MAX];
    getcwd(par_path, sizeof(par_path));
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *point_cwd;
    char ref;
    do{
        pws = getpwuid(geteuid());
        gethostname(ht, 1023);
        getcwd(cwd, sizeof(cwd));
        point_cwd = cwd;
        ref = '\0';
        if(startsWith(par_path, cwd)){
            point_cwd = &point_cwd[strlen(par_path)];
            ref = '~';
        }
        printf("<%s@%s:%c%s>", pws->pw_name, ht,ref,point_cwd);
        nread = getline(&line, &len, stdin);
        line[nread-1] = '\0';
        char *comGot;
        comGot = trimwhitespace(&line[0]);
        if(strcmp(comGot, "cd")==0){
            if (chdir(par_path) != 0)  
                perror("Error: ");
        }
        else if(startsWith("cd ",comGot) || startsWith("cd\t",comGot)){
            comGot = &comGot[3];
            if (chdir(comGot) != 0)  
                perror("Error: ");
        }
        else if(strcmp(comGot,"HISTORY BRIEF") == 0)
            print(&stack1, false);
        else if(strcmp(comGot, "HISTORY FULL")==0)
            print(&stack1, true);
        else if(startsWith("EXEC ",comGot) || startsWith("EXEC\t",comGot)){
            comGot = &comGot[5];
            bool num = isNum(comGot);
            if(num){
                int a = atoi(comGot);
                runCommandByID(&stack1,a);
            }
            else
                runCommandByName(&stack1, comGot);
        }
        else if(strcmp(comGot, "pid")==0)
            printf("command name: %s  process id: %d\n",argv[0], getpid());
        else if(startsWith("pid ",comGot) || startsWith("pid\t",comGot)){
            comGot = &comGot[4];   
            printProcess(&stack1, comGot);
        }
        else if(startsWith("HIST",comGot)){
            comGot = &comGot[4];
            if(!isNum(comGot))
                printf("Please enter valid argument\n e.g. !HIST<Number of Commands>\n");
            else{
                int a = atoi(comGot);
                printlastN(&stack1, a);
            }
        }
        else if(startsWith("!HIST",comGot)){
            comGot = &comGot[5];
            if(!isNum(comGot))
                printf("Please enter valid argument\n e.g. !HIST<Number of Commands>\n");
            else{
                int a = atoi(comGot);
                runlastN(&stack1, a);
            }
        }
        else if(startsWith("STOP",comGot)){
            clearStack(&stack1);
            printf("Exiting normally, bye\n");
            break;
        }
        else if(comGot[0] == '&'){
            comGot = &comGot[1];
            executeFromLine(comGot, false);
        }
        else if(comGot[0] != '\0' ){
            executeFromLine(comGot, true);
        }
        printf("\n");
    }while(1);
}