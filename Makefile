
define MSG
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile for ft_ping                               :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By arsciand                                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#                                                      #+#    #+#              #
#                                                     ###   ########.fr        #
#                                                                              #
# **************************************************************************** #
endef

# Progress bar with on line output printing. This script get the maximum
# lines , then expr a compilation percentage.

ifneq ($(words $(MAKECMDGOALS)),1)
.DEFAULT_GOAL		=	make
%:
	@$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST))
else
ifndef ECHO
T					:=	$(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
						-nrRf $(firstword $(MAKEFILE_LIST)) \
						ECHO="OutputPrintable" | grep -c "OutputPrintable")
N					:=	x
C					=	$(words $N)$(eval N := x $N)
V					=	"`expr "   [\`expr $C '*' 100 / $T\`" : '.*\(....\)$$'`%]"
ECHO				=	printf "\e[1A\r%s %s %s                                          \n" $V
endif

# Color

RESET_C				=	\033[m
B_C					=	\033[0;34m
Y_C					=	\033[0;33m
G_C					=	\033[0;32m
R_C					=	\033[0;31m
M_C					=	\033[0;35m
C_C					=	\033[0;36m
F_C					=	\033[35;5;108m

# Programms names

NAME				=	ft_ping
LNAME				=	libft.a

# Build information that can be added the predefines buffer at compilation

BUILD_FILE			=	.build
BUILD_DATE			=	$$(date +'%Y%m%d')
BUILD_BRANCH		=	$$(git symbolic-ref HEAD 2>/dev/null | cut -d"/" -f 3)
BUILD_RELEASE		=	$$(awk 'NR==3 {print $$3}' $(BUILD_FILE))
BUILD_VERSION		=	$$(awk 'NR==4 {print $$3}' $(BUILD_FILE))
BUILD_PATCH			=	$$(awk 'NR==5 {print $$3}' $(BUILD_FILE))
DEFAULT_BUILD_FILE	=	"Build information, patch level is incremented at \
compilation.\n\nRELEASE\t=\t0\nVERSION\t=\t0\nPATCH\t=\t0"

# Dir/Files Path (Do not modify)

S_PATH				=	srcs/
H_PATH				+=	includes/
H_PATH				+=	libft/includes/
B_PATH				=	build/
O_PATH				=	build/objs/
L_PATH				=	libft/

###############################################################################
#                               Modifications                                 #
###############################################################################

# Add custom dir for .o

CORE				=	core/
DB					=	db/
TOOLS				=	tools/
SIGNALS				= 	signals/
DISPLAY				=	display/

# Add previous custom dir with $(O_PATH){custom dir} to PATH varriable

PATHS				+=	$(B_PATH)
PATHS				+=	$(O_PATH)
PATHS				+=	$(O_PATH)$(CORE)
PATHS				+=	$(O_PATH)$(DB)
PATHS				+=	$(O_PATH)$(TOOLS)
PATHS				+=	$(O_PATH)$(DISPLAY)
PATHS				+=	$(O_PATH)$(SIGNALS)

# Files

SRC					+= $(S_PATH)$(CORE)exec_ping.c
SRC					+= $(S_PATH)$(CORE)init.c
SRC					+= $(S_PATH)$(CORE)main.c
SRC					+= $(S_PATH)$(CORE)memory.c
SRC					+= $(S_PATH)$(CORE)opts_args.c
SRC					+= $(S_PATH)$(CORE)resolve.c
SRC					+= $(S_PATH)$(CORE)send_packet.c
SRC					+= $(S_PATH)$(CORE)signals.c
SRC					+= $(S_PATH)$(CORE)socket.c
SRC					+= $(S_PATH)$(DISPLAY)errors.c
SRC					+= $(S_PATH)$(DISPLAY)usage.c
SRC					+= $(S_PATH)$(DISPLAY)debug.c
SRC					+= $(S_PATH)$(TOOLS)time.c

# Headers

HDR					+=	libft.h
HDR					+=	ft_ping.h

# std

# STD					+=	gnu17


###############################################################################
#                                                                             #
###############################################################################

# Objects

OBJ					=	$(patsubst $(S_PATH)%.c, $(O_PATH)%.o, $(SRC))
LIB					=	$(L_PATH)$(LNAME)
vpath %.h $(H_PATH)

# Variables

DEBUG				=
DEBUG_MODE			=
CFLAGS				=	-Wall -Wextra -Werror #-std=$(STD)
ifeq ($(DEBUG), g)
	CFLAGS			=	-g
else ifeq ($(DEBUG), fsanitize)
	CFLAGS			=	-fsanitize=address -g3
else ifeq ($(DEBUG), hard)
	CFLAGS			+=	-Weverything -fsanitize=address,undefined -DDEBUG
else ifeq ($(DEBUG), dev)
	CFLAGS			=
endif
CC					=	clang $(CFLAGS)
IFLAGS				+=	$(addprefix -I, $(H_PATH))
CMPLC				=	$(CC) -c $(IFLAGS)
CMPLO				=	$(CC) -o
BUILD				=	$(PATHS)
AR_RC				=	ar rc
RANLI				=	ranlib
RM_RF				=	/bin/rm -rf
MKDIR				=	mkdir -p
NORME				=	norminette
SLEEP				=	sleep 0.01
GCFIL				=	"- > Compiling	-"
RMSHW				=	"- - Removing	-"
MKSHW				=	"- + Creating	-"
GCSUC				=	echo "$(G_C)=====>     SUCCESS$(RESET_C)"
CLSUC				=	echo "$(R_C)=====>     DONE$(RESET_C)"
NORMD				=	echo "$(G_C)=====>     DONE$(RESET_C)"

.PHONY: all norme clean fclean re test

# Rules
make:
	echo "# **************************************************************************** #"
	echo "#                                                                              #"
	echo "#                                                         :::      ::::::::    #"
	echo "#    Makefile for ft_ping                               :+:      :+:    :+:    #"
	echo "#                                                     +:+ +:+         +:+      #"
	echo "#    By arsciand                                    +#+  +:+       +#+         #"
	echo "#                                                 +#+#+#+#+#+   +#+            #"
	echo "#                                                      #+#    #+#              #"
	echo "#                                                     ###   ########.fr        #"
	echo "#                                                                              #"
	echo "# **************************************************************************** #"
	$(MAKE) --no-print-directory all

all: libm $(BUILD) $(NAME)

# Compilation core

ifneq ("$(wildcard $(L_PATH)$(LNAME))","")
TEST				=	libft/libft.a
else
TEST				=
endif

$(NAME): $(OBJ) $(BUILD_FILE) $(TEST)
	$(ECHO) $(GCFIL) $(NAME)
	$(CMPLO) $(NAME) $(OBJ) $(LIB)
	$(GCSUC)
	echo "---\nCFLAGS - =$(B_C) $(CFLAGS)$(RESET_C)\n---"
	echo "\n$(G_C)[$(BUILD_BRANCH)] $(RESET_C)$@ $(F_C) \
	v.$(BUILD_RELEASE)_$(BUILD_VERSION)_$(BUILD_PATCH)_$(BUILD_DATE) $(RESET_C) is ready !"
	cp $(NAME) \
	$(B_PATH)$(NAME)_$(BUILD_RELEASE)_$(BUILD_VERSION)_$(BUILD_PATCH + 1)_$(BUILD_DATE)

$(OBJ): $(O_PATH)%.o: $(S_PATH)%.c $(HDR)
	$(CMPLC) -DBUILDR=$(BUILD_RELEASE) -DBUILDV=$(BUILD_VERSION) \
	-DBUILDP=$$(echo "$(BUILD_PATCH) + 1" | bc) -DDATE=$(BUILD_DATE) $< -o $@
	$(ECHO) $(GCFIL) $<

# Check if .build exist, then incremente patch level each compilation.
# If not exist, create it with default values

UNAME_S				:=	$(shell uname -s)

ifeq ($(UNAME_S), Darwin)
$(BUILD_FILE): $(OBJ)
	if ! test -f $(BUILD_FILE); \
	then echo $(DEFAULT_BUILD_FILE) > $(BUILD_FILE); fi
	sed -i '.bak' "5s/$(BUILD_PATCH)/$$(echo $$(($(BUILD_PATCH) + 1)))/g" \
	$(BUILD_FILE)
	rm $(BUILD_FILE).bak
else
$(BUILD_FILE): $(OBJ)
	if ! test -f $(BUILD_FILE); \
	then echo $(DEFAULT_BUILD_FILE) > $(BUILD_FILE); fi
	sed -i "5s/$(BUILD_PATCH)/$$(echo $$(($(BUILD_PATCH) + 1)))/g" \
	$(BUILD_FILE)
endif

$(PATHS):
	$(MKDIR) $(PATHS)
	$(foreach var,$(PATHS), $(ECHO) $(MKSHW) $(var);)

norme:
	$(NORMR)
	$(NORME) $(SRC) $(H_PATH)$(HNAME)
	$(NORMD)

clean: libco
	for i in $(OBJ); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	$(CLSUC)

fclean: libc
	for i in $(OBJ); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	for i in $(PATHS); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	$(RM_RF) $(NAME)
	$(ECHO) $(RMSHW) $(NAME)
	$(CLSUC)

# Libc rules

libm:
	make -C $(L_PATH)
	printf "\n"

libco:
	make clean -C $(L_PATH)

libc:
	make fclean -C $(L_PATH)

help:
	echo "Makefile for ft_ping"
	echo "usage : make [VERBOSE=1] [DEBUG=g|fsanitize|hard|dev] [all|clean|fclean|re|libm|libco|libc|help]"

re:
	$(MAKE) --no-print-directory fclean all

ifndef VERBOSE
.SILENT:
endif

endif
