# Makefile for speedstatus.c

all: speedstatus_execution

speedstatus_execution: speedstatus.c
		gcc -Wall speedstatus.c -o speedstatus_execution `pkg-config --cflags dbus-1` `pkg-config --libs dbus-1` -lpthread

.PHONY: clean
clean:
		rm -f speedstatus_execution
