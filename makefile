# Constants
NAME			= not-apache
NAME			:= $(addprefix ./build/,$(NAME))

# Compiler
CXX				= clang++

CXX_FLAGS		= -Wall -Werror -Wextra -std=c++98 -pedantic-errors
DEBUG_FLAGS		=
BUILD_FLAGS		= -O3 -pthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ifeq ($(CXX),g++)
		CXX_FLAGS += -fPIE
	endif
endif

ifdef BUILD_DEBUG
DEBUG_FLAGS		= -g -fsanitize=address -DDEBUG_THROW=1
BUILD_FLAGS		= -pthread
endif
ifdef BUILD_LEAK
DEBUG_FLAGS		= -g -fsanitize=leak -DDEBUG_THROW=1
BUILD_FLAGS		= -pthread
endif

# Files
SRC_DIR			= src
BUILD_DIR		= build
OUT_DIR			= build/out
INC_DIR			= src/includes

SRC	=\
	main.cpp\
	log/Loggable.cpp\
	log/Logger.cpp\
	log/LogItem.cpp\
	regex/Regex.cpp\
	config/ConfigValidatorBuilder.cpp\
	config/ConfigParser.cpp\
	config/ConfigLine.cpp\
	config/ConfigException.cpp\
	config/AConfigValidator.cpp\
	config/AConfigBlock.cpp\
	config/blocks/RootBlock.cpp\
	config/blocks/RouteBlock.cpp\
	config/blocks/ServerBlock.cpp\
	config/validators/ArgumentLength.cpp\
	config/validators/RequiredKey.cpp\
	config/validators/IntValidator.cpp\
	config/validators/IsDirectory.cpp\
    config/validators/IsFile.cpp\
    config/validators/HTTPMethodValidator.cpp\
    config/validators/IpValidator.cpp\
	config/validators/MutuallyExclusive.cpp\
	config/validators/BooleanValidator.cpp\
	config/validators/Unique.cpp\
	env/env.cpp\
	env/ENVBuilder.cpp\
	utils/AThread.cpp\
	utils/intToString.cpp\
	utils/isSpace.cpp\
	utils/mutex.cpp\
	utils/split.cpp\
	utils/atoi.cpp\
	utils/stoi.cpp\
	utils/toUpper.cpp\
	utils/stoh.cpp\
	utils/isDigit.cpp\
	utils/countSpaces.cpp\
	utils/strdup.cpp\
	utils/DataList.cpp\
    utils/Uri.cpp\
    utils/ArgParser.cpp\
	server/handlers/AHandler.cpp\
	server/handlers/StandardHandler.cpp\
	server/handlers/HandlerHolder.cpp\
	server/handlers/ThreadHandler.cpp\
	server/communication/TCPListener.cpp\
	server/communication/ServerEventBus.cpp\
	server/http/HTTPClient.cpp\
	server/http/HTTPClientData.cpp\
	server/http/HTTPParser.cpp\
	server/http/HTTPParseData.cpp\
	server/http/HTTPResponder.cpp\
	server/http/HTTPMimeTypes.cpp\
	server/http/RequestBuilder.cpp\
	server/http/ResponseBuilder.cpp\
	server/terminal/TerminalClient.cpp\
	server/terminal/TerminalResponder.cpp\
	server/globals.cpp\
	server/Server.cpp

