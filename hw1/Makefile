LLVM_CONFIG=/advanced-compiler/llvm_build/bin/llvm-config

CXX=`$(LLVM_CONFIG) --bindir`/clang
CXXFLAGS=`$(LLVM_CONFIG) --cppflags` -fPIC -fno-rtti
LDFLAGS=`$(LLVM_CONFIG) --ldflags`

all: Hw1.so Test

Hw1.so: Hw1.o
	$(CXX) -shared Hw1.o $(LDFLAGS) -fPIC -o Hw1.so

Hw1.o: Hw1.cpp
	$(CXX) -c Hw1.cpp $(CXXFLAGS) -o Hw1.o

Test: test1.c test2.c test3.c
	$(CXX) -S -emit-llvm test1.c -o test1.ll
	$(CXX) -S -emit-llvm test2.c -o test2.ll
	$(CXX) -S -emit-llvm test3.c -o test3.ll

clean:
	rm -f *.o *.ll *.so
