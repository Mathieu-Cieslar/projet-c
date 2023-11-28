COBJS ?= client.o bmp.o couleur.o json.o operations.o
SOBJS ?= serveur.o json.o operations.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJS)
	gcc -o $(SERVER) $(SOBJS) -lm

$(CLIENT): $(COBJS)
	gcc -o $(CLIENT) $(COBJS)

.c.o: 
	gcc -Wall -Werror -ggdb3 -c $*.c

