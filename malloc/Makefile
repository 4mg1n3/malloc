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
debug: CFLAGS += -g -O0
debug: clean library
	@echo "Built with debug symbols"

memcheck: debug
	@echo "Running memory leak check with Valgrind..."
	@echo "=== Testing test_basic ==="
	LD_PRELOAD=./$(NAME) valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/test_basic
	@echo ""
	@echo "=== Testing test_extensive ==="
	LD_PRELOAD=./$(NAME) valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tests/test_extensive

memcheck-quick: debug
	@echo "Running quick memory check..."
	LD_PRELOAD=./$(NAME) valgrind --leak-check=yes ./tests/test_basic
