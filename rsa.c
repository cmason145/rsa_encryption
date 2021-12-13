#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <gmp.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

void lg(mpz_t o, mpz_t n) {
    // Initialize k to 0, temp_n to n
    mpz_t k, temp_n;
    mpz_inits(k, temp_n, NULL);
    mpz_set_ui(k, 0);
    mpz_set(temp_n, n);

    // Set n to abs(n)
    mpz_abs(temp_n, temp_n);

    while (mpz_cmp_ui(temp_n, 0) > 0) {
        // Floor division using bitwise arithmetic, quicker
        mpz_tdiv_q_2exp(temp_n, temp_n, 1);
        // k += 1
        mpz_add_ui(k, k, 1);
    }

    // o <- k
    mpz_set(o, k);
    mpz_clears(k, temp_n, NULL);
    return;
}

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    uint64_t lower = nbits / 4;
    uint64_t high = (3 * nbits) / 4;

    srandom(time(0));

    // Calculate bits for p in this range
    uint64_t bits_for_p = rand() % (high - lower + 1) + lower;
    uint64_t bits_for_q = nbits - bits_for_p;

    // Initialize log_2(n) value
    mpz_t log2val;
    mpz_inits(log2val, NULL);
    mpz_set_ui(log2val, 0);

    //return;

    do {
        // Generate random prime numbers
        make_prime(p, bits_for_p, iters);
        make_prime(q, bits_for_q, iters);

        // Set n to p x q
        mpz_mul(n, p, q);

        // log2val <- log_2(n)
        lg(log2val, n);
    } while (mpz_cmp_ui(log2val, nbits) < 0);

    mpz_clears(log2val, NULL);

    // Initialize values to calculate phi(n)
    mpz_t phi_n, p_1, q_1;
    mpz_inits(phi_n, p_1, q_1, NULL);
    mpz_set_ui(phi_n, 0);
    mpz_set_ui(p_1, 0);
    mpz_set_ui(q_1, 0);

    mpz_sub_ui(p_1, p, 1);
    mpz_sub_ui(q_1, q, 1);

    // phi_n = (q-1)(p-1)
    mpz_mul(phi_n, q_1, p_1);

    mpz_t gcd_term, rand_value;
    mpz_inits(gcd_term, rand_value, NULL);
    mpz_set_ui(gcd_term, 0);
    mpz_set_ui(rand_value, 0);

    do {
        mpz_urandomb(rand_value, state, nbits);
        gcd(gcd_term, rand_value, phi_n);
    } while (mpz_cmp_ui(gcd_term, 1) != 0);

    mpz_set(e, rand_value);

    mpz_clears(gcd_term, rand_value, NULL);
    mpz_clears(phi_n, p_1, q_1, NULL);
    return;
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n", n, e, s);
    gmp_fprintf(pbfile, "%s\n", username);
    return;
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile,
        "%Zx"
        "\n",
        n);
    gmp_fscanf(pbfile,
        "%Zx"
        "\n",
        e);
    gmp_fscanf(pbfile,
        "%Zx"
        "\n",
        s);
    fscanf(pbfile,
        "%s"
        "\n",
        username);
    return;
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    // Initialize phi_n, p_1, and q_1
    mpz_t phi_n, p_1, q_1;
    mpz_inits(phi_n, p_1, q_1, NULL);
    mpz_set_ui(phi_n, 0);
    mpz_set_ui(p_1, 0);
    mpz_set_ui(q_1, 0);

    // phi_n = (p-1)(q-1)
    mpz_sub_ui(p_1, p, 1);
    mpz_sub_ui(q_1, q, 1);
    mpz_mul(phi_n, p_1, q_1);

    // d <- inverse of e mod phi_n
    mod_inverse(d, e, phi_n);

    mpz_clears(phi_n, p_1, q_1, NULL);

    return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
    return;
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile,
        "%Zx"
        "\n",
        n);
    gmp_fscanf(pvfile,
        "%Zx"
        "\n",
        d);
    return;
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    // Initialize our k value and log value used for calculations
    // mpz_t k, log_val;
    // mpz_inits(k, log_val, NULL);
    // mpz_set_ui(k, 0);
    // mpz_set_ui(log_val, 0);

    // // log_val = log_2(n)
    // lg(log_val, n);

    // // log_val = log_val - 1
    // mpz_sub_ui(log_val, log_val, 1);

    // // log_val = floor(log_val / 8)
    // mpz_tdiv_q_2exp(log_val, log_val, 3);

    // // k = log_val
    // mpz_set(k, log_val);

    // // Convert k to unsigned long to use in calloc
    // unsigned long k_size = mpz_get_ui(k);

    // k = log_2(n)
    size_t k = mpz_sizeinbase(n, 2);

    // k_size = (log_2(n) - 1) / 8
    size_t k_size = (k - 1) / 8;

    // Create block
    uint8_t *block = (uint8_t *) calloc(k_size, sizeof(uint8_t));

    // Set 0th byte of the block to 0xFF
    block[0] = 0xFF;

    mpz_t m, c;
    mpz_inits(m, c, NULL);
    mpz_set_ui(m, 0);
    mpz_set_ui(c, 0);

    uint64_t bytes = 0;

    do {
        bytes = fread(block + 1, sizeof(uint8_t), k_size - 1, infile);

        // Import what was read to mpz_t m
        mpz_import(m, bytes + 1, 1, sizeof(uint8_t), 1, 0, block);

        // Encrypt m, put E(m) into c
        rsa_encrypt(c, m, e, n);

        // Write to outfile
        gmp_fprintf(outfile, "%Zx\n", c);
    } while (bytes > 0);

    mpz_clears(m, c, NULL);
    // mpz_clears(k, log_val, NULL);
    free(block);
    return;
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    // Initialize our k value and log value used for calculations
    mpz_t k, log_val;
    mpz_inits(k, log_val, NULL);
    mpz_set_ui(k, 0);
    mpz_set_ui(log_val, 0);

    // log_val = log_2(n)
    lg(log_val, n);

    // log_val = log_val - 1
    mpz_sub_ui(log_val, log_val, 1);

    // log_val = floor(log_val / 8)
    mpz_tdiv_q_2exp(log_val, log_val, 3);

    // k = log_val
    mpz_set(k, log_val);

    // Convert k to unsigned long to use in calloc
    unsigned long k_size = mpz_get_ui(k);

    // Create block
    uint8_t *block = (uint8_t *) calloc(k_size, sizeof(uint8_t));

    uint64_t bytes = 0, bytes_converted = 0;

    mpz_t c, m;
    mpz_inits(c, m, NULL);
    mpz_set_ui(c, 0);
    mpz_set_ui(m, 0);

    do {
        // Scan in input
        gmp_fscanf(infile,
            "%Zx"
            "\n",
            c);

        // Decrypt the ciphertext c
        rsa_decrypt(m, c, d, n);

        // Export it to the block
        mpz_export(block, &bytes_converted, 1, sizeof(uint8_t), 1, 0, m);

        // Write to outfile
        bytes = fwrite(block + 1, sizeof(uint8_t), bytes_converted - 1, outfile);

    } while (!(feof(infile)));

    mpz_clears(c, m, NULL);
    mpz_clears(k, log_val, NULL);
    free(block);
    return;
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false;
}
