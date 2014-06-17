
CC = icc
CFLAGS = -Wall -m64 -msse2 -O3 -g 
CXX = icpc
CXXFLAGS = -Wall -m64 -mavx -O3 -g 
CUDAHOME = /usr/local/encap/cuda-5.0
CUDACC = $(CUDAHOME)/bin/nvcc
CUDACCFLAGS = -m64 -g -arch sm_21
CUDALIBS = -Bdynamic -Wl,-rpath,$(CUDAHOME)/lib64 -L$(CUDAHOME)/lib64 -lcudart 

PARAMS = -c -0.08887868 0.654803
NUMFRAMES = 3000

.SUFFIXES: .C .c .cu ..c .i .o

.cu.o:
	$(CUDACC) $(CUDACCFLAGS) -c $<

.c.o:
	$(CC) $(CCFLAGS) -c $<

.C.o:
	$(CXX) $(CXXFLAGS) -c $<

OBJS = testHarness.o WKFUtils.o cudaFloat.o

all : testHarness

testHarness : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o testHarness $(CUDALIBS)

runSerial : all
	./testHarness serial
	display mandelbrot.pgm

runSSE2 : all
	./testHarness SSE2
	display mandelbrot.pgm

runAVX : all
	./testHarness AVX 
	display mandelbrot.pgm

runCUDA : all
	./testHarness CUDA 
	display mandelbrot.pgm

animate : all
	./testHarness CUDA $(PARAMS)
	foreach x (`seq 0 $(NUMFRAMES)`)
	convert -quality 100 ppms/mandelbrot$x.ppm ppms/mandelbrot$x.jpg
	rm -f ppms/mandelbrot$x.ppm
	end
	ffmpeg -r 24 -b 1800 -i ppms/mandelbrot%d.jpg movie.mp4

clean:	
	rm -f $(OBJS) testHarness ppms/*.ppm ppms/*.jpg