HEADERS	=\
	log/Loggable.hpp\
	log/Logger.hpp\
	log/LogItem.hpp\
	regex/Regex.hpp\
	config/ConfigValidatorBuilder.hpp\
  config/ConfigParser.hpp\
  config/ConfigLine.hpp\
  config/ConfigException.hpp\
  config/AConfigValidator.hpp\
  config/AConfigBlock.hpp\
  config/blocks/RootBlock.hpp\
  config/blocks/RouteBlock.hpp\
  config/blocks/ServerBlock.hpp\
  config/validators/ArgumentLength.hpp\
  config/validators/RequiredKey.hpp\
  config/validators/IntValidator.hpp\
  config/validators/MutuallyExclusive.hpp\
  config/validators/BooleanValidator.hpp\
  config/validators/IsDirectory.hpp\
  config/validators/IsFile.hpp\
  config/validators/HTTPMethodValidator.hpp\
  config/validators/IpValidator.hpp\
  config/validators/Unique.hpp\
  config/ParseExceptions.hpp\
  env/env.hpp\
	env/ENVBuilder.hpp\
  utils/AThread.hpp\
	utils/split.hpp\
	utils/atoi.hpp\
	utils/toUpper.hpp\
	utils/stoi.hpp\
	utils/stoh.hpp\
	utils/isDigit.hpp\
	utils/countSpaces.hpp\
  utils/CreateMap.hpp\
  utils/CreateVector.hpp\
  utils/ErrorThrow.hpp\
  utils/intToString.hpp\
	utils/isSpace.hpp\
	utils/localTime.hpp\
	utils/mutex.hpp\
	utils/strdup.hpp\
	utils/DataList.hpp\
	utils/Uri.hpp\
	utils/ArgParser.hpp\
	server/handlers/AHandler.hpp\
  server/handlers/StandardHandler.hpp\
  server/handlers/HandlerHolder.hpp\
  server/handlers/ThreadHandler.hpp\
  server/communication/TCPListener.hpp\
  server/communication/ServerEventBus.hpp\
  server/http/HTTPClient.hpp\
  server/http/HTTPClientData.hpp\
  server/http/HTTPParser.hpp\
  server/http/HTTPParseData.hpp\
  server/http/HTTPResponder.hpp\
  server/http/HTTPMimeTypes.hpp\
  server/http/RequestBuilder.hpp\
  server/http/ResponseBuilder.hpp\
  server/terminal/TerminalClient.hpp\
  server/terminal/TerminalResponder.hpp\
  server/global/GlobalConfig.hpp\
  server/global/GlobalLogger.hpp\
  server/ServerTypes.hpp\
  server/Server.hpp

# Fix sources and headers
OBJ				= $(patsubst %.cpp,%.o,$(SRC))
HEADERS			:= $(addprefix $(INC_DIR)/,$(HEADERS))

# Colours
DARK_GREEN		= \033[0;32m
GREEN			= \033[0;92m
END				= \033[0;0m

PREFIX			= $(DARK_GREEN)$(notdir $(NAME)) $(END)>>

# Rules
.PHONY: all clean fclean re exec valgrind debug rdebug leak rleak

all: $(NAME)

deps:
	@echo "$(SRC)"

$(NAME): $(addprefix $(OUT_DIR)/,$(OBJ))
	@echo "$(PREFIX)$(GREEN) Bundling objects...$(END)"
	@echo "BUILD $(NAME) $(CXX_FLAGS) $(DEBUG_FLAGS) $(BUILD_FLAGS)"
	@$(CXX) $(CXX_FLAGS) $(DEBUG_FLAGS) $(BUILD_FLAGS) -I$(INC_DIR) -o $@ $(addprefix $(OUT_DIR)/,$(OBJ))

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "$(PREFIX)$(GREEN) Compiling file $(END)$(notdir $<) $(GREEN)to $(END)$(notdir $@)"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXX_FLAGS) $(DEBUG_FLAGS) -I$(INC_DIR) -o $@ -c $<

clean:
	@echo "$(PREFIX)$(GREEN) Removing directory $(END)$(OUT_DIR)"
	@rm -rf $(OUT_DIR)

fclean: clean
	@echo "$(PREFIX)$(GREEN) Removing file $(END)$(NAME)"
	@rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

exec:
	$(MAKE) all
	./$(NAME)

valgrind:
	cd ./build; valgrind --undef-value-errors=no --leak-check=full ../$(NAME) -f ../tests/parser/parser.conf -c; cd ../

debug:
	$(MAKE) BUILD_DEBUG=1 all

rdebug:
	$(MAKE) fclean
	$(MAKE) debug

leak:
	$(MAKE) BUILD_LEAK=1 all

rleak:
	$(MAKE) fclean
	$(MAKE) leak