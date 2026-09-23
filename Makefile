NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
CPPFLAGS = -Iinclude -Isrc
DEPFLAGS = -MMD -MP

SRC = src/core/main.c \
	src/parser/parse.c \
	src/core/init.c \
	src/core/start.c \
	src/core/cleanup.c \
	src/utils/ft_atoi.c \
	src/utils/ft_strcmp.c \
	src/utils/ft_memset.c \
	src/time/time.c \
	src/log/log.c \
	src/scheduler/heap.c \
	src/scheduler/heap_order.c \
	src/scheduler/scheduler.c \
	src/dongle/dongle.c \
	src/dongle/acquire.c \
	src/threads/coder.c \
	src/threads/phases.c \
	src/threads/monitor.c
OBJ = $(SRC:src/%.c=obj/%.o)
DEP = $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

obj/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

-include $(DEP)

.PHONY: all clean fclean re
