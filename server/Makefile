CXX = g++ -fPIC
C = gcc

NETLIBS= -lnsl -I./mysql -lmysqlclient

all: server compat.o parser.o

server: server.cpp
	$(CXX) -o server $@.cpp $(NETLIBS) -pthread
compat.o: compat.c
	$(C) -o compat.o compat.c $(NETLIBS)
parser.o: parser.c
	$(CXX) -o parser.o parser.c $(NETLIBS)
	

%.o: %.cc
	@echo 'Building $@ from $<'
	$(C) -o $@ -c -I. $<

clean:
	rm -f server

