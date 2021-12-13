CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic -g $(shell pkg-config --cflags gmp)
LDFLAGS = $(shell pkg-config --libs gmp)
COMMON_OBJS = rsa.o numtheory.o randstate.o

all: keygen encrypt decrypt

keygen: keygen.o $(COMMON_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

encrypt: encrypt.o $(COMMON_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

decrypt: decrypt.o $(COMMON_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f keygen encrypt decrypt *.o

cleankeys:
	rm -f *.priv *.pub

format: 
	clang-format -i -style=file *.[c,h]

scan-build: clean
	scan -build make