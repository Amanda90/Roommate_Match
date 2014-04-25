//thing
const char * usage =
"                                                               \n"
"Server 		                                                \n"
"                                                               \n"
"Simple server program                                          \n"
"                                                               \n"
"To use it, type:      				                            \n"
"                                                               \n"
"   myhttpd [-f|-t|-p]   {port}                                 \n"
"                                                               \n"
"Where 1024 < port < 65536.     						        \n"
"and -f, -t, and -p specify processes, threads, and pool concurrency \n";

//#include <my_global.h>
#include <mysql.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <fstream> 
#include <time.h>

#define HTML 1
#define GIF 2
#define PLAIN 3
#define INDEX 4
#define DEFAULT 5
#define PROCESS 6
#define THREAD 7
#define POOL 8
#define SCRIPT 9
#define ICONS 10
#define DEFAULTPORT 8888
#define SERVER "localhost"
#define DATABASE "imgs"

int QueueLength = 5;

// Processes time request
void processTimeRequest( int socket );
void processRequest( int socket );
void processRequestThread( int socket );
void poolSlave( int socket );
int endsIn(char * string, char * endsIn);
int port;
int mode;
int folder;
char user[32];
char pwd[32];
pthread_mutex_t mutex;
MYSQL * connection;

int
main( int argc, char ** argv )
{
	srand(time(NULL));
	pthread_mutex_init(&mutex, NULL);
  // Print usage if not enough arguments
  /*if ( argc != 3 ) {
    fprintf( stderr, "%s", usage );
    exit( -1 );
  }*/
	//getting user and password...

	FILE * userFile = fopen("mysqluser", "rt");
	FILE * pwdFile = fopen("mysqlpwd", "rt");
	char nextChar;

	int i;
	int offset = 0;

	fgets(user, 32, userFile);
	fgets(pwd, 32, pwdFile);
	
	fclose(userFile);
	fclose(pwdFile);

	for(i = 0; i < 32; i++){
		if(user[i] == '\n'){
			user[i] = '\0';
		}
		if(pwd[i] == '\n'){
			pwd[i] = '\0';
		}
	}

	

	printf("Connecting to mySQL database\n");


	/** Here is the sample code to refer to when using mySQL
    MYSQL_RES *res_set; // Create a pointer to recieve the return value.
    MYSQL_ROW row;  // Assign variable for rows. 
    mysql_query(connect,"INSERT INTO img VALUES(16.2254,95.5564, 321548);");
    // Send a query to the database.
    unsigned int count = 0; // Create a counter for the rows 
 
	printf("Preparing to set res_set\n");
    res_set = mysql_store_result(connect); // Receive the result and store it in res_set 
	printf("res_set set.\n");
 
    unsigned int numrows = mysql_affected_rows(connect); // Create the count to print all rows
	printf("numrows set\n");
	printf("Numer of rows affected: %i\n", numrows);

	**/
 
    /* This while is to print all rows and not just the first row found, */
	
	if(argc == 3){
		if(!strcmp(argv[1], "-f")){
			mode = PROCESS;
		} else if (!strcmp(argv[1], "-t")){
			mode = THREAD;
		} else if(!strcmp(argv[1], "-p")){
			mode = POOL;
		} else if(!strcmp(argv[1], "-i")){
			mode = DEFAULT;
		}
		int tempPort = atoi(argv[2]);
		if(tempPort > 1024 && tempPort < 65536){
			port = tempPort;
		} else {
			printf("Invalid port number.\n");	
			exit(-1);
		}
		printf("Mode: %i\nPort: %i\n", mode, port);
	} else if(argc == 2){
		if(argv[1][0] == '-'){
			if(!strcmp(argv[1], "-f")){
				mode = PROCESS;
			} else if (!strcmp(argv[1], "-t")){
				mode = THREAD;
			} else if(!strcmp(argv[1], "-p")){
				mode = POOL;
			} else if(!strcmp(argv[1], "-i")){
				mode = DEFAULT;
			}
			port = DEFAULTPORT;
		} else {
			int tempPort = atoi(argv[0]);
			if(tempPort > 1024 && tempPort < 65536){
				port = tempPort;
				mode = DEFAULT;
			} else {
				printf("Invalid arguments\n");
				printf("\n\n%s", usage);
				exit(-1);
			}
		}
	} else {
		mode = DEFAULT;
		port = DEFAULTPORT;
	}
  
  // Set the IP address and port for this server
  struct sockaddr_in serverIPAddress; 
  memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
  serverIPAddress.sin_family = AF_INET;
  serverIPAddress.sin_addr.s_addr = INADDR_ANY;
  serverIPAddress.sin_port = htons((u_short) port);
  
  // Allocate a socket
  int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
  if (masterSocket < 0) {
    perror("socket");
    exit( -1 );
  }

  // Set socket options to reuse port. Otherwise we will
  // have to wait about 2 minutes before reusing the same port number
  int optval = 1; 
  int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(int));
   
  // Bind the socket to the IP address and port
  int error = bind( masterSocket,(struct sockaddr *)&serverIPAddress, sizeof(serverIPAddress));

  if (error){
    perror("bind");
    exit( -1 );
  }
  
  // Put socket in listening mode and set the 
  // size of the queue of unprocessed connections
  error = listen(masterSocket, QueueLength);
  if (error) {
    perror("listen");
    exit( -1 );
  }
	if(mode == POOL){
		pthread_t threads[5];
		pthread_attr_t attr;
		
		pthread_attr_init(&attr);
		pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
		for(int i = 0; i < 5; i++){
			pthread_create(&threads[i], &attr, (void *(*)(void *))poolSlave, (void*)masterSocket);
		}
		pthread_join(threads[0], NULL);
	} else {
		while ( 1 ) {
			// Accept incoming connections
			struct sockaddr_in clientIPAddress;
			int alen = sizeof(clientIPAddress);
			int slaveSocket = accept( masterSocket,
			      (struct sockaddr *)&clientIPAddress,
			      (socklen_t*)&alen);

			if(mode == DEFAULT){
				if ( slaveSocket < 0 ) {
				  perror( "accept" );
				  exit( -1 );
				}
				// Process request.
				//processTimeRequest( slaveSocket );
				processRequest( slaveSocket );
				
				// Close socket
				close( slaveSocket );
			} else if(mode == PROCESS){
				pid_t slave = fork();
				if(slave == 0){
					processRequest(slaveSocket);
					close(slaveSocket);
					exit(EXIT_SUCCESS);
				} else {
					close(slaveSocket);		
				}
			} else if(mode == THREAD){
				pthread_t newThread;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

				pthread_create(&newThread, &attr,(void * (*)(void*))processRequestThread, (void*)slaveSocket);
			}
		}
	}
}

