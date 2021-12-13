#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void gcd(mpz_t g, mpz_t a, mpz_t b) {
    // Initialize temp vars
    mpz_t temp_b;
    mpz_t temp_a;
    mpz_init(temp_b);
    mpz_init(temp_a);

    // Set vars to temp variables
    mpz_set(temp_a, a); // temp_a <- a
    mpz_set(temp_b, b); // temp_b <- b

    while (mpz_cmp_ui(temp_b, 0) != 0) {
        // Initialize temp t var
        mpz_t t;
        mpz_init(t);

        mpz_set(t, temp_b); //t <- b

        mpz_mod(temp_b, temp_a, temp_b); // b <- a mod b

        mpz_set(temp_a, t); // a <- t

        // clear t value
        mpz_clear(t);
    }

    mpz_set(g, temp_a); // return a, g <- a

    // Clear temp vars
    mpz_clear(temp_a);
    mpz_clear(temp_b);

    return;
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
    // Initialize our variables
    mpz_t r;
    mpz_t r_p;
    mpz_t t;
    mpz_t t_p;

    mpz_init(r);
    mpz_init(r_p);
    mpz_init(t);
    mpz_init(t_p);

    // (r, r') <- (n, a)
    mpz_set(r, n);
    mpz_set(r_p, a);

    // (t, t') <- (0, 1)
    mpz_set_ui(t, 0);
    mpz_set_ui(t_p, 1);

    while (mpz_cmp_ui(r_p, 0) != 0) {
        // Initialize q
        mpz_t q;
        mpz_init(q);

        mpz_fdiv_q(q, r, r_p); // q <- floor(r, r')

        // Initialize temp placeholder for r`
        mpz_t temp_r_p;
        mpz_init(temp_r_p);
        mpz_set(temp_r_p, r_p);

        // Initialize temp placeholder for r
        mpz_t temp_r;
        mpz_init(temp_r);
        mpz_set(temp_r, r);

        mpz_set(r, temp_r_p); // r <- r'

        // Initialize value of q x r'
        mpz_t qmr_p;
        mpz_init(qmr_p);
        mpz_mul(qmr_p, q, temp_r_p);

        // Initialize value of r - q x r`
        mpz_t rsqr_p;
        mpz_init(rsqr_p);
        mpz_sub(rsqr_p, temp_r, qmr_p);

        // Set r' to r - q x r'
        mpz_set(r_p, rsqr_p);

        // Clear all the variables we just used
        mpz_clear(temp_r_p);
        mpz_clear(temp_r);
        mpz_clear(qmr_p);
        mpz_clear(rsqr_p);

        // Initialize temp placeholder for t`
        mpz_t temp_t_p;
        mpz_init(temp_t_p);
        mpz_set(temp_t_p, t_p);

        // Initialize temp placeholder for t
        mpz_t temp_t;
        mpz_init(temp_t);
        mpz_set(temp_t, t);

        mpz_set(t, temp_t_p); // t <- t'

        // Initialize value of q x t'
        mpz_t qmt_p;
        mpz_init(qmt_p);
        mpz_mul(qmt_p, q, temp_t_p);

        // Initialize value of t - q x t`
        mpz_t tsqt_p;
        mpz_init(tsqt_p);
        mpz_sub(tsqt_p, temp_t, qmt_p);

        // Set t' to t - q x t'
        mpz_set(t_p, tsqt_p);

        mpz_clear(temp_t_p);
        mpz_clear(temp_t);
        mpz_clear(qmt_p);
        mpz_clear(tsqt_p);
        mpz_clear(q);
    }

    // If r > 1
    if (mpz_cmp_ui(r, 1) > 0) {
        // Set o to 0, return to inverse
        mpz_set_ui(o, 0);

        // Clear all of our values
        mpz_clear(t_p);
        mpz_clear(t);
        mpz_clear(r_p);
        mpz_clear(r);
        return;
    }

    if (mpz_cmp_ui(t, 0) < 0) {
        // Initialize t + n
        mpz_t tpn;
        mpz_init(tpn);
        mpz_add(tpn, t, n);

        // Set t to t + n
        mpz_set(t, tpn);
        mpz_clear(tpn);
    }

    // Return t
    mpz_set(o, t); // o <- t

    // Clear all of our values
    mpz_clear(t_p);
    mpz_clear(t);
    mpz_clear(r_p);
    mpz_clear(r);
    return;
}

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    // Initialize temp variables
    mpz_t temp_a;
    mpz_t temp_d;
    mpz_t temp_n;
    mpz_init(temp_a);
    mpz_init(temp_d);
    mpz_init(temp_n);

    // Set variables to temp values
    mpz_set(temp_a, a); // temp_a <- a
    mpz_set(temp_d, d); // temp_d <- d
    mpz_set(temp_n, n); // temp_n <- n

    // Create v variable
    mpz_t v;
    mpz_init(v);
    mpz_set_ui(v, 1); // v <- 1

    // Create p variable
    mpz_t p;
    mpz_init(p);
    mpz_set(p, temp_a); // p <- a

    while (mpz_cmp_ui(temp_d, 0) > 0) {
        if (mpz_odd_p(temp_d)) {
            // Initialize result of v x p
            mpz_t vtp;
            mpz_init(vtp);

            // vtp <- v x p
            mpz_mul(vtp, v, p);

            // v <- (v x p) mod n
            mpz_mod(v, vtp, temp_n);
            mpz_clear(vtp);
        }
        // Initialize result of p x p
        mpz_t ptp;
        mpz_init(ptp);

        // ptp <- p x p
        mpz_mul(ptp, p, p);

        // p <- (p x p) mod n
        mpz_mod(p, ptp, temp_n);

        mpz_clear(ptp);

        // d <- floor(d / 2)
        mpz_fdiv_q_ui(temp_d, temp_d, 2);
    }

    // Return v, o <- v
    mpz_set(o, v);

    // Clear all of the values we created
    mpz_clear(temp_a);
    mpz_clear(temp_d);
    mpz_clear(temp_n);

    mpz_clear(v);
    mpz_clear(p);

    return;
}

