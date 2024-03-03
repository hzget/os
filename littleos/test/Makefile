CFLAGS = -std=gnu99 -O0 -Wall -Wextra -Werror -fno-builtin
CFLAGS += -pedantic -Wconversion
CFLAGS += -I../
CFLAGS += -DUNIT_TESTING=1

LDFLAGS = -L. -Wl,-R. -lcmocka

all: clean string

string:
	gcc -c -fPIC $(CFLAGS) ../string.c -o string.o
	gcc -shared -Wl,-soname,libstring.so -o libstring.so string.o
	gcc -o string_test string_test.c $(CFLAGS) $(LDFLAGS) -lstring
	./string_test

clean:
	rm *_test *.o *.so -rf
