#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char * get_chars(char* toGet, char * getFrom, char * toReturn){
    char buffer[256];
    sprintf(buffer, "\"%s\":", toGet);
    char * loc = strstr(getFrom, toGet);
    int i = 0;
    int k= strlen(buffer);
    while(loc[k] != ';'){
        toReturn[i] = loc[k];
        k++;
        i++;
    }
    toReturn[i] = '\0';
    return toReturn;
}

int get_num(char * toGet, char * getFrom){
    char buffer[256];
    sprintf(buffer, "\"%s\":", toGet);
    char toReturn[256];
    char * loc = strstr(getFrom, toGet);
    int i = 0;
    int k = strlen(buffer);
    while(loc[k] != ';'){
        toReturn[i] = loc[k];
        k++;
        i++;
    }
    toReturn[i] = '\0';
    int numReturn = atoi(toReturn);
    return numReturn;
}

int get_multi_count(char * toGet, char * getFrom){
    char buffer[256];
    sprintf(buffer, "\"%s\"", toGet);
    char toReturn[256];
    char * loc = strstr(getFrom, toGet);
    int i = 0;
    int k = strlen(buffer);
    while(loc[k] != ')'){
        toReturn[i] = loc[k];
        printf("Checking: %c\n", loc[k]);
        k++;
        i++;
    }
    toReturn[i] = '\0';
    int numReturn = atoi(toReturn);
    return numReturn;
}

int get_multi(int index, char * toGet, char * getFrom, char * toReturn){
    char buffer[256];
    sprintf(buffer, "\"%s\"", toGet);
    char * loc = strstr(getFrom, toGet);
    int i = 0;
    int k = strlen(buffer);
    int count = 0;
    while(loc[k] != ':'){
        k++;
        i++;
    }
    i = 0;
    do {
        printf("Doing stuff. Count = %i\n", count);
        if(loc[k] != ','){
            toReturn[i] = loc[k];
        } else {
            i = 0;
            count ++;
        }
    }while (count < index);
    return toReturn;
}

