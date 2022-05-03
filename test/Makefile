CC := g++
CFLAG := -std=c++11
CFLAG_SO := 
CASE_DIR := $(shell ls -d case*)
HASH_MAP_DIR = ../
CASE_INCLUDE := -I$(BOOST_ROOT) -I$(HASH_MAP_DIR)
MAIN_INCLUDE := $(CASE_INCLUDE) $(foreach dir, $(CASE_DIR), -I$(dir))
CASE_SRC := $(foreach dir, $(CASE_DIR), $(wildcard $(dir)/*.cpp)) 
CASE_OBJ := $(patsubst %.cpp, %.o, $(CASE_SRC))

ifdef DEBUG
CFLAG += -g
endif

ifdef COVER
CFLAG += --coverage 
endif
CFLAG_CASE = $(CFLAG) -fPIC 

main: main.cpp $(CASE_OBJ)
	$(CC) $(CFLAG) -o $@ $^ $(MAIN_INCLUDE)

$(CASE_OBJ) : %.o : %.cpp $(HASH_MAP_DIR)/*.*
	$(CC) $(CFLAG_CASE) -c -o $@ $< $(CASE_INCLUDE)


.PHONY : clean
clean:
	rm -rf case*/*.o
