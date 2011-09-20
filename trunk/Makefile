CXX=g++
OPTIONS=-Wall -pedantic -O3 -std=c++0x
INCLUDE=-I./include/
LIBRARY=
BINARIES=scompressor

scompressor_SRC=./src/scompressor.cpp

make: $(BINARIES) 
#documentation

scompressor: $($@_SRC)
	$(CXX) -o $@ $($@_SRC) $(INCLUDE) $(LIBRARY) $(OPTIONS)

documentation: ./doc/Doxyfile
	doxygen ./doc/Doxyfile
	make -C doc/latex -f Makefile

clean:
	rm -rf *~ include/*~ src/*~ doc/*~ 

distclean: clean
	rm -rf doc/html doc/latex $(BINARIES)
