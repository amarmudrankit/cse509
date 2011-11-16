CC	= gcc
CFLAGS	= -ggdb -Wall -D_GNU_SOURCE

LIB	= libextlib.so.1
LIB_FLAGS = -fPIC -shared -lstdc++

RM	= rm
ECHO	= echo

SRCS	= main.c syscall_handler.c
OBJS	= ${SRCS:.c=.o} ${COM_SRCS:.c=.o} ../file_manager/file_manager.o

.SUFFIXES:
.SUFFIXES: .o .c

all: $(OBJS) $(HEADERS)
	g++ -c ../file_manager/file_manager.cpp
	$(CC) $(CFLAGS) $(LIB_FLAGS) $(OBJS) -o $(LIB)

.c.o :
	$(OLIB) $(CC) $(CFLAGS) -c $<

clean :
	-$(RM) -f $(OBJS)
	-$(RM) -f $(LIB)

