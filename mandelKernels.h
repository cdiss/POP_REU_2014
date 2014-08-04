void serialMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters);

void autoVecMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters);

void intrinsicsMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, signed maxIters);

void interchangeMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters);

void ompMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters);

void ispcMandel(float startReal, float startImag, int steps, int horizsteps, float step, unsigned* output, unsigned maxIters);

