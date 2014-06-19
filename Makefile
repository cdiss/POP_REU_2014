
CC = icc
CFLAGS = -Wall -m64 -O3 
CXX = icpc
CXXFLAGS = -Wall -m64 -O3 -openmp -fno-alias
CUDAHOME = /usr/local/encap/cuda-5.0
CUDACC = $(CUDAHOME)/bin/nvcc
CUDACCFLAGS = -m64 -arch sm_21
CUDALIBS = -Bdynamic -Wl,-rpath,$(CUDAHOME)/lib64 -L$(CUDAHOME)/lib64 -lcudart 
ISPC = ispc
ISPCFLAGS = -O2 --arch=x86-64 

# The performance difference between these two options should be tested at some point
#ISPCFLAGS +=  --target=avx1-i32x8
#ISPCFLAGS +=  --target=avx1-i32x16
# Likewise, these two options for SSE2
#ISPCFLAGS +=  --target=sse2-i32x4
#ISPCFLAGS +=  --target=sse2-i32x8

OBJS_AVX = kernelispc_ispc_avx.o testHarness_avx.o WKFUtils.o #cudaFloat.o
OBJS_SSE2 = kernelispc_ispc_sse2.o testHarness_sse2.o WKFUtils.o #cudaFloat.o
OBJS_PHI = kernelispc_ispc_phi.o testHarness_phi.o WKFUtils_phi.o #cudaFloat.o
ISPCDEPS_AVX = kernelispc_ispc_avx.h  # automatically generated below
ISPCDEPS_SSE2 = kernelispc_ispc_sse2.h  # automatically generated below

.SUFFIXES: .C .c .cu ..c .i .o .ispc 

%.o: %.cu $(DEPS)
	$(CUDACC) $(CUDACCFLAGS) -c $<

%.o: %.c $(DEPS)
	$(CC) $(CCFLAGS) -c $<

%.o: %.C $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< 

# compiling ISPC files for AVX
%_ispc_avx.h %_ispc_avx.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=avx1-i32x8 $< -o $*_ispc_avx.o -h $*_ispc_avx.h

# compiling ISPC files for SSE2
%_ispc_sse2.h %_ispc_sse2.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=sse2-i32x4 $< -o $*_ispc_sse2.o -h $*_ispc_sse2.h

# compiling ISPC files to C++ for Phi
%_ispc_phi.C : %.ispc	
	$(ISPC) $(ISPCFLAGS) $< --emit-c++ --target=generic-16 -o $*_ispc_phi.C --c++-include-file=knc.h

# compiling C++ files for AVX
%_avx.o: %.C $(ISPCDEPS_AVX)
	$(CXX) $(CXXFLAGS) -mavx -o $@ -c $<

# compiling C++ files for SSE2
%_sse2.o: %.C $(ISPCDEPS_SSE2)
	$(CXX) $(CXXFLAGS) -msse2 -o $@ -c $<

# compiling C++ files for Phi
%_phi.o: %.C
	$(CXX) $(CXXFLAGS) -mmic -o $@ -c $<

default : all
all : testHarnessAVX testHarnessSSE2

testHarnessAVX : $(OBJS_AVX) 
	$(CXX) $(CXXFLAGS) -mavx $(OBJS_AVX) -o $@ 

testHarnessSSE2 : $(OBJS_SSE2) 
	$(CXX) $(CXXFLAGS) -msse2 $(OBJS_SSE2) -o $@ 

testHarnessPhi.mic : $(OBJS_PHI)
	$(CXX) $(CXXFLAGS) -mmic $(OBJS_PHI) -o $@

runSerial : testHarnessSSE2
	./testHarnessSSE2 serial -b

runSSE2_INTRIN : testHarnessSSE2
	./testHarnessSSE2 SSE2 -b

runAVX_INTRIN : testHarnessAVX
	./testHarnessAVX AVX -b

runPHI_INTRIN : testHarnessPhi.mic
	./testHarnessPhi.mic Phi -b

runSSE2_ISPC : testHarnessSSE2
	./testHarnessSSE2 ISPC -b

runAVX_ISPC : testHarnessAVX
	./testHarnessAVX ISPC -b

runPHI_ISPC : testHarnessPhi.mic
	./testHarnessPhi.mic ISPC -b

runALL : testHarnessSSE2 testHarnessAVX
	@echo; echo
	@echo "SSE2 using intrinsics"
	@make runSSE2_INTRIN
	@echo; echo
	@echo "SSE2 using ISPC"
	@make runSSE2_ISPC
	@echo; echo
	@echo "AVX using intrinsics"
	@make runAVX_INTRIN
	@echo; echo
	@echo "AVX using ISPC"
	@make runAVX_ISPC
	@echo

clean:	
	rm -f *.o $(ISPCDEPS_AVX) $(ISPCDEPS_SSE2) testHarness testHarnessAVX testHarnessSSE2 testHarnessPhi.mic ppms/*.ppm ppms/*.jpg *.ppm *~ *_ispc_phi.cpp *_ispc_phi.C 
