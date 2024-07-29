NAME			= ircserv
NAME_BONUS		= ircserv_bonus

CC				= c++
FLAGS			= -Wall -Wextra -Werror -std=c++98
INC				= -IIncl
RM				= rm -rf

SRC_DIR			= Mandatory/Srcs/
INC_DIR   		= Mandatory/Incl/
OBJDIR      	= .obj

SRC_DIR_BONUS 	= Bonus/Srcs/
INC_DIR_BONUS 	= Bonus/Incl/
OBJDIR_BONUS    = .bonusObj

# Colors
GREEN       = \e[92;5;118m
RESET       = \e[0m
YELLOW      = \e[93m

CPP_FILES   = main Server Channel Network User Parse
HPP_FILES   = Server Channel Network User Parse

CPP_FILES_BONUS   = main Server Channel Network User Parse Bot
HPP_FILES_BONUS   = Server Channel Network User Parse Bot

SRC         = $(addprefix $(SRC_DIR), $(CPP_FILES:=.cpp))
OBJ         = $(addprefix $(OBJDIR)/, $(CPP_FILES:=.o))
HEADER      = $(addprefix $(INC_DIR), $(HPP_FILES:=.hpp))

SRC_BONUS         = $(addprefix $(SRC_DIR_BONUS), $(CPP_FILES_BONUS:=.cpp))
OBJ_BONUS         = $(addprefix $(OBJDIR_BONUS)/, $(CPP_FILES_BONUS:=.o))
HEADER_BONUS      = $(addprefix $(INC_DIR_BONUS), $(HPP_FILES_BONUS:=.hpp))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(OBJ) $(FLAGS) -o $(NAME)
	@echo "\n$(GREEN)Build complete: $(NAME)$(RESET)"

$(OBJDIR)/%.o: $(SRC_DIR)%.cpp $(HEADER)
	@mkdir -p $(OBJDIR)
	@$(CC) $(INC) $(FLAGS) -c $< -o $@
	@echo -n "$(GREEN)█$(RESET)"

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(OBJ_BONUS)
	@$(CC) $(OBJ_BONUS) $(FLAGS) -o $(NAME_BONUS)
	@echo "\n$(GREEN)Build complete: $(NAME_BONUS)$(RESET)"

$(OBJDIR_BONUS)/%.o: $(SRC_DIR_BONUS)%.cpp $(HEADER_BONUS)
	@mkdir -p $(OBJDIR_BONUS)
	@$(CC) $(INC) $(FLAGS) -c $< -o $@
	@echo -n "$(GREEN)█$(RESET)"

clean:
	@if [ -d "$(OBJDIR_BONUS)" ]; then \
		$(RM) $(OBJDIR_BONUS); \
		echo "$(YELLOW)$(NAME_BONUS) object files removed$(RESET)"; \
	fi
	@if [ -d "$(OBJDIR)" ]; then \
		$(RM) $(OBJDIR); \
		echo "$(YELLOW)$(NAME) object files removed$(RESET)"; \
	fi

fclean: clean
	@if [ -f "$(NAME_BONUS)" ]; then \
		$(RM) $(NAME_BONUS); \
		echo "$(YELLOW)$(NAME_BONUS) removed$(RESET)"; \
	fi
	@if [ -f "$(NAME)" ]; then \
		$(RM) $(NAME); \
		echo "$(YELLOW)$(NAME) removed$(RESET)"; \
	fi

re: fclean all

run: all
	@./$(NAME) 8080 l

run_bonus: bonus
	@./$(NAME_BONUS) 8080 l

.PHONY: all clean fclean re bonus
