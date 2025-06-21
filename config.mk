# debug
DEBUG = -g

# flags
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -pedantic -Wall -Os ${CPPFLAGS} ${DEBUG}

# compiler
CC = cc
