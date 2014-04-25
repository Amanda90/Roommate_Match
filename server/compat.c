#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>

#define SERVER "localhost"
#define DATABASE "students"

int str_to_bool(char * string){
    if(!strcmp(string, "true")){
        return 1;
    } else {
        return 0;
    }
}


int main(int argc, char ** argv){
    char user[32];
    char pword[32];

    MYSQL * connection;
    FILE * userFile = fopen("mysqluser", "rt");
    FILE * pwdFile = fopen("mysqlpwd", "rt");

    char nextChar;
    int i, j, k;
    int offset = 0;
    char * query = "SELECT * FROM student";
    int err;

    fgets(user, 32, userFile);
    fgets(pword, 32, pwdFile);
    for(i = 0; i < 32; i++){
        if(user[i] == '\n'){
            user[i] = '\0';
        }
        if(pword[i] == '\n'){
            pword[i] = '\0';
        }
    }

    printf("Connecting to mySQL database...\n");
    connection = mysql_init(NULL);
    mysql_real_connect(connection, SERVER, user, pword, DATABASE, 2546, "/homes/csreid/mysqld/mysqld.sock", 0);
    if(!connection){
        fprintf(stderr, "connection failed\n");
    } else {
        printf("Connected\n");
    }

    MYSQL_RES * results;

    err = mysql_query(connection, query);
    results = mysql_store_result(connection);
    unsigned int numrows = mysql_num_rows(results);
    MYSQL_ROW rows[numrows];
    int total;

    int values[numrows];
    int compat_table_size =(numrows*(numrows-1))/2 ;
    long compat_table[compat_table_size][3];
    printf("Number of rows: %i\n", numrows);
    printf("Table size: %i\n", numrows);

    for(i = 0; i < numrows; i++){
        printf("fetching row...\n");
        rows[i] = mysql_fetch_row(results);
        printf("fetched:");
        for(j = 0; j < 14; j++){
            printf("%s ", rows[i][j]);
        }
        printf("\n");
    }
    printf("Generating compat table\n");

    i = 0;
    j = numrows -1 ;

    while(i < j){
        printf("thingy1: %s\n", rows[j][12]);
        printf("thingy2: %s\n", rows[i][12]);
        compat_table[i][0] = atol((char *)rows[j][12]);
        compat_table[i][1] = atol((char *)rows[i][12]);
        printf("table 1: %ld\n", compat_table[i][0]);
        printf("table 2: %ld\n\n", compat_table[i][1]);
        total = 0;
        for(k = 4; k <= 6; k++){
            total += ((str_to_bool(rows[i][k])-str_to_bool(rows[j][k])*10) * ((str_to_bool(rows[i][k])-str_to_bool(rows[j][k])*10)));
        }
        if(!strcmp(rows[i][7], rows[j][7])){
            //do nothing
        }else{
            total += 100;
        } 
        for(k = 8; k <= 11; k++){
            if(k != 11){
                total += ((str_to_bool(rows[i][k])-str_to_bool(rows[j][k])*10) * ((str_to_bool(rows[i][k])-str_to_bool(rows[j][k])*10))); 
            } else {
                total += ((str_to_bool(rows[i][k])-str_to_bool(rows[j][k])/10) * ((str_to_bool(rows[i][k])-str_to_bool(rows[j][k])/10))); 
            }
        }
        compat_table[i][2] = sqrt(total);
        i++;
        j--;
    }

        printf("  s_id 1  |  s_id 2  | compat\n");
        printf("----------+----------+----------\n");
    for(i = 0; i < compat_table_size; i++){
        printf("%ld|%ld|   %ld  \n", compat_table[i][0], compat_table[i][1], compat_table[i][2]);
        printf("----------+----------+------\n");
    }
    printf("Done!\n");
}
