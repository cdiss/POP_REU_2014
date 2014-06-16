typedef struct {
  float *v;
  float *a;
  float *u;

  float *av;
  float *aa;
  float *au;
} msmparms;  

int setupmsm(msmparms &parms);
int finishmsm(msmparms &parms);
int testmsm(msmparms &parms);
