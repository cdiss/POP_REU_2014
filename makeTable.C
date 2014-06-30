#include <stdio.h>
#include <string.h>

void parseData(FILE* in, float data[]);

int main(int argc, char* argv[]) {
    FILE* out = fopen("table.out", "w");
    if (!out) { printf("Error opening table.out\n"); return 0; }
    FILE* in = fopen(argv[1], "r");
    if (!in) { printf("Error opening file specified\n"); return 0; }
    float data[75];
    parseData(in, data);
    fclose(in);
    fprintf(out, "---------------------------------------------------------------\n");
    fprintf(out, "----------------------Benchmark Results------------------------\n");
    fprintf(out, "\n");
    fprintf(out, "   -- All results in serial-equivalent GFLOPS.\n");
    fprintf(out, "   -- That is, GFLOPS counting only those operations which are \n");
    fprintf(out, "   -- performed in the reference serial code.\n");
    fprintf(out, "\n\n");
    fprintf(out, "----------------SSE2------------------------------------------\n");
    fprintf(out, "|                        ---------- Unrolling level ----------\n");
    fprintf(out, "|                        1         2         4         8          Single-Threaded\n");
    fprintf(out, "|REFERENCE SERIAL    %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[0], data[1], data[2], data[3], data[4]);
    fprintf(out, "|Intrinsics          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[5], data[6], data[7], data[8], data[9]);
    fprintf(out, "|Intel vectorization %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[10], data[11], data[12], data[13], data[14]);
    fprintf(out, "|Intel OpenMP 4.0    %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[15], data[16], data[17], data[18], data[19]);
    fprintf(out, "|ISPC 1.7            %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[20], data[21], data[22], data[23], data[24]);
    fprintf(out, "--------------------------------------------------------------\n\n");
    fprintf(out, "----------------AVX-------------------------------------------\n");
    fprintf(out, "|                        ---------- Unrolling level ----------\n");
    fprintf(out, "|                        1         2         4         8          Single-Threaded\n");
    fprintf(out, "|REFERENCE SERIAL    %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[25], data[26], data[27], data[28], data[29]);
    fprintf(out, "|Intrinsics          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[30], data[31], data[32], data[33], data[34]);
    fprintf(out, "|Intel vectorization %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[35], data[36], data[37], data[38], data[39]);
    fprintf(out, "|Intel OpenMP 4.0    %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[40], data[41], data[42], data[43], data[44]);
    fprintf(out, "|ISPC 1.7            %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[45], data[46], data[47], data[48], data[49]);
    fprintf(out, "--------------------------------------------------------------\n\n");
    fprintf(out, "----------------PHI-------------------------------------------\n");
    fprintf(out, "|                        ---------- Unrolling level ----------\n");
    fprintf(out, "|                        1         2         4         8          Single-Threaded\n");
    fprintf(out, "|REFERENCE SERIAL    %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[50], data[51], data[52], data[53], data[54]);
    fprintf(out, "|Intrinsics          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[55], data[56], data[57], data[58], data[59]);
    fprintf(out, "|Intel vectorization %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[60], data[61], data[62], data[63], data[64]);
    fprintf(out, "|Intel OpenMP 4.0    %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[65], data[66], data[67], data[68], data[69]);
    fprintf(out, "|ISPC 1.7            %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[70], data[71], data[72], data[73], data[74]);
    fprintf(out, "--------------------------------------------------------------\n\n");
    fclose(out);
    FILE* gnutablesse2 = fopen("gnutable_sse2.out", "w");
    if (!gnutablesse2) { printf("Error opening gnutable_sse2.out\n"); return 0; }
    fprintf(gnutablesse2, "# SSE2 table data for plotting with gnuplot\n");
    fprintf(gnutablesse2, "# Comma separated rather than whitespace separated (use\n");
    fprintf(gnutablesse2, "# set datafile separator \",\"), so that whitespace can appear\n");
    fprintf(gnutablesse2, "# in the labels.\n");
    fprintf(gnutablesse2, "REFERENCE SERIAL,1,%f,%f,%f,%f,%f,1\n", data[0], data[1], data[2], data[3], data[4]);
    fprintf(gnutablesse2, "Intrinsics,2,%f,%f,%f,%f,%f,2\n", data[5], data[6], data[7], data[8], data[9]);
    fprintf(gnutablesse2, "Intel vectorization,3,%f,%f,%f,%f,%f,3\n", data[10], data[11], data[12], data[13], data[14]);
    fprintf(gnutablesse2, "Intel OpenMP 4.0,4,%f,%f,%f,%f,%f,4\n", data[15], data[16], data[17], data[18], data[19]);
    fprintf(gnutablesse2, "ISPC 1.7,5,%f,%f,%f,%f,%f,5\n", data[20], data[21], data[22], data[23], data[24]);
    fclose(gnutablesse2);
    FILE* gnutableavx = fopen("gnutable_avx.out", "w");
    if (!gnutableavx) { printf("Error opening gnutable_avx.out\n"); return 0; }
    fprintf(gnutableavx, "# AVX table data for plotting with gnuplot\n");
    fprintf(gnutableavx, "# Comma separated rather than whitespace separated (use\n");
    fprintf(gnutableavx, "# set datafile separator \",\"), so that whitespace can appear\n");
    fprintf(gnutableavx, "# in the labels.\n");
    fprintf(gnutableavx, "REFERENCE SERIAL,1,%f,%f,%f,%f,%f,1\n", data[25], data[26], data[27], data[28], data[29]);
    fprintf(gnutableavx, "Intrinsics,2,%f,%f,%f,%f,%f,2\n", data[30], data[31], data[32], data[33], data[34]);
    fprintf(gnutableavx, "Intel vectorization,3,%f,%f,%f,%f,%f,3\n", data[35], data[36], data[37], data[38], data[39]);
    fprintf(gnutableavx, "Intel OpenMP 4.0,4,%f,%f,%f,%f,%f,4\n", data[40], data[41], data[42], data[43], data[44]);
    fprintf(gnutableavx, "ISPC 1.7,5,%f,%f,%f,%f,%f,5\n", data[45], data[46], data[47], data[48], data[49]);
    fclose(gnutableavx);
    FILE* gnutablephi = fopen("gnutable_phi.out", "w");
    if (!gnutablephi) { printf("Error opening gnutable_phi.out\n"); return 0; }
    fprintf(gnutablephi, "# Phi table data for plotting with gnuplot\n");
    fprintf(gnutablephi, "# Comma separated rather than whitespace separated (use\n");
    fprintf(gnutablephi, "# set datafile separator \",\"), so that whitespace can appear\n");
    fprintf(gnutablephi, "# in the labels.\n");
    fprintf(gnutablephi, "REFERENCE SERIAL,1,%f,%f,%f,%f,%f,1\n", data[50], data[51], data[52], data[53], data[54]);
    fprintf(gnutablephi, "Intrinsics,2,%f,%f,%f,%f,%f,2\n", data[55], data[56], data[57], data[58], data[59]);
    fprintf(gnutablephi, "Intel vectorization,3,%f,%f,%f,%f,%f,3\n", data[60], data[61], data[62], data[63], data[64]);
    fprintf(gnutablephi, "Intel OpenMP 4.0,4,%f,%f,%f,%f,%f,4\n", data[65], data[66], data[67], data[68], data[69]);
    fprintf(gnutablephi, "ISPC 1.7,5,%f,%f,%f,%f,%f,5\n", data[70], data[71], data[72], data[73], data[74]);
    fclose(gnutablephi);

}

void parseData(FILE* in, float data[]) {
    for (int i = 0; i < 75; i++) {
        // read until found 'GFLOPS:'
        char word[50] = {'\0'};
        while (strcmp(word, "GFLOPS:")) {
            if (fscanf(in, "%s", word) == EOF) { printf("Reached end of file.\n"); break; }
            printf("%s\n", word);
        }
        // read the GFLOPS value
        fscanf(in, "%f", &data[i]);
        printf("Read data value: %f\n", data[i]);
    }
}
