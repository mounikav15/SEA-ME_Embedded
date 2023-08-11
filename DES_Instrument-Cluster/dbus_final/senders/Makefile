# Makefile for speedstatus.c

all: speedstatus

speedstatus: speedstatus.c
		gcc -Wall speedstatus.c -o speedstatus `pkg-config --cflags dbus-1` `pkg-config --libs dbus-1`

.PHONY: clean
clean:
		rm -f speedstatus
