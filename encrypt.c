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
    FILE *pbkey = fopen("rsa.pub", "r");

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
            if ((pbkey = fopen(optarg, "r")) == NULL) {
                fprintf(stderr, "Error: Failed to open public key file.\n");
                return EXIT_FAILURE;
            }
            break;
        case 'v': verbose = true; break;
        case 'h': help(); return EXIT_SUCCESS;
        default: help(); return EXIT_FAILURE;
        }
    }

    char username[1024];

    // Initialize n, e, and s
    mpz_t n, e, s;
    mpz_inits(n, e, s, NULL);
    mpz_set_ui(n, 0);
    mpz_set_ui(e, 0);
    mpz_set_ui(s, 0);

    // Read public key from public key file
    rsa_read_pub(n, e, s, username, pbkey);

    if (verbose) {
        fprintf(stdout, "user = %s\n", username);

        fprintf(stdout, "s (%zu bits) = ", mpz_sizeinbase(s, 2));
        gmp_fprintf(stdout, "%Zd", s);
        fprintf(stdout, "\n");

        fprintf(stdout, "n (%zu bits) = ", mpz_sizeinbase(n, 2));
        gmp_fprintf(stdout, "%Zd", n);
        fprintf(stdout, "\n");

        fprintf(stdout, "e (%zu bits) = ", mpz_sizeinbase(e, 2));
        gmp_fprintf(stdout, "%Zd", e);
        fprintf(stdout, "\n");
    }

    // Initialize username mpz_t type
    mpz_t user;
    mpz_inits(user, NULL);
    mpz_set_ui(user, 0);

    // Convert username to mpz_t
    mpz_set_str(user, username, 62);

    // Verify the signature
    if ((rsa_verify(user, s, e, n) != true)) {
        fprintf(stderr, "Error: The signature does not match the expected value.\n");
        mpz_clears(n, e, s, user, NULL);
        fclose(infile);
        fclose(outfile);
        fclose(pbkey);
        return EXIT_FAILURE;
    }

    // Encrypt the file

    rsa_encrypt_file(infile, outfile, n, e);

    // Close out everything
    mpz_clears(n, e, s, user, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pbkey);
    return 0;
}