void processRequestThread(int socket){
	processRequest(socket);
	printf("Connection closed\n");
	close(socket);
}

void poolSlave(int socket){
	struct sockaddr_in clientIPAddress;
	int alen = sizeof( clientIPAddress );

	while(1){
	
		pthread_mutex_lock(&mutex);
		int slaveSocket = accept( socket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
		pthread_mutex_unlock(&mutex);
		//check if accept worked
		processRequest(slaveSocket);
		close(slaveSocket);
	}
}

void processRequest(int socket){
	int n;
	int max = 1024;
	int docLength = 0;
	char newChar;
	char oldChar;
	int length = 0;
	int gotGet = 0;
	int i = 0;
	char curr_string[max+1];
	char docpath[max+1];
	int current;
	int content;
	int	bin = 0;

	printf("New request\n");

	//First, initialize curr_string to a series of null characters to prevent unwanted behavior.
	for(i = 0; i < max; i++){
		curr_string[i] = '\0';
	}

	while(n = read(socket, &newChar, sizeof(newChar))){
		if(newChar != '\0'){
			curr_string[docLength] = newChar;
			docLength ++;
		} else {
			break;	
		}
	}
	curr_string[docLength] = '\0';

	docLength = 0;
	float lat;
	float lon;
	char latString[7];
	char lonString[7];
	char * directory = "imgs/";
	char name[23];
	void * buffer = malloc(1024);
	char query[1024];

	if(strcmp("POST", curr_string) == 0){
		//Request to put a picture to the server
		
		printf("Post request\n");

		for(i = 0; i < max; i++){
			curr_string[i] = '\0';
		}
		//the first two parts will be latitude and longitude:

		n = read(socket, &latString, 7);
		
		n = read(socket, &lonString, 7);
		lat = atof(latString);
		lon = atof(lonString);


		int offset = 0;
		char c;

		//put image data in database

		int err = sprintf(query, "INSERT INTO img VALUES(%f,%f,%i)", lat, lon, rand());	

		connection = mysql_init(NULL);

		mysql_real_connect(connection,SERVER,user,pwd,DATABASE,2546,"/homes/csreid/mysqld/mysqld.sock",0);
	
		if(!connection){
			fprintf(stderr, "Connection failed\n");
		} else {
			printf("Connected\n");	
		}

   		MYSQL_RES *res_set; // Create a pointer to recieve the return value.
	    MYSQL_ROW row;  // Assign variable for rows. 	

   	    err = mysql_query(connection, query);

	    mysql_close(connection);   /* Close and shutdown */



		//Get the image.		

		strcat(name, directory);
		strcat(name, latString);
		strcat(name, lonString);
		strcat(name, ".jpg");

		FILE * file = fopen(name, "a");
		
		n = read(socket, &newChar, 1);

		fwrite(&newChar, 1, 1, file);

		while(read(socket, &newChar, 1) != 0){
			fwrite(&newChar, 1, 1, file);
		}
		printf("File recieved.\n");
		
		fclose(file);

	} else if(!strcmp("UPDATE", curr_string)){
		//First part will be user's lat and lon:
		//(same code as in POST request)

		printf("Update request\n");

		n = read(socket, &latString, 7);
		n = read(socket, &lonString, 7);
		lat = atof(latString);
		lon = atof(lonString);
		
		//a difference of .01 in lat or lon values corresponds to about 1km separation. So, these values will give all pictures within 1 km of the user.
		double maxLat = lat + .01;
		double minLat = lat - .01;

		double maxLon = lon + .01;
		double minLon = lon - .01;

		/* Sample query to get values in range:
		SELECT * FROM img WHERE lat <= 59.0212 AND lat >= 59.0012 AND lon <= 35.1425 AND lon >= 35.1225;
		*/

		//int err = sprintf(query, "SELECT timestamp FROM img WHERE lat <= %f AND lat >= %f AND lon <= %f AND lon >= %f", maxLat, minLat, maxLon, minLon);
		int err = sprintf(query, "SELECT * FROM img WHERE lat <= %f AND lat >= %f AND lon <= %f AND lon >= %f", maxLat, minLat, maxLon, minLon);


		//connect to database, send the query, and return the results
		connection = mysql_init(NULL);

		mysql_real_connect(connection,SERVER,user,pwd,DATABASE,2546,"/homes/csreid/mysqld/mysqld.sock",0);
	
		if(!connection){
			fprintf(stderr, "Connection failed\n");
		}

   		MYSQL_RES *res_set; // Create a pointer to recieve the return value.
	    MYSQL_ROW row;  // Assign variable for rows. 	

   	    err = mysql_query(connection, query);

		res_set = mysql_store_result(connection);

		unsigned int numrows = mysql_num_rows(res_set);

		int values[numrows];

		for(i = 0; i < numrows; i++){
			row = mysql_fetch_row(res_set);
			if(!row[2]){
				printf("NULL");
				exit(0);
			} else {
				values[i] = atoi(row[2]);
			}
		}

	    mysql_close(connection);   /* Close and shutdown */

		//send the keys of the valid images
		//data is formatted like:

		/*
			<NUMBER OF ELEMENTS>\r\n
			<ELEMENT 1>
			<ELEMENT 2>
			etc...
		*/
		printf("%i valid rows\n", numrows);
		write(socket, &numrows, sizeof(int));
		write(socket, "\r\n", 2);

		for(i = 0; i < numrows; i++){
			write(socket, &values[i], sizeof(int));
		}
		
		
	} else if (!strcmp("REQUEST", curr_string)){
		//send the phone the image requested

		printf("Image request\n");
		char keyString[9];
		int key;
		
		n = read(socket, &keyString, 9);
		key = atoi(keyString);

		printf("Key: %i\n", key);

		//connect to database
		connection = mysql_init(NULL);

		mysql_real_connect(connection,SERVER,user,pwd,DATABASE,2546,"/homes/csreid/mysqld/mysqld.sock",0);
	
		if(!connection){
			fprintf(stderr, "Connection failed\n");
		}

		//construct query
		int err = sprintf(query, "SELECT * FROM img WHERE timestamp = %i", key);

		//send query
   		MYSQL_RES *res_set; // Create a pointer to recieve the return value.
	    MYSQL_ROW row;  // Assign variable for rows. 	

   	    err = mysql_query(connection, query);

		res_set = mysql_store_result(connection);

		unsigned int numrows = mysql_num_rows(res_set);
		
		row = mysql_fetch_row(res_set);


		//construct path to requested image, store path in "name"
		strcat(name, "imgs/");
		strcat(name, row[0]);
		strcat(name, row[1]);
		strcat(name, ".jpg");

		//respond with requested file
		write(socket, "INCOMING", 8);
		
		FILE * file = fopen(name, "a");
		int count;
		int offset = 0;
		printf("Sending requested file...\n");
		while(count = pread(fileno(file), buffer, 1024, offset)){
			if(write(socket, buffer, 1024) != count){
				//perror("write");
			}
			offset += 1024;
		}
		printf("Sent");
		

	}else {
		printf("WTF\n\n");
	}
}

int endsIn(char* string, char* endsIn){
	int strSize = strlen(string);
	int endSize = strlen(endsIn);
	int counter = strSize - endSize;
	int j = 0;
	int match = 1;
	
	while(counter <= strSize){
		if(string[counter] != endsIn[j]){
			match = 0;
		}
		counter++;
		j++;
	}
	return match;
}
