
CC = icc
CFLAGS = -Wall -m64 -mavx -O3 
CXX = icpc
CXXFLAGS = -Wall -m64 -mavx -O3 -openmp 
CUDAHOME = /usr/local/encap/cuda-5.0
CUDACC = $(CUDAHOME)/bin/nvcc
CUDACCFLAGS = -m64 -arch sm_21
CUDALIBS = -Bdynamic -Wl,-rpath,$(CUDAHOME)/lib64 -L$(CUDAHOME)/lib64 -lcudart 
ISPC = ispc
ISPCFLAGS8 = -O2 --arch=x86-64 --target=avx1-i32x8
ISPCFLAGS16 = -O2 --arch=x86-64 --target=avx1-i32x16

OBJS = kernelispc_ispc.o testHarness.o WKFUtils.o cudaFloat.o
DEPS = kernelispc_ispc.h  # automatically generated below

.SUFFIXES: .C .c .cu ..c .i .o .ispc 

.cu.o:
	$(CUDACC) $(CUDACCFLAGS) -c $<

%_ispc.h %_ispc.o: %.ispc
	$(ISPC) $(ISPCFLAGS8) $< -o $*_ispc.o -h $*_ispc.h

.c.o: $(DEPS)
	$(CC) $(CCFLAGS) -c $<

%.o: %.C $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< 

default : testHarness
all : testHarness

testHarness : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(CUDALIBS)

runSerial : testHarness
	./testHarness serial

runSSE2 : testHarness
	./testHarness SSE2

runAVX : testHarness
	./testHarness AVX 

runISPC : testHarness
	./testHarness ISPC

clean:	
	rm -f $(OBJS) testHarness ppms/*.ppm ppms/*.jpg *~ *_ispc.h
