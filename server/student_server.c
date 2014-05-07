#include <mysql.h>
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
#define DEFAULTPORT 3377
#define SERVER "localhost"
#define DATABASE "imgs"
int QueueLength = 5;
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

extern get_(char * toGet, char * getFrom, char * toReturn);
extern get_chars(char * toGet, char * getFrom, char * toReturn);
extern get_multivalued(char* toGet, char* getFrom, char * toReturn);

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
		mode = THREAD;
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
        int i;
		
		pthread_attr_init(&attr);
		pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
		for(i = 0; i < 5; i++){
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
	//char query[1024];
    printf("Current string: %s\n", curr_string);
    char buffer[256];
    int formNum = get_num("form", curr_string);
    int importance_of_grades;
    if(formNum == 1){
        get_chars("fname", curr_string, buffer);
        char * fname = strdup(buffer);
        get_chars("lname", curr_string, buffer);
        char * lname = strdup(buffer);
        get_chars("gender", curr_string, buffer);
        char * gender = strdup(buffer);
        get_chars("year", curr_string, buffer);
        char * year = strdup(buffer);
        get_chars("smoking", curr_string, buffer);
        char * smoking = strdup(buffer);
        get_chars("pledge", curr_string, buffer);
        char * pledge = strdup(buffer);
        get_chars("drink", curr_string, buffer);
        char * drink = strdup(buffer);
        get_chars("religion", curr_string, buffer);
        char * religion = strdup(buffer);
        get_chars("shared_before", curr_string, buffer);
        char * shared_before = strdup(buffer);
        get_chars("early_bird", curr_string, buffer);
        char * early_bird = strdup(buffer);
        get_chars("snore", curr_string, buffer);
        char * snore = strdup(buffer);
        get_chars("clean", curr_string, buffer);
        char * clean = strdup(buffer); 
        importance_of_grades = get_num("importance_of_grades", curr_string);

        char s_id[10];
        connection = mysql_init(NULL);
        mysql_real_connect(connection, SERVER, user, pwd, DATABASE, 2546, "/homes/csreid/myqld/mysqld.sock", 0);
        if(!connection){
            fprintf(stderr, "Connection failed\n");
        } else {
            printf("Connected\n");
        }

        char * query = (char *)malloc(2048 * sizeof(char));
        /*Table student:
         * lname
         * fname
         * gender
         * year
         * smoking
         * pledge
         * party
         * religion
         * shared_before
         * early_bired
         * snore
         * importance_of_grades
         * s_id*/
        sprintf(query, "INSERT INTO student VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %i, %s)", lname, fname, gender, year, smoking, pledge, drink, religion, shared_before, early_bird, snore, importance_of_grades, s_id);
        int err = mysql_query(connection, query);
        if(!err){
            fprintf(stderr, "ERROR: %i\n", err);
        }

    } else {
        printf("Second form!\n");
    }
   /* int sports_count = get_multi_count("sports", curr_string);
    printf("Number of sports: %i\n", sports_count);
    get_multi(0, "sports", curr_string, buffer);
    char * first_sport = strdup(buffer);
    printf("First sport: %s\n", first_sport);

    printf("Drink: %s\n", drink);*/
    
}

