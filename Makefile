
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

# naming scheme: testHarness[platform]_[un][th] 
# 	where [platform]=SSE2|AVX|Phi
# 	[un] = loop unroll factor (1, 2, 4, or 8)
# 	[th] = 't' to use multithreading, or 's' for single-threaded
# 	In addition, Phi executables must be suffixed with '.mic'
BINS_SSE2 = testHarnessSSE2_1t testHarnessSSE2_2t testHarnessSSE2_4t testHarnessSSE2_8t testHarnessSSE2_8s
BINS_AVX = testHarnessAVX_1t testHarnessAVX_2t testHarnessAVX_4t testHarnessAVX_8t testHarnessAVX_8s
BINS_PHI = testHarnessPhi_1t.mic testHarnessPhi_2t.mic testHarnessPhi_4t.mic testHarnessPhi_8t.mic testHarnessPhi_8s.mic

OBJS_SSE2_1t = mandel_ispc_sse2_1t.o mandelKernels_sse2_1t.o testHarness_sse2_1t.o WKFUtils_sse2.o
OBJS_SSE2_2t = $(subst 1t,2t,$(OBJS_SSE2_1t))
OBJS_SSE2_4t = $(subst 1t,4t,$(OBJS_SSE2_1t))
OBJS_SSE2_8t = $(subst 1t,8t,$(OBJS_SSE2_1t))
OBJS_SSE2_8s = $(subst 1t,8s,$(OBJS_SSE2_1t))
OBJS_AVX_1t = $(subst sse2,avx,$(OBJS_SSE2_1t))
OBJS_AVX_2t = $(subst 1t,2t,$(OBJS_AVX_1t))
OBJS_AVX_4t = $(subst 1t,4t,$(OBJS_AVX_1t))
OBJS_AVX_8t = $(subst 1t,8t,$(OBJS_AVX_1t))
OBJS_AVX_8s = $(subst 1t,8s,$(OBJS_AVX_1t))
OBJS_PHI_1t = $(subst sse2,phi,$(OBJS_SSE2_1t))
OBJS_PHI_2t = $(subst 1t,2t,$(OBJS_PHI_1t))
OBJS_PHI_4t = $(subst 1t,4t,$(OBJS_PHI_1t))
OBJS_PHI_8t = $(subst 1t,8t,$(OBJS_PHI_1t))
OBJS_PHI_8s = $(subst 1t,8s,$(OBJS_PHI_1t))

ISPCDEPS = mandel_ispc.h  # automatically generated below

.SUFFIXES: .C .c .cu ..c .i .o .ispc 

%.o: %.cu $(DEPS)
	$(CUDACC) $(CUDACCFLAGS) -c $<

%.o: %.c $(DEPS)
	$(CC) $(CCFLAGS) -c $<

%.o: %.C $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< 

# compiling miscellaneous files for SSE2, AVX, Phi
%_sse2.o : %.C 
	$(CXX) $(CXXFLAGS) -msse2 -o $@ -c $<
%_avx.o : %.C 
	$(CXX) $(CXXFLAGS) -mavx -o $@ -c $<
%_phi.o : %.C 
	$(CXX) $(CXXFLAGS) -mmic -o $@ -c $<

# compiling ISPC files for SSE2
# Can also use --target=sse2-i32x4 but I found that slightly slower (consistently)
%_ispc.h %_ispc_sse2_1t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=sse2-i32x8 -DUNROLL_FACTOR=1 -DMULTITHREADED $< -o $*_ispc_sse2_1t.o -h $*_ispc.h
%_ispc.h %_ispc_sse2_2t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=sse2-i32x8 -DUNROLL_FACTOR=2 -DMULTITHREADED $< -o $*_ispc_sse2_2t.o -h $*_ispc.h
%_ispc.h %_ispc_sse2_4t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=sse2-i32x8 -DUNROLL_FACTOR=4 -DMULTITHREADED $< -o $*_ispc_sse2_4t.o -h $*_ispc.h
%_ispc.h %_ispc_sse2_8t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=sse2-i32x8 -DUNROLL_FACTOR=8 -DMULTITHREADED $< -o $*_ispc_sse2_8t.o -h $*_ispc.h
%_ispc.h %_ispc_sse2_8s.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=sse2-i32x8 -DUNROLL_FACTOR=8 $< -o $*_ispc_sse2_8s.o -h $*_ispc.h


