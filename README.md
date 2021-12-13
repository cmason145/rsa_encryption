Assignment 6- RSA Encryption

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

Command line options
• -h : Prints out a help message describing the purpose of the graph and the command-line options
it accepts, exiting the program afterwards. Refer to the reference program in the resources repo for
an idea of what to print.
• -v : Enables verbose printing. If enabled, the program prints out all Hamiltonian paths found as
well as the total number of recursive calls to dfs().
• -i infile : Specify the input file to encrypt/decrypt. If not specified, the default input should be set as stdin.
• -o outfile : Specify the output file output the encrypted/decrypted message. If not specified, the default output should be
set as stdout.
• -n private key: file which has private/public key
