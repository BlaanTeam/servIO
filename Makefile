NAME = servio


CC = c++

CFLAGS = -Wall -Wextra -Werror -ggdb -std=c++98


# core files
CORE_SRC = servio.cpp\
	sio_config.cpp\
	sio_cmdline_opts.cpp\
	sio_lexer.cpp\
	sio_ast.cpp\
	sio_parser.cpp\
	sio_utils.cpp\

# core headers
CORE_HEADER = servio.hpp\
	sio_config.hpp\
	sio_cmdline_opts.hpp\
	sio_lexer.hpp\
	sio_ast.hpp\
	sio_parser.hpp\
	sio_utils.hpp

# http files
HTTP_SRC = sio_request.cpp\
	sio_http_codes.cpp\
	sio_client.cpp\

# http headers
HTTP_HEADER = barrel.hpp\
	sio_request.hpp\
	sio_http_codes.hpp\
	sio_client.hpp\

# utility files
UTILITY_SRC = sio_socket.cpp\
	sio_helpers.cpp\

# utility headers
UTILITY_HEADER = barrel.hpp\
	sio_socket.hpp\
	sio_helpers.hpp\

SRCS = sio_main.cpp\
	$(addprefix http/,$(HTTP_SRC))\
	$(addprefix core/, $(CORE_SRC))\
	$(addprefix utility/, $(UTILITY_SRC))

SRCS := $(addprefix src/, $(SRCS))

HEADERS = $(addprefix core/, $(CORE_HEADER))\
	$(addprefix  utility/, $(UTILITY_HEADER))\
	$(addprefix http/, $(HTTP_HEADER))

OBJS = $(SRCS:%.cpp=%.o)

INCLUDES = core http utility

INCLUDES := -I src/\
	$(addprefix -I src/, $(INCLUDES))


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $^ -fsanitize=address

# !TOADD: depend on HEADERS macro
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -vf $(OBJS)

fclean: clean
	rm -vf $(NAME)

re: fclean all