PREFIX=~/clang-dev/bin
CC=$(PREFIX)/clang
CXX=$(PREFIX)/clang++
OPT=$(PREFIX)/opt
DIS=$(PREFIX)/llvm-dis
SRC=syringe.cpp
EMIT_BC=-emit-llvm -O0 -c
EMIT_LL=-S -emit-llvm -O1
EMIT_ASM=-S -O0
TARGETS=syringe.bc syringe.s syringe_test syringe_test_opt syringe_opt.bc *.o *.ll *.a
FLAGS=-fsyringe $(VERBOSE) $(DEBUG)
#VERBOSE=-v
#DEBUG=-g

.PHONEY: $(TARGETS) test clean opt

all: syringe.ll

syringe_rt.o: syringe_rt.cpp
	$(CXX) syringe_rt.cpp -c

libsyringe_rt.a: syringe_rt.o
	ar rcs libsyringe_rt.a syringe_rt.o

libs: libsyringe_rt.a

injection_decl.o: injection_decl.cpp
	$(CXX) -c injection_decl.cpp $(FLAGS)

%.ll: %.cpp
	$(CXX) $< $(EMIT_LL) $(FLAGS)

%.bc: %.cpp
	$(CXX) $< $(EMIT_BC) $(FLAGS)

%.s: %.cpp
	$(CXX) $(SRC) $(EMIT_ASM) $(FLAGS)

test: syringe_test
	./syringe_test

opt-test: syringe_test_opt
	./syringe_test_opt

syringe_opt.bc: syringe.bc
	$(OPT) syringe.bc -syringe -o syringe_opt.bc

syringe_opt.ll: syringe_opt.bc
	$(DIS) syringe_opt.bc

#syringe_test_opt: main.o syringe_opt.o injection_decl.o libs
#$(CXX) main.o syringe_opt.o injection_decl.o -L . -lsyringe_rt -o syringe_test_opt $(FLAGS)

syringe_test_opt: main.o syringe_opt.o injection_decl.o
	$(CXX) main.o syringe_opt.o injection_decl.o  -o syringe_test_opt $(FLAGS)


syringe_opt.o: syringe_opt.bc
	$(CXX) syringe_opt.bc -c $(FLAGS)

main.o: main.cpp
	$(CXX) main.cpp -c $(FLAGS)

clean:
	rm -f $(TARGETS)
