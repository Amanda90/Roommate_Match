CXX = g++ -fPIC
C = gcc -g

NETLIBS= -lnsl -I./mysql -lmysqlclient

all: get_pairs student_server compat.o parser.o

compat.o: compat.c
	$(C) -c compat.c $(NETLIBS)
parser.o: parser.c
	$(C) -c parser.c $(NETLIBS)
	
student_server: student_server.c compat.o parser.o
	$(C) -o student_server student_server.c parser.o $(NETLIBS) -pthread

get_pairs: compat.o get_pairs.c
	$(C) -o get_pairs get_pairs.c compat.o $(NETLIBS)

%.o: %.cc
	@echo 'Building $@ from $<'
	$(C) -o $@ -c -I. $<

clean:
	rm -f server *.o