# compiling ISPC files for AVX
# Can also use --target=avx1-i32x16 but I found that very much slower (consistently)
%_ispc.h %_ispc_avx_1t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=avx1-i32x8 -DUNROLL_FACTOR=1 -DMULTITHREADED $< -o $*_ispc_avx_1t.o -h $*_ispc.h
%_ispc.h %_ispc_avx_2t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=avx1-i32x8 -DUNROLL_FACTOR=2 -DMULTITHREADED $< -o $*_ispc_avx_2t.o -h $*_ispc.h
%_ispc.h %_ispc_avx_4t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=avx1-i32x8 -DUNROLL_FACTOR=4 -DMULTITHREADED $< -o $*_ispc_avx_4t.o -h $*_ispc.h
%_ispc.h %_ispc_avx_8t.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=avx1-i32x8 -DUNROLL_FACTOR=8 -DMULTITHREADED $< -o $*_ispc_avx_8t.o -h $*_ispc.h
%_ispc.h %_ispc_avx_8s.o: %.ispc
	$(ISPC) $(ISPCFLAGS) --target=avx1-i32x8 -DUNROLL_FACTOR=8 $< -o $*_ispc_avx_8s.o -h $*_ispc.h


# compiling ISPC files to C++ for Phi
%_ispc_1t.C : %.ispc	
	$(ISPC) $(ISPCFLAGS) $< --emit-c++ --target=generic-16 -DUNROLL_FACTOR=1 -DMULTITHREADED -o $*_ispc_1t.C --c++-include-file=knc.h
%_ispc_2t.C : %.ispc	
	$(ISPC) $(ISPCFLAGS) $< --emit-c++ --target=generic-16 -DUNROLL_FACTOR=2 -DMULTITHREADED -o $*_ispc_2t.C --c++-include-file=knc.h
%_ispc_4t.C : %.ispc	
	$(ISPC) $(ISPCFLAGS) $< --emit-c++ --target=generic-16 -DUNROLL_FACTOR=4 -DMULTITHREADED -o $*_ispc_4t.C --c++-include-file=knc.h
%_ispc_8t.C : %.ispc	
	$(ISPC) $(ISPCFLAGS) $< --emit-c++ --target=generic-16 -DUNROLL_FACTOR=8 -DMULTITHREADED -o $*_ispc_8t.C --c++-include-file=knc.h
%_ispc_8s.C : %.ispc	
	$(ISPC) $(ISPCFLAGS) $< --emit-c++ --target=generic-16 -DUNROLL_FACTOR=8 -o $*_ispc_8s.C --c++-include-file=knc.h

# compiling ISPC-generated C++ files for Phi
%_ispc_phi_1t.o : %_ispc_1t.C
	$(CXX) $(CXXFLAGS) -mmic -o $@ -c $<
%_ispc_phi_2t.o : %_ispc_2t.C
	$(CXX) $(CXXFLAGS) -mmic -o $@ -c $<
%_ispc_phi_4t.o : %_ispc_4t.C
	$(CXX) $(CXXFLAGS) -mmic -o $@ -c $<
%_ispc_phi_8t.o : %_ispc_8t.C
	$(CXX) $(CXXFLAGS) -mmic -o $@ -c $<
%_ispc_phi_8s.o : %_ispc_8s.C
	$(CXX) $(CXXFLAGS) -mmic -o $@ -c $<


