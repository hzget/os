
CFLAGS += -I../

all: clean string

string:
	gcc -c -fPIC $(CFLAGS) ../string.c -o string.o -fno-builtin
	gcc -shared -Wl,-soname,libstring.so -o libstring.so string.o
	gcc -o string_test string_test.c -L. -Wl,-R. -lstring -lcmocka -DUNIT_TESTING=1 -fno-builtin
	./string_test

clean:
	rm *_test *.o *.so -rf
