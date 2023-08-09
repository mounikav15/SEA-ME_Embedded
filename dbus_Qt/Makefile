# Makefile for dbussend_test.c

all: dbussend_test

dbussend_test: dbussend_test.c
		gcc -Wall dbussend_test.c -o dbussend_test `pkg-config --cflags dbus-1` `pkg-config --libs dbus-1`

.PHONY: clean
clean:
		rm -f dbussend_test
