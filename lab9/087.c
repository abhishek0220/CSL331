#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

FILE *arrFile;
FILE *qrFile;

int *queryInt;
int querySize;

int sortedTill;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct searchSpace {
   int *arr;
   int size;
};

struct arguments {
   int index;
   int key;
};

struct searchSpace *dataArrayList[1000];
int totLines = 0;

void delay(int milli_seconds)
{ 
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

char *trimwhitespace(char *comstr)
{
    char *str = comstr;
    char *end;
    while(*str == ' ') str++;
    if(*str == 0)
    return str;
    end = str + strlen(str) - 1;
    while(end >= str && (*end==' ' || *end == '\n')) end--;
    end[1] = '\0';
    return str;
}

int getNumIntegerLength(char *str){
    int tot = 0;
    int end = 1;
    for(int i=0; i<strlen(str); i++){
        if(str[i] >= '0' && str[i] <= '9') end = 0;
        else if(end == 0){
            end = 1;
            tot++;
        }
    }
    if(end == 0) tot++;
    return tot;
}

void getInt(int *arr, char *str){
    int in = 0;
    int end = 1;
    int num = 0;
    for(int i=0; i<strlen(str); i++){
        if(str[i] >= '0' && str[i] <= '9'){
            num = num*10 + (str[i] - '0');
            end = 0;
        }
        else if(end == 0){
            end = 1;
            arr[in] = num;
            num = 0;
            in++;
        }
    }
    if(end == 0){
        arr[in] = num;
    }
}

void readQuery(){
    int intnum;
    char *line = NULL;
    char *stripedLine;
    size_t len = 0;
    while (!feof (qrFile)){
        getline(&line, &len, qrFile);
        stripedLine = trimwhitespace(line);
        intnum = getNumIntegerLength(stripedLine);
        queryInt = (int*)malloc(sizeof(int)*(intnum));
        getInt(queryInt, stripedLine);
        querySize = intnum;
        break;
    }
}

void readData(){
    int intnum;
    char *line = NULL;
    char *stripedLine;
    size_t len = 0;
    while (!feof (arrFile)){
        getline(&line, &len, arrFile);
        
        stripedLine = trimwhitespace(line);
        intnum = getNumIntegerLength(stripedLine);
        if(intnum == 0) continue;
        int *tem = (int*)malloc(sizeof(int)*(intnum));
        getInt(tem, stripedLine);
        //printf("H3\n");
        dataArrayList[totLines] = (struct searchSpace*) malloc(sizeof(struct searchSpace));
        //printf("H4\n");
        dataArrayList[totLines]->arr = tem;
        dataArrayList[totLines]->size = intnum;
        //printf("H5\n");
        totLines++;
    } 
}

void freeSpaces(){
    free(queryInt);
    for(int i=0; i<totLines; i++){
        free(dataArrayList[i]->arr);
        free(dataArrayList[i]);
    }
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int getOrUpdateSortedIndex(int con){
    int tem;
    pthread_mutex_lock(&lock);
    if(con == 1) sortedTill++;
    //else printf("ele now is %d\n", sortedTill);
    tem = sortedTill;
    pthread_mutex_unlock(&lock);
    return tem;
}

void* sortThread(void * arg){
    for(int i=0; i<totLines; i++){
        qsort(dataArrayList[i]->arr, dataArrayList[i]->size, sizeof(int), cmpfunc);
        delay(100);
        getOrUpdateSortedIndex(1);
    }
    return NULL;
}

void* searchThread(void * arg){
    struct arguments *tem = (struct arguments*) arg;
    struct searchSpace *args = dataArrayList[tem->index];
    int *temarr = args->arr;
    //printf("Searching %d in array %d\n", tem->key, tem->index);
    for(int i=0; i<args->size; i++){
        if(temarr[i] == tem->key) printf("Found integer %d in array %d at index %d\n", tem->key, tem->index, i);
    }
    return NULL;
}

void printARR(){
    for(int i=0; i<totLines; i++){
        int *tem;
        tem = dataArrayList[i]->arr;
        for(int j=0; j<dataArrayList[i]->size; j++){
            printf("%d\t", tem[j]);
        }
        printf("\n");
    }
}

void searchAll(int key){
    int sortedByIndex = getOrUpdateSortedIndex(0);
    pthread_t searching[sortedByIndex];
    //printf("here\n");
    struct arguments *args[sortedByIndex];
    for(int i=0; i<sortedByIndex; i++){
        args[i] = (struct arguments*)malloc(sizeof(struct arguments));;
        args[i]->index = i;
        args[i]->key = key;
        pthread_create(&searching[i],NULL , searchThread, args[i]);
    }
    for(int i=0; i<sortedByIndex; i++){
        pthread_join(searching[i], NULL);
        free(args[i]);
    }
}

int main(){
    sortedTill = 0;
    arrFile = fopen("arraydata.txt", "r");
    qrFile = fopen("querydata.txt","r");
    readQuery();
    //printf("H1\n");
    readData();
    //printf("H2\n");
    fclose(arrFile);
    fclose(qrFile);
    
    pthread_t sorting;
    pthread_create(&sorting,NULL , sortThread, NULL);
    delay(100);
    for(int i=0; i<querySize; i++){
        //printf("will search %d\n", queryInt[i]);
        searchAll(queryInt[i]);
        delay(90);
    }
    pthread_join(sorting, NULL);
    freeSpaces();
    return 0;
}