PREFIX=~/clang-dev/bin
CC=$(PREFIX)/clang
CXX=$(PREFIX)/clang++
OPT=$(PREFIX)/opt
DIS=$(PREFIX)/llvm-dis
SRC=syringe.cpp
EMIT_BC=-emit-llvm $(OPTIMIZE) -c
EMIT_LL=-S -emit-llvm $(OPTIMIZE)
EMIT_ASM=-S $(OPTIMIZE)
TARGETS=syringe.bc syringe.s syringe_test syringe_test_opt *.bc *.o *.ll *.a
FLAGS=-fsyringe -fsyringe-config-file="syringe.yml" -fuse-ld=lld $(OPTIMIZE) $(DEBUG) $(VERBOSE)
#OPTIMIZE=-O0
VERBOSE=-v
DEBUG=-g
LIBS= -lc++abi -I $(HOME)/clang-dev/include/c++/v1  -L $(HOME)/clang-dev/lib
STD=-stdlib=libc++
export LD_LIBRARY_PATH=$(HOME)/clang-dev/lib

.PHONEY: $(TARGETS) test clean opt

all: syringe.ll

injection_decl.o: injection_decl.c
	$(CC) -c injection_decl.c $(FLAGS) $(STD)

%.o: %.c
	$(CC) $< -c $(FLAGS) $(STD)

%.ll: %.c
	$(CC) $< $(EMIT_LL) $(FLAGS) $(STD)

%.bc: %.c
	$(CC) $< $(EMIT_BC) $(FLAGS) $(STD)

%.s: %.c
	$(CC) $(SRC) $(EMIT_ASM) $(FLAGS) $(STD)

test: syringe_test
	./syringe_test

opt-test: syringe_test_opt
	./syringe_test_opt

syringe_test_opt: main.o syringe.o injection_decl.o
	$(CXX) main.o syringe.o injection_decl.o  -o syringe_test_opt $(FLAGS) $(LIBS) $(STD)

main.o: main.c
	$(CC) main.c -c $(FLAGS) $(STD)

clean:
	rm -f $(TARGETS)
