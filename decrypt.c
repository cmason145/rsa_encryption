#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#define OPTIONS "i:o:n:vh"

void help(void) {
    fprintf(stderr, "Help Screen.\n");
    return;
}

int main(int argc, char **argv) {
    int opt = 0;
    // Default files
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvkey = fopen("rsa.priv", "r");

    // Default values
    bool verbose = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            if ((infile = fopen(optarg, "r")) == NULL) {
                fprintf(stderr, "Error: Failed to open input file.\n");
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            if ((outfile = fopen(optarg, "w+")) == NULL) {
                fprintf(stderr, "Error: Failed to open output file.\n");
                return EXIT_FAILURE;
            }
            break;
        case 'n':
            if ((pvkey = fopen(optarg, "r")) == NULL) {
                fprintf(stderr, "Error: Failed to open private key file.\n");
                return EXIT_FAILURE;
            }
            break;
        case 'v': verbose = true; break;
        case 'h': help(); return EXIT_SUCCESS;
        default: help(); return EXIT_FAILURE;
        }
    }

    // Initialize our n, d values
    mpz_t n, d;
    mpz_inits(n, d, NULL);
    mpz_set_ui(n, 0);
    mpz_set_ui(d, 0);

    rsa_read_priv(n, d, pvkey);

    if (verbose) {
        fprintf(stdout, "n (%zu bits) = ", mpz_sizeinbase(n, 2));
        gmp_fprintf(stdout, "%Zd", n);
        fprintf(stdout, "\n");

        fprintf(stdout, "d (%zu bits) = ", mpz_sizeinbase(d, 2));
        gmp_fprintf(stdout, "%Zd", d);
        fprintf(stdout, "\n");
    }

    rsa_decrypt_file(infile, outfile, n, d);

    fclose(infile);
    fclose(outfile);
    fclose(pvkey);
    mpz_clears(n, d, NULL);

    return 0;
}