# compiling C++ files for AVX
%_avx_1t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mavx -DUNROLL_FACTOR=1 -DMULTITHREADED -o $@ -c $<
%_avx_2t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mavx -DUNROLL_FACTOR=2 -DMULTITHREADED -o $@ -c $<
%_avx_4t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mavx -DUNROLL_FACTOR=4 -DMULTITHREADED -o $@ -c $<
%_avx_8t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mavx -DUNROLL_FACTOR=8 -DMULTITHREADED -o $@ -c $<
%_avx_8s.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mavx -DUNROLL_FACTOR=8 -o $@ -c $<

# compiling C++ files for SSE2
%_sse2_1t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -msse2 -DUNROLL_FACTOR=1 -DMULTITHREADED -o $@ -c $<
%_sse2_2t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -msse2 -DUNROLL_FACTOR=2 -DMULTITHREADED -o $@ -c $<
%_sse2_4t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -msse2 -DUNROLL_FACTOR=4 -DMULTITHREADED -o $@ -c $<
%_sse2_8t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -msse2 -DUNROLL_FACTOR=8 -DMULTITHREADED -o $@ -c $<
%_sse2_8s.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -msse2 -DUNROLL_FACTOR=8 -o $@ -c $<

# compiling C++ files for Phi
%_phi_1t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mmic -DUNROLL_FACTOR=1 -DMULTITHREADED -o $@ -c $<
%_phi_2t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mmic -DUNROLL_FACTOR=2 -DMULTITHREADED -o $@ -c $<
%_phi_4t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mmic -DUNROLL_FACTOR=4 -DMULTITHREADED -o $@ -c $<
%_phi_8t.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mmic -DUNROLL_FACTOR=8 -DMULTITHREADED -o $@ -c $<
%_phi_8s.o: %.C $(ISPCDEPS)
	$(CXX) $(CXXFLAGS) -mmic -DUNROLL_FACTOR=8 -o $@ -c $<

default : all
all : $(BINS_SSE2) $(BINS_AVX) makeTable
phi : $(BINS_PHI) makeTable

testHarnessSSE2_1t : $(OBJS_SSE2_1t) 
	$(CXX) $(CXXFLAGS) -msse2 $(OBJS_SSE2_1t) -o $@ 
testHarnessSSE2_2t : $(OBJS_SSE2_2t) 
	$(CXX) $(CXXFLAGS) -msse2 $(OBJS_SSE2_2t) -o $@ 
testHarnessSSE2_4t : $(OBJS_SSE2_4t) 
	$(CXX) $(CXXFLAGS) -msse2 $(OBJS_SSE2_4t) -o $@ 
testHarnessSSE2_8t : $(OBJS_SSE2_8t) 
	$(CXX) $(CXXFLAGS) -msse2 $(OBJS_SSE2_8t) -o $@ 
testHarnessSSE2_8s : $(OBJS_SSE2_8s) 
	$(CXX) $(CXXFLAGS) -msse2 $(OBJS_SSE2_8s) -o $@ 

testHarnessAVX_1t : $(OBJS_AVX_1t) 
	$(CXX) $(CXXFLAGS) -mavx $(OBJS_AVX_1t) -o $@ 
testHarnessAVX_2t : $(OBJS_AVX_2t) 
	$(CXX) $(CXXFLAGS) -mavx $(OBJS_AVX_2t) -o $@ 
testHarnessAVX_4t : $(OBJS_AVX_4t) 
	$(CXX) $(CXXFLAGS) -mavx $(OBJS_AVX_4t) -o $@ 
testHarnessAVX_8t : $(OBJS_AVX_8t) 
	$(CXX) $(CXXFLAGS) -mavx $(OBJS_AVX_8t) -o $@ 
testHarnessAVX_8s : $(OBJS_AVX_8s) 
	$(CXX) $(CXXFLAGS) -mavx $(OBJS_AVX_8s) -o $@ 

testHarnessPhi_1t.mic : $(OBJS_PHI_1t) 
	$(CXX) $(CXXFLAGS) -mmic $(OBJS_PHI_1t) -o $@ 
