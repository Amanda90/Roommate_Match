#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
char * get(char* toGet, char * getFrom){
    char buffer[256];
    sprintf(buffer, "\"%s\":", toGet);
    char * toReturn = (char *)malloc(256 * sizeof(char));
    char * loc = strstr(getFrom, toGet);
    int i = 0;
    int k= strlen(buffer);
    while(loc[k] != ';'){
        toReturn[i] = loc[k];
        k++;
        i++;
    }
    return toReturn;
}

int main(){
    int jsonFile = open("json", O_RDONLY);
    char json[2048];
    int end = read(jsonFile, json, 2048);
    json[end] = '\0';
    printf("fname: %s\nlname: %s\n", get("fname", json), get("lname", json));
}
