NAME=libmalloc.so
SRCS=src/malloc.c \
	src/my_malloc.c \
	src/my_free.c
OBJS=$(SRCS:.c=.o)

CPPFLAGS=-D_DEFAULT_SOURCE
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla -fvisibility=hidden -fPIC
LDFLAGS=-shared -Wl,--no-undefined

library: $(OBJS)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS)

check:
	LD_LIBRARY_PATH=libmalloc.so ./tests/memoryfootprint 4096; echo $$?;
	LD_LIBRARY_PATH=libmalloc.so ./tests/speed; echo $$?;

clean:
	$(RM) $(OBJS) $(NAME)
