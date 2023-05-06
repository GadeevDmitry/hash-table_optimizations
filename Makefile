#----------------------------------------------------------------------------------
# CONST
#----------------------------------------------------------------------------------

FLAGS_DEBUG  = -D _DEBUG -ggdb3 -std=c++20 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -pie -Wlarger-than=8192 -Wstack-usage=8192
FLAGS_NDEBUG = -g -std=c++20 -Wall

#----------------------------------------------------------------------------------
# SETTINGS
#----------------------------------------------------------------------------------

OPTIMIZE = -O3
FLAGS    = $(FLAGS_NDEBUG) $(OPTIMIZE)

#----------------------------------------------------------------------------------
# LIB
#----------------------------------------------------------------------------------

TRC	  = lib/logs/trace/trace
LOG   = lib/logs/log
ALG   = lib/algorithm/algorithm
STK   = lib/stack/stack
LST   = lib/list/list
CHAIN = src/chain/chain

TRC_STAT_H   = $(TRC)_static.h
LOG_STAT_H   = $(LOG)_static.h
ALG_STAT_H   = $(ALG)_static.h
STK_STAT_H   = $(STK)_static.h
LST_STAT_H   = $(LST)_static.h
CHAIN_STAT_H = $(CHAIN)_static.h

TRC_H   = $(TRC).h    $(TRC)_settings.h
LOG_H   = $(LOG).h    $(LOG)_settings.h $(LOG)_def.h $(LOG)_undef.h
ALG_H   = $(ALG).h    $(ALG)_settings.h
STK_H   = $(STK).h    $(STK)_settings.h
LST_H   = $(LST).h    $(LST)_settings.h
CHAIN_H = $(CHAIN).h  $(CHAIN)_settings.h

TRC_O   = $(TRC).o
LOG_O   = $(LOG).o
ALG_O   = $(ALG).o
STK_O   = $(STK).o
LST_O   = $(LST).o
CHAIN_O = $(CHAIN).o

$(TRC_O):   $(TRC_H)   $(TRC_STAT_H)   $(LOG_H)
$(LOG_O):   $(LOG_H)   $(LOG_STAT_H)   $(TRC_H)
$(ALG_O):   $(ALG_H)   $(ALG_STAT_H)   $(LOG_H)
$(STK_O):   $(STK_H)   $(STK_STAT_H)   $(LOG_H) $(ALG_H)
$(LST_O):   $(LST_H)   $(LST_STAT_H)   $(LOG_H) $(ALG_H)
$(CHAIN_O): $(CHAIN_H) $(CHAIN_STAT_H) $(LOG_H) $(ALG_H)

LIB_O = $(LOG_O) $(TRC_O) $(ALG_O) $(STK_O) $(LST_O) $(CHAIN_O)
LIB_H = $(LOG_H)          $(ALG_H) $(STK_H) $(LST_H) $(CHAIN_H)

lib: $(LIB_O)

clean_lib:
	rm -f $(CHAIN_O)
	rm -f $(LOG_O)
	rm -f $(ALG_O)
	rm -f $(STK_O)
	rm -f $(LST_O)
	rm -f $(TRC_O)

.PHONY: clean

#----------------------------------------------------------------------------------
# HASH_TABLE
#----------------------------------------------------------------------------------

all: $(HASH_TABLE_O) $(HASH_CALC_O) $(HASH_TYPE_O) $(DIV) $(SEARCH)

OBJ = obj
SRC = src

HASH_DIR   = $(SRC)/hash
HASH_H     = $(HASH_TABLE_H) $(HASH_CALC_H) $(HASH_TYPE_H)
HASH_O     = $(HASH_TABLE_O) $(HASH_CALC_O) $(HASH_TYPE_O)

HASH_TABLE_CPP = $(HASH_DIR)/hash_table.cpp
HASH_TABLE_H   = $(HASH_DIR)/hash_table.h
HASH_TABLE_O   = $(OBJ)/hash_table.o

HASH_CALC_CPP  = $(HASH_DIR)/hash.cpp
HASH_CALC_H    = $(HASH_DIR)/hash.h
HASH_CALC_O    = $(OBJ)/hash.o

HASH_TYPE_CPP  = $(HASH_DIR)/type.cpp
HASH_TYPE_H    = $(HASH_DIR)/type.h
HASH_TYPE_O    = $(OBJ)/type.o

clean_hash:
	rm -r $(OBJ)

#----------------------------------------------------------------------------------

$(HASH_TABLE_O): $(HASH_TABLE_CPP) $(HASH_H)
	mkdir -p $(OBJ)
	g++ -c $< $(FLAGS) -o $@

$(HASH_CALC_O): $(HASH_CALC_CPP) $(HASH_CALC_H) $(TYPE_H)
	mkdir -p $(OBJ)
	g++ -c $< $(FLAGS) -o $@

$(HASH_TYPE_O): $(HASH_TYPE_CPP) $(HASH_TYPE_H)
	mkdir -p $(OBJ)
	g++ -c $< $(FLAGS) -o $@

#----------------------------------------------------------------------------------
# EXE
#----------------------------------------------------------------------------------

HASH_TIME_CPP  = $(HASH_DIR)/hash_time.cpp
HASH_TIME      = hash_time

$(HASH_TIME): $(HASH_TIME_CPP) $(HASH_CALC_O) $(LIB_O)
	g++ $< $(HASH_CALC_O) $(LIB_O) $(FLAGS) -o $@

DIV_CPP = $(SRC)/division.cpp
DIV     = div

$(DIV): $(DIV_CPP) $(HASH_O) $(LIB_O)
	g++ $< $(HASH_O) $(LIB_O) $(FLAGS) -o $@

SEARCH_CPP = $(SRC)/search.cpp
SEARCH     = search

$(SEARCH): $(SEARCH_CPP) $(HASH_O) $(LIB_O)
	g++ $< $(HASH_O) $(LIB_O) $(FLAGS) -o $@


%.o: %.cpp
	g++ -c $(FLAGS) $< -o $@

.PHONY: all