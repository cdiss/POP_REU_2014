#include <stdio.h>
#include <string.h>

void parseData(FILE* in, float data[]);

int main(int argc, char* argv[]) {
    FILE* out = fopen("table.out", "w");
    if (!out) { printf("Error opening table.out\n"); return 0; }
    FILE* in = fopen(argv[1], "r");
    if (!in) { printf("Error opening file specified\n"); return 0; }
    float data[90];
    parseData(in, data);
    fclose(in);
    int i = 0;
    fprintf(out, "---------------------------------------------------------------------------------------\n");
    fprintf(out, "----------------------Benchmark Results------------------------------------------------\n");
    fprintf(out, "\n");
    fprintf(out, "   -- All results in serial-equivalent GFLOPS.\n");
    fprintf(out, "   -- That is, GFLOPS counting only those operations which are \n");
    fprintf(out, "   -- performed in the reference serial code.\n");
    fprintf(out, "\n\n");
    fprintf(out, "----------------SSE2-------------------------------------------------------------------\n");
    fprintf(out, "|                              ---------- Unrolling level ----------\n");
    fprintf(out, "|                              1         2         4         8          Single-Threaded\n");
    fprintf(out, "|REFERENCE SERIAL          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Intrinsics                %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Auto-vectorization        %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Loop interchange          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|OpenMP 4.0 (interchange)  %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|ISPC 1.7                  %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "---------------------------------------------------------------------------------------\n\n");
    fprintf(out, "----------------AVX--------------------------------------------------------------------\n");
    fprintf(out, "|                              ---------- Unrolling level ----------\n");
    fprintf(out, "|                              1         2         4         8          Single-Threaded\n");
    fprintf(out, "|REFERENCE SERIAL          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Intrinsics                %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Auto-vectorization        %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Loop interchange          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|OpenMP 4.0 (interchange)  %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|ISPC 1.7                  %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "---------------------------------------------------------------------------------------\n\n");
    fprintf(out, "----------------PHI--------------------------------------------------------------------\n");
    fprintf(out, "|                              ---------- Unrolling level ----------\n");
    fprintf(out, "|                              1         2         4         8          Single-Threaded\n");
    fprintf(out, "|REFERENCE SERIAL          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Intrinsics                %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Auto-vectorization        %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|Loop interchange          %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|OpenMP 4.0 (interchange)  %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "|ISPC 1.7                  %9.2f %9.2f %9.2f %9.2f          %9.2f\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(out, "---------------------------------------------------------------------------------------\n\n");
    fclose(out);
    FILE* gnutablesse2 = fopen("gnutable_sse2.out", "w");
    if (!gnutablesse2) { printf("Error opening gnutable_sse2.out\n"); return 0; }
    i = 0;
    fprintf(gnutablesse2, "# SSE2 table data for plotting with gnuplot\n");
    fprintf(gnutablesse2, "# Comma separated rather than whitespace separated (use\n");
    fprintf(gnutablesse2, "# set datafile separator \",\"), so that whitespace can appear\n");
    fprintf(gnutablesse2, "# in the labels.\n");
    fprintf(gnutablesse2, "REFERENCE SERIAL,1,%f,%f,%f,%f,%f,1\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablesse2, "Intrinsics,2,%f,%f,%f,%f,%f,2\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablesse2, "Auto-vectorization,3,%f,%f,%f,%f,%f,3\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablesse2, "Loop interchange,4,%f,%f,%f,%f,%f,4\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablesse2, "OpenMP 4.0 (interchange),5,%f,%f,%f,%f,%f,5\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablesse2, "ISPC 1.7,6,%f,%f,%f,%f,%f,6\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fclose(gnutablesse2);
    FILE* gnutableavx = fopen("gnutable_avx.out", "w");
    if (!gnutableavx) { printf("Error opening gnutable_avx.out\n"); return 0; }
    fprintf(gnutableavx, "# AVX table data for plotting with gnuplot\n");
    fprintf(gnutableavx, "# Comma separated rather than whitespace separated (use\n");
    fprintf(gnutableavx, "# set datafile separator \",\"), so that whitespace can appear\n");
    fprintf(gnutableavx, "# in the labels.\n");
    fprintf(gnutableavx, "REFERENCE SERIAL,1,%f,%f,%f,%f,%f,1\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutableavx, "Intrinsics,2,%f,%f,%f,%f,%f,2\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutableavx, "Auto-vectorization,3,%f,%f,%f,%f,%f,3\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutableavx, "Loop interchange,4,%f,%f,%f,%f,%f,4\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutableavx, "OpenMP 4.0 (interchange),5,%f,%f,%f,%f,%f,5\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutableavx, "ISPC 1.7,6,%f,%f,%f,%f,%f,6\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fclose(gnutableavx);
    FILE* gnutablephi = fopen("gnutable_phi.out", "w");
    if (!gnutablephi) { printf("Error opening gnutable_phi.out\n"); return 0; }
    fprintf(gnutablephi, "# Phi table data for plotting with gnuplot\n");
    fprintf(gnutablephi, "# Comma separated rather than whitespace separated (use\n");
    fprintf(gnutablephi, "# set datafile separator \",\"), so that whitespace can appear\n");
    fprintf(gnutablephi, "# in the labels.\n");
    fprintf(gnutablephi, "REFERENCE SERIAL,1,%f,%f,%f,%f,%f,1\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablephi, "Intrinsics,2,%f,%f,%f,%f,%f,2\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablephi, "Auto-vectorization,3,%f,%f,%f,%f,%f,3\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablephi, "Loop interchange,4,%f,%f,%f,%f,%f,4\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablephi, "OpenMP 4.0 (interchange),5,%f,%f,%f,%f,%f,5\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fprintf(gnutablephi, "ISPC 1.7,6,%f,%f,%f,%f,%f,6\n", data[i++], data[i++], data[i++], data[i++], data[i++]);
    fclose(gnutablephi);

}

void parseData(FILE* in, float data[]) {
    for (int i = 0; i < 90; i++) {
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