bool is_prime(mpz_t n, uint64_t iters) {
    // Initialize temp variables
    mpz_t temp_n;
    mpz_init(temp_n);

    mpz_set(temp_n, n); // temp_n <- n

    // We can do a quick check if n is 2 or 3, it must be prime
    if ((mpz_cmp_ui(temp_n, 2) == 0) || (mpz_cmp_ui(temp_n, 3) == 0)) {
        mpz_clear(temp_n);
        return true;
    }

    mpz_t check;
    mpz_init(check);
    mpz_mod_ui(check, temp_n, 2); // check <- n % 2

    // Check if n <= 1 or if n % 2 == 0, indicating we have an even number n, which by definition cannot be prime
    if ((mpz_cmp_ui(temp_n, 1) <= 0) || (mpz_cmp_ui(check, 0) == 0)) {
        mpz_clear(check);
        mpz_clear(temp_n);
        return false;
    }

    // Clear our temp variable check
    mpz_clear(check);

    // Initialize r and s
    mpz_t r;
    mpz_t s;
    mpz_init(r);
    mpz_init(s);

    mpz_set_ui(s, 0); // s <- 0

    mpz_sub_ui(r, temp_n, 1); // r <- n - 1

    // Find s and r
    while (mpz_even_p(r)) {
        mpz_add_ui(s, s, 1); // s <- s + 1
        mpz_fdiv_q_ui(r, r, 2); // r <- floor(r, 2)
    }

    // Initialize counter
    mpz_t i;
    mpz_init(i);
    for (mpz_set_ui(i, 1); mpz_cmp_ui(i, iters) <= 0; mpz_add_ui(i, i, 1)) {
        // Initialize a
        mpz_t a;
        mpz_init(a);

        // Initialize n - 3
        mpz_t n_3;
        mpz_init(n_3);
        mpz_set(n_3, temp_n);

        mpz_sub_ui(n_3, n_3, 3); // n_3 <- n - 3
        mpz_urandomm(a, state, n_3);

        // In order to ensure that it says in the 2 range
        mpz_add_ui(a, a, 2); // a <- a + 2

        // Clear n - 3
        mpz_clear(n_3);

        // Initialize y
        mpz_t y;
        mpz_init(y);
        mpz_set_ui(y, 0);

        pow_mod(y, a, r, temp_n); // y <- pow_mod(a, r, n);

        // Initialize n - 1
        mpz_t n_1;
        mpz_init(n_1);
        mpz_set(n_1, temp_n);

        mpz_sub_ui(n_1, n_1, 1); // n_1 <- n - 1

        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_1) != 0)) {
            // Initialize j
            mpz_t j;
            mpz_init(j);
            mpz_set_ui(j, 1);

            // Initialize s - 1
            mpz_t s_1;
            mpz_init(s_1);
            mpz_set(s_1, s);

            mpz_sub_ui(s_1, s_1, 1); // s_1 <- s - 1

            while ((mpz_cmp(j, s_1) <= 0) && (mpz_cmp(y, n_1) != 0)) {
                // Initialize two
                mpz_t two;
                mpz_init(two);
                mpz_set_ui(two, 2);

                pow_mod(y, y, two, temp_n); // y <- pow_mod(y, 2, n);

                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(temp_n, i, r, s, two, j, s_1, n_1, y, a, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1); // j <- j + 1

                // Clear the two
                mpz_clear(two);
            }

            if (mpz_cmp(y, n_1) != 0) {
                mpz_clears(j, s_1, n_1, y, a, i, r, s, temp_n, NULL);
                return false;
            }

            mpz_clear(j);
            mpz_clear(s_1);
        }

        mpz_clear(n_1);
        mpz_clear(y);
        mpz_clear(a);
    }

    mpz_clear(i);

    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(temp_n);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    // Initialize guess
    mpz_t guess;
    mpz_init(guess);

    // Initialize offset value
    mpz_t offset;
    mpz_init(offset);

    // Initialize value of one
    mpz_t one;
    mpz_init(one);
    mpz_set_ui(one, 1);

    // offset <- 2^(bits - 1)
    mpz_mul_2exp(offset, one, bits - 1);

    // offset <- offset - 1
    mpz_sub_ui(offset, offset, 1);

    // While our value is even or isn't a prime
    while (mpz_even_p(guess) || (!(is_prime(guess, iters)))) {
        // From 0 to 2^(bits-1) - 1
        mpz_urandomb(guess, state, bits - 1);

        // guess <- guess + offset
        mpz_add(guess, offset, guess);
    }

    // Return the guess to p
    mpz_set(p, guess); // p <- guess

    // Clear values
    mpz_clears(guess, offset, one, NULL);
    return;
}

