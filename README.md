# RSA Encryption

Explanation of program
The program is meant to utilize RSA encryption to encrypt and decrypt files, making it so that if one doesn't know the private key, it is nearly impossible to read the original message.

Files to submit:

rsa.{c, h}
numtheory.{c, h}
randstate.{c, h}
decrypt.c
encrypt.c
keygen.c
Makefile
README.md
DESIGN.pdf


How to run the files
The tsp file can be run by running make all, then using the command line options. There are three binary files which will be created: keygen, decrypt, and encrypt. 

## Command line options
Key Generation:
- -b : specifies the minimum bits needed for the public modulus n.
- -i : specifies the number ofMiller-Rabin iterations for testing primes (default: 50).
- -n pbfile : specifies the public key file (default: rsa.pub).
- -d pvfile : specifies the private key file (default: rsa.priv).
- -s : specifies the random seed for the random state initialization (default: the seconds since the
UNIX epoch, given by time(NULL)).
- -v : enables verbose output.
- -h : displays program synopsis and usage.

Encryptor:
- -i : specifies the input file to encrypt (default: stdin).
- -o : specifies the output file to encrypt (default: stdout).
- -n : specifies the file containing the public key (default: rsa.pub).
- -v : enables verbose output.
- -h : displays program synopsis and usage.

Decryptor:
- -i : specifies the input file to decrypt (default: stdin).
- -o : specifies the output file to decrypt (default: stdout).
- -n : specifies the file containing the private key (default: rsa.priv).
- -v : enables verbose output.
- -h : displays program synopsis and usage.
