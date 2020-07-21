#client:client.c
#	gcc client.c cJSON.c -o client -lssl -lcurl -lcrypto -lm

client:client.o cJSON.o main.o
	gcc client.o cJSON.o main.o -o client -lssl -lcurl -lcrypto -lm

main.o:main.c
	gcc -c main.c -o main.o

client.o:client.c
	gcc -c client.c -o client.o

cJSON.o:cJSON.c
	gcc -c cJSON.c -o cJSON.o


clean:
	rm -rf *.o client
	