// int main(void) {
//     // MOD INVERSE TEST
//     // Get integer value
//     //mpz_t a, n, o;
//     // mpz_inits(a, n, o, NULL);
//     // mpz_set_ui(a, 0);
//     // mpz_set_ui(n, 0);
//     // mpz_set_ui(o, 0);

//     // printf("Integer value = ");
//     // gmp_scanf("%Zd", a);

//     // printf("Mod value = ");
//     // gmp_scanf("%Zd", n);

//     // // Get mod inverse value
//     // mod_inverse(o, a, n);

//     // gmp_printf("Mod inverse of %Zd mod %Zd = %Zd", a, n, o);
//     // printf("\n");

//     // mpz_clears(a, n, o, NULL);

//     // // MOD EXP TEST
//     // // Get base value
//     // mpz_t base;
//     // mpz_init(base);
//     // printf("base = ");
//     // gmp_scanf("%Zd", base);

//     // // Get exponent value
//     // mpz_t exp;
//     // mpz_init(exp);
//     // printf("exponent = ");
//     // gmp_scanf("%Zd", exp);

//     // // Get modulus value
//     // mpz_t modul;
//     // mpz_init(modul);
//     // printf("modulus = ");
//     // gmp_scanf("%Zd", modul);

//     // // Calculate test modular exponentation
//     // mpz_t res;
//     // mpz_init(res);
//     // pow_mod(res, base, exp, modul);

//     // // Calculate actual value
//     // mpz_t actual;
//     // mpz_init(actual);
//     // mpz_powm(actual, base, exp, modul);

//     // // Print result
//     // printf("test values: \n");
//     // gmp_printf("%Zd ^ %Zd mod %Zd = %Zd", base, exp, modul, res);
//     // printf("\n");

//     // // Print actual value
//     // printf("actual value: \n");
//     // gmp_printf("%Zd ^ %Zd mod %Zd = %Zd", base, exp, modul, actual);
//     // printf("\n");

//     // mpz_clear(base);
//     // mpz_clear(exp);
//     // mpz_clear(modul);
//     // mpz_clear(actual);
//     // mpz_clear(res);

//     // Initialize seed
//     randstate_init(65537);

//     // Get value to test primality
//     mpz_t prime;
//     mpz_init(prime);
//     printf("prime = ");
//     gmp_scanf("%Zd", prime);

//     // Get number of iterations
//     uint64_t iters;
//     printf("number of iterations = ");
//     scanf("%" SCNu64, &iters);

//     bool result;

//     result = is_prime(prime, iters);

//     if (result) {
//         gmp_printf("%Zd is a prime", prime);
//     } else {
//         gmp_printf("%Zd is not a prime", prime);
//     }
//     printf("\n");

//     mpz_clear(prime);
//     randstate_clear();

//     // //MAKE_PRIME TEST
//     // mpz_t p;
//     // mpz_init(p);
//     // uint64_t bits = 20;
//     // uint64_t iters = 70;
//     // uint64_t seed = time(0);
//     // randstate_init(seed);
//     // make_prime(p, bits, iters);
//     // printf("make_prime generated numbers:\n");
//     // gmp_printf("%Zd", p);
//     // printf("\n");
//     // printf("primality:\n");
//     // printf("%d\n", is_prime(p, iters));
//     // randstate_clear();
//     // mpz_clear(p);

//     return 0;
// }
