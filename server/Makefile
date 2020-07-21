server:server.o main.o
	gcc server.o main.o -o server -lsqlite3

server.o:server.c
	gcc -c server.c -o server.o

main.o:main.c
	gcc -c main.c -o main.o -lsqlite3

clean:
	rm -rf *.o server
