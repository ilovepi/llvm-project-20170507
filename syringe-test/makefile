PREFIX=$(HOME)/clang-dev/bin
CC=$(PREFIX)/clang
CXX=$(PREFIX)/clang++
OPT=$(PREFIX)/opt
DIS=$(PREFIX)/llvm-dis
SRC=syringe.cpp
EMIT_BC=-emit-llvm $(OPTIMIZE) -c
EMIT_LL=-S -emit-llvm $(OPTIMIZE)
EMIT_ASM=-S $(OPTIMIZE)
TARGETS=syringe.bc syringe.s syringe_test syringe_test_opt *.bc *.o *.ll *.a
FLAGS=-fsyringe $(OPTIMIZE) $(DEBUG) $(VERBOSE)
#OPTIMIZE=-O0
#VERBOSE=-v
DEBUG=-g
LIBS=-fuse-ld=$(PREFIX)/ld.lld
#LIBS=-lc++abi -L $(HOME)/clang-dev/lib -fuse-ld=$(PREFIX)/ld.lld
#STD=-stdlib=libc++ -nostdinc++ -I $(HOME)/clang-dev/include/c++/v1
#export LD_LIBRARY_PATH=$(HOME)/clang-dev/lib

.PHONEY: $(TARGETS) test clean opt

all: syringe.ll

%.o: %.cpp
	$(CXX) $< -c $(FLAGS) $(STD)

%.ll: %.cpp
	$(CXX) $< $(EMIT_LL) $(FLAGS) $(STD)

%.bc: %.cpp
	$(CXX) $< $(EMIT_BC) $(FLAGS) $(STD)

%.s: %.cpp
	$(CXX) $(SRC) $(EMIT_ASM) $(FLAGS) $(STD)

test: syringe_test
	./syringe_test

opt-test: syringe_test_opt
	./syringe_test_opt

debug: syringe_test_opt
	gdb ./syringe_test_opt

syringe_test_opt: main.o syringe.o injection_decl.o
	$(CXX) main.o syringe.o injection_decl.o  -o syringe_test_opt $(FLAGS) $(LIBS) $(STD)

clean:
	rm -f $(TARGETS)
