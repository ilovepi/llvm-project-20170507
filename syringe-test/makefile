PREFIX=~/clang-dev/bin
CC=$(PREFIX)/clang
CXX=$(PREFIX)/clang++
OPT=$(PREFIX)/opt
DIS=$(PREFIX)/llvm-dis
SRC=syringe.cpp
EMIT_BC=-emit-llvm $(OPTIMIZE) -c
EMIT_LL=-S -emit-llvm $(OPTIMIZE)
EMIT_ASM=-S $(OPTIMIZE)
TARGETS=syringe.bc syringe.s syringe_test syringe_test_opt syringe_opt.bc *.o *.ll *.a
FLAGS=-fsyringe $(OPTIMIZE) $(DEBUG) $(VERBOSE) 
STD=-stdlib=libc++
#OPTIMIZE=-O0
#VERBOSE=-v
DEBUG=-g
LIBS= -lc++abi -I $(HOME)/clang-dev/include/c++/v1  -L $(HOME)/clang-dev/lib
export LD_LIBRARY_PATH=$(HOME)/clang-dev/lib

.PHONEY: $(TARGETS) test clean opt

all: syringe.ll

injection_decl.o: injection_decl.cpp
	$(CXX) -c injection_decl.cpp $(FLAGS) $(STD)

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

syringe_opt.bc: syringe.bc
	$(OPT) syringe.bc -syringe -o syringe_opt.bc

syringe_opt.ll: syringe_opt.bc
	$(DIS) syringe_opt.bc

syringe_test_opt: main.o syringe_opt.o injection_decl.o
	$(CXX) main.o syringe_opt.o injection_decl.o  -o syringe_test_opt $(FLAGS) $(LIBS) $(STD)

syringe_opt.o: syringe_opt.bc
	$(CXX) syringe_opt.bc -c $(FLAGS)

main.o: main.cpp
	$(CXX) main.cpp -c $(FLAGS) $(STD)

clean:
	rm -f $(TARGETS)
