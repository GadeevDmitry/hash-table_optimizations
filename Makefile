CFLAGS = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -pie -Wlarger-than=8192 -Wstack-usage=8192
#----------------------------------------------------------------------------------

TRC	    = lib/logs/trace/trace
LOG     = lib/logs/log
ALG     = lib/algorithm/algorithm
STK     = lib/stack/stack
LST     = lib/list/list

TRC_STAT_H   = $(TRC)_static.h
LOG_STAT_H   = $(LOG)_static.h
ALG_STAT_H   = $(ALG)_static.h
STK_STAT_H   = $(STK)_static.h
LST_STAT_H   = $(LST)_static.h

TRC_H   = $(TRC).h $(TRC)_settings.h
LOG_H   = $(LOG).h $(LOG)_settings.h $(LOG)_def.h $(LOG)_undef.h
ALG_H   = $(ALG).h $(ALG)_settings.h
STK_H   = $(STK).h $(STK)_settings.h
LST_H   = $(LST).h $(LST)_settings.h

LOG_O   = $(LOG).o
TRC_O   = $(TRC).o
ALG_O   = $(ALG).o
STK_O   = $(STK).o
LST_O   = $(LST).o

LIB_O   = $(LOG_O) $(TRC_O) $(ALG_O) $(STK_O) $(LST_O)

$(TRC_O): $(TRC_H) $(TRC_STAT_H) $(LOG_H)
$(LOG_O): $(LOG_H) $(LOG_STAT_H) $(TRC_H)
$(ALG_O): $(ALG_H) $(ALG_STAT_H) $(LOG_H)
$(STK_O): $(STK_H) $(STK_STAT_H) $(LOG_H) $(ALG_H)
$(LST_O): $(LST_H) $(LST_STAT_H) $(LOG_H) $(ALG_H)

lib: $(LIB_O)

clean:
	rm -f $(LOG_O)
	rm -f $(ALG_O)
	rm -f $(STK_O)
	rm -f $(LST_O)
	rm -f $(TRC_O)

.PHONY: clean

#----------------------------------------------------------------------------------

OBJ = obj
SRC = src

hash_table.o: $(SRC)/hash_table.cpp $(SRC)/hash_table.h $(SRC)/type.h
	mkdir -p $(OBJ)
	g++ -c $(CFLAGS) $< -o $(OBJ)/$@

#----------------------------------------------------------------------------------

%.o: %.cpp
	g++ -c $(CFLAGS) $< -o $@
