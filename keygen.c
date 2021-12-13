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

#define OPTIONS "b:i:n:d:s:vh"

void help(void) {
    fprintf(stderr, "Help Screen.\n");
    return;
}

int main(int argc, char **argv) {
    int opt = 0;
    // Default files
    FILE *pbfile = fopen("rsa.pub", "w+");
    FILE *pvfile = fopen("rsa.priv", "w+");

    // Default values
    uint64_t seed = time(NULL);
    uint64_t bits = 255;
    uint64_t iterations = 50;
    bool verbose = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': bits = (uint64_t) atoi(optarg); break;
        case 'i': iterations = (uint64_t) atoi(optarg); break;
        case 'n':
            if ((pbfile = fopen(optarg, "w+")) == NULL) {
                fprintf(stderr, "Error: Failed to open public key file.\n");
                return EXIT_FAILURE;
            }
            break;
        case 'd':
            if ((pvfile = fopen(optarg, "w+")) == NULL) {
                fprintf(stderr, "Error: Failed to open private key file.\n");
                return EXIT_FAILURE;
            }
            break;
        case 's': seed = (uint64_t) atoi(optarg); break;
        case 'v': verbose = true; break;
        case 'h': help(); return EXIT_SUCCESS;
        default: help(); return EXIT_FAILURE;
        }
    }

    // Set file perms of pvfile to 0600
    int perms;
    if ((perms = fileno(pvfile)) == -1) {
        fprintf(stderr, "Error: Failed to set permissions of pvfile.\n");
        return EXIT_FAILURE;
    }
    fchmod(perms, 0600);

    // Initialize random state
    randstate_init(seed);

    // Initilalize our p, q, n, and e values
    mpz_t p, q, n, e;
    mpz_inits(p, q, n, e, NULL);
    mpz_set_ui(p, 1);
    mpz_set_ui(q, 1);
    mpz_set_ui(n, 1);
    mpz_set_ui(e, 1);

    // Make the public key
    rsa_make_pub(p, q, n, e, bits, iterations);

    mpz_t d;
    mpz_inits(d, NULL);
    mpz_set_ui(d, 1);

    // Make the private key
    rsa_make_priv(d, e, p, q);

    // Get current user's name
    char *username = getenv("USER");

    // Initialize username
    mpz_t user;
    mpz_inits(user, NULL);
    mpz_set_ui(user, 0);

    // Convert username to mpz_t
    mpz_set_str(user, username, 62);

    mpz_t s;
    mpz_inits(s, NULL);
    mpz_set_ui(s, 0);

    // Get signature of the username
    rsa_sign(s, user, d, n);

    // Write public key
    rsa_write_pub(n, e, s, username, pbfile);

    // Write private key
    rsa_write_priv(n, d, pvfile);

    if (verbose) {
        fprintf(stdout, "user = %s\n", username);

        fprintf(stdout, "s (%zu bits) = ", mpz_sizeinbase(s, 2));
        gmp_fprintf(stdout, "%Zd", s);
        fprintf(stdout, "\n");

        fprintf(stdout, "p (%zu bits) = ", mpz_sizeinbase(p, 2));
        gmp_fprintf(stdout, "%Zd", p);
        fprintf(stdout, "\n");

        fprintf(stdout, "q (%zu bits) = ", mpz_sizeinbase(q, 2));
        gmp_fprintf(stdout, "%Zd", q);
        fprintf(stdout, "\n");

        fprintf(stdout, "n (%zu bits) = ", mpz_sizeinbase(n, 2));
        gmp_fprintf(stdout, "%Zd", n);
        fprintf(stdout, "\n");

        fprintf(stdout, "e (%zu bits) = ", mpz_sizeinbase(e, 2));
        gmp_fprintf(stdout, "%Zd", e);
        fprintf(stdout, "\n");

        fprintf(stdout, "d (%zu bits) = ", mpz_sizeinbase(d, 2));
        gmp_fprintf(stdout, "%Zd", d);
        fprintf(stdout, "\n");
    }

    mpz_clears(q, p, n, e, d, user, s, NULL);
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    return 0;
}
