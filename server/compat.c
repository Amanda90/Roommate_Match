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

#define SERVER "localhost"
#define DATABASE "students"


int main(int argc, char ** argv){
    char user[32];
    char pword[32];

    MYSQL * connection;
    FILE * userFile = fopen("mysqluser", "rt");
    FILE * pwdFile = fopen("mysqlpwd", "rt");

    char nextChar;
    int i, j;
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
    MYSQL_ROW row;

    err = mysql_query(connection, query);
    results = mysql_store_result(connection);
    unsigned int numrows = mysql_num_rows(results);

    int values[numrows];
    int compat_table[(numrows*(numrows-1))/2];
    printf("Number of rows: %i\n", numrows);
    printf("Table size: %i\n", numrows);

    for(i = 0; i < numrows; i++){
        for(j = 1; j < numrows; j++)
        row = mysql_fetch_row(results);
        int j = 0;
        for(j; j < 14; j++){
            j++;
            fprintf(stderr, "%s ", row[j]);
        }
        printf("Out of while loop\n");
    }
    printf("Done!\n");
}