testHarnessPhi_2t.mic : $(OBJS_PHI_2t) 
	$(CXX) $(CXXFLAGS) -mmic $(OBJS_PHI_2t) -o $@ 
testHarnessPhi_4t.mic : $(OBJS_PHI_4t) 
	$(CXX) $(CXXFLAGS) -mmic $(OBJS_PHI_4t) -o $@ 
testHarnessPhi_8t.mic : $(OBJS_PHI_8t) 
	$(CXX) $(CXXFLAGS) -mmic $(OBJS_PHI_8t) -o $@ 
testHarnessPhi_8s.mic : $(OBJS_PHI_8s) 
	$(CXX) $(CXXFLAGS) -mmic $(OBJS_PHI_8s) -o $@ 

makeTable : makeTable.o
	$(CXX) $(CXXFLAGS) makeTable.o -o $@

runALL : testHarnessSSE2_8t testHarnessAVX_8t
	@echo; echo; echo "Reference serial code"
	./testHarnessSSE2_8t serial -b
	@echo; echo; echo "SSE2 using intrinsics"
	./testHarnessSSE2_8t intrin -b
	@echo; echo; echo "SSE2 using Intel compiler vectorization"
	./testHarnessSSE2_8t intel -b
	@echo; echo; echo "SSE2 using OpenMP"
	./testHarnessSSE2_8t omp -b
	@echo; echo; echo "SSE2 using ISPC"
	./testHarnessSSE2_8t ISPC -b
	@echo; echo; echo "AVX using intrinsics"
	./testHarnessAVX_8t intrin -b
	@echo; echo; echo "AVX using Intel compiler vectorization"
	./testHarnessAVX_8t intel -b
	@echo; echo; echo "AVX using OpenMP"
	./testHarnessAVX_8t omp -b
	@echo; echo; echo "AVX using ISPC"
	./testHarnessAVX_8t ISPC -b
	@echo

runALLPhi : testHarnessPhi_8t.mic
	@echo; echo; echo "Reference serial code"
	./testHarnessPhi_8t.mic serial -b
	@echo; echo; echo "Intrinsics"
	./testHarnessPhi_8t.mic intrin -b
	@echo; echo; echo "Intel compiler vectorization"
	./testHarnessPhi_8t.mic intel -b
	@echo; echo; echo "OpenMP"
	./testHarnessPhi_8t.mic omp -b
	@echo; echo; echo "ISPC"
	./testHarnessPhi_8t.mic ISPC -b
	@echo

fullTest : $(BINS_SSE2) $(BINS_AVX) $(BINS_PHI) makeTable
	./testHarnessSSE2_1t serial > fullTest.log
	./testHarnessSSE2_2t serial >> fullTest.log
	./testHarnessSSE2_4t serial >> fullTest.log
	./testHarnessSSE2_8t serial >> fullTest.log
	./testHarnessSSE2_8s serial >> fullTest.log
	./testHarnessSSE2_1t intrin >> fullTest.log
	./testHarnessSSE2_2t intrin >> fullTest.log
	./testHarnessSSE2_4t intrin >> fullTest.log
	./testHarnessSSE2_8t intrin >> fullTest.log
	./testHarnessSSE2_8s intrin >> fullTest.log
	./testHarnessSSE2_1t intel >> fullTest.log
	./testHarnessSSE2_2t intel >> fullTest.log
	./testHarnessSSE2_4t intel >> fullTest.log
	./testHarnessSSE2_8t intel >> fullTest.log
	./testHarnessSSE2_8s intel >> fullTest.log
	./testHarnessSSE2_1t omp >> fullTest.log
	./testHarnessSSE2_2t omp >> fullTest.log
	./testHarnessSSE2_4t omp >> fullTest.log
	./testHarnessSSE2_8t omp >> fullTest.log
	./testHarnessSSE2_8s omp >> fullTest.log
	./testHarnessSSE2_1t ISPC >> fullTest.log
	./testHarnessSSE2_2t ISPC >> fullTest.log
	./testHarnessSSE2_4t ISPC >> fullTest.log
	./testHarnessSSE2_8t ISPC >> fullTest.log
	./testHarnessSSE2_8s ISPC >> fullTest.log
	./testHarnessAVX_1t serial >> fullTest.log
	./testHarnessAVX_2t serial >> fullTest.log
	./testHarnessAVX_4t serial >> fullTest.log
	./testHarnessAVX_8t serial >> fullTest.log
	./testHarnessAVX_8s serial >> fullTest.log
	./testHarnessAVX_1t intrin >> fullTest.log
	./testHarnessAVX_2t intrin >> fullTest.log
	./testHarnessAVX_4t intrin >> fullTest.log
	./testHarnessAVX_8t intrin >> fullTest.log
	./testHarnessAVX_8s intrin >> fullTest.log
	./testHarnessAVX_1t intel >> fullTest.log
	./testHarnessAVX_2t intel >> fullTest.log
	./testHarnessAVX_4t intel >> fullTest.log
	./testHarnessAVX_8t intel >> fullTest.log
	./testHarnessAVX_8s intel >> fullTest.log
	./testHarnessAVX_1t omp >> fullTest.log
	./testHarnessAVX_2t omp >> fullTest.log
	./testHarnessAVX_4t omp >> fullTest.log
	./testHarnessAVX_8t omp >> fullTest.log
	./testHarnessAVX_8s omp >> fullTest.log
	./testHarnessAVX_1t ISPC >> fullTest.log
	./testHarnessAVX_2t ISPC >> fullTest.log
	./testHarnessAVX_4t ISPC >> fullTest.log
	./testHarnessAVX_8t ISPC >> fullTest.log
	./testHarnessAVX_8s ISPC >> fullTest.log
	./testHarnessPhi_1t.mic serial >> fullTest.log
	./testHarnessPhi_2t.mic serial >> fullTest.log
	./testHarnessPhi_4t.mic serial >> fullTest.log
	./testHarnessPhi_8t.mic serial >> fullTest.log
	./testHarnessPhi_8s.mic serial >> fullTest.log
	./testHarnessPhi_1t.mic intrin >> fullTest.log
	./testHarnessPhi_2t.mic intrin >> fullTest.log
	./testHarnessPhi_4t.mic intrin >> fullTest.log
	./testHarnessPhi_8t.mic intrin >> fullTest.log
	./testHarnessPhi_8s.mic intrin >> fullTest.log
	./testHarnessPhi_1t.mic intel >> fullTest.log
	./testHarnessPhi_2t.mic intel >> fullTest.log
	./testHarnessPhi_4t.mic intel >> fullTest.log
	./testHarnessPhi_8t.mic intel >> fullTest.log
	./testHarnessPhi_8s.mic intel >> fullTest.log
	./testHarnessPhi_1t.mic omp >> fullTest.log
	./testHarnessPhi_2t.mic omp >> fullTest.log
	./testHarnessPhi_4t.mic omp >> fullTest.log
	./testHarnessPhi_8t.mic omp >> fullTest.log
	./testHarnessPhi_8s.mic omp >> fullTest.log
	./testHarnessPhi_1t.mic ISPC >> fullTest.log
	./testHarnessPhi_2t.mic ISPC >> fullTest.log
	./testHarnessPhi_4t.mic ISPC >> fullTest.log
	./testHarnessPhi_8t.mic ISPC >> fullTest.log
	./testHarnessPhi_8s.mic ISPC >> fullTest.log
	./makeTable fullTest.log

clean:	
	rm -f *.o $(ISPCDEPS) testHarness makeTable $(BINS_SSE2) $(BINS_AVX) $(BINS_PHI) ppms/*.ppm ppms/*.jpg *.ppm *~ *_ispc_phi.cpp *_ispc_phi.C *_ispc.C
