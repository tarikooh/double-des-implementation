#include <stdio.h>
#include <stdint.h>
#include <string.h>


/* Initial Permutation Table */
static const int IP[] = {
58,50,42,34,26,18,10,2,
60,52,44,36,28,20,12,4,
62,54,46,38,30,22,14,6,
64,56,48,40,32,24,16,8,
57,49,41,33,25,17,9,1,
59,51,43,35,27,19,11,3,
61,53,45,37,29,21,13,5,
63,55,47,39,31,23,15,7
};

/* Final Permutation */
static const int FP[] = {
40,8,48,16,56,24,64,32,
39,7,47,15,55,23,63,31,
38,6,46,14,54,22,62,30,
37,5,45,13,53,21,61,29,
36,4,44,12,52,20,60,28,
35,3,43,11,51,19,59,27,
34,2,42,10,50,18,58,26,
33,1,41,9,49,17,57,25
};

/* Expansion table */
static const int E[] = {
32,1,2,3,4,5,4,5,6,7,8,9,
8,9,10,11,12,13,12,13,14,15,16,17,
16,17,18,19,20,21,20,21,22,23,24,25,
24,25,26,27,28,29,28,29,30,31,32,1
};

/* Simple S-box */
static const int S[4][16] = {
{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
};

/* Permutation P */
static const int P[] = {
16,7,20,21,29,12,28,17,
1,15,23,26,5,18,31,10,
2,8,24,14,32,27,3,9,
19,13,30,6,22,11,4,25
};

/* Rotate left */
uint32_t rotl28(uint32_t val, int shift) {
    return ((val << shift) | (val >> (28 - shift))) & 0x0FFFFFFF;
}

/* Generic permutation */
uint64_t permute(uint64_t input, const int *table, int n) {
    uint64_t output = 0;
    for (int i = 0; i < n; i++) {
        output <<= 1;
        output |= (input >> (64 - table[i])) & 1;
    }
    return output;
}

/* Feistel function */
uint32_t feistel(uint32_t r, uint64_t subkey) {
    uint64_t expanded = permute((uint64_t)r << 32, E, 48);
    expanded ^= subkey;

    uint32_t output = 0;

    for (int i = 0; i < 8; i++) {
        int chunk = (expanded >> (42 - 6*i)) & 0x3F;
        int row = ((chunk & 0x20) >> 4) | (chunk & 1);
        int col = (chunk >> 1) & 0xF;

        int val = S[row][col];
        output = (output << 4) | val;
    }

    return (uint32_t)permute((uint64_t)output << 32, P, 32);
}

/* Simple subkey generation */
void generate_subkeys(uint64_t key, uint64_t subkeys[16]) {
    for (int i = 0; i < 16; i++) {
        subkeys[i] = (key >> i) & 0xFFFFFFFFFFFF;
    }
}

/* DES encrypt block */
uint64_t des_encrypt_block(uint64_t block, uint64_t key) {
    uint64_t subkeys[16];
    generate_subkeys(key, subkeys);

    block = permute(block, IP, 64);

    uint32_t L = block >> 32;
    uint32_t R = block & 0xFFFFFFFF;

    for (int i = 0; i < 16; i++) {
        uint32_t temp = R;
        R = L ^ feistel(R, subkeys[i]);
        L = temp;
    }

    uint64_t preoutput = ((uint64_t)R << 32) | L;
    return permute(preoutput, FP, 64);
}

/* DES decrypt block */
uint64_t des_decrypt_block(uint64_t block, uint64_t key) {
    uint64_t subkeys[16];
    generate_subkeys(key, subkeys);

    block = permute(block, IP, 64);

    uint32_t L = block >> 32;
    uint32_t R = block & 0xFFFFFFFF;

    for (int i = 15; i >= 0; i--) {
        uint32_t temp = R;
        R = L ^ feistel(R, subkeys[i]);
        L = temp;
    }

    uint64_t preoutput = ((uint64_t)R << 32) | L;
    return permute(preoutput, FP, 64);
}

uint64_t double_des_encrypt(uint64_t block, uint64_t key1, uint64_t key2) {
    block = des_encrypt_block(block, key1);
    block = des_encrypt_block(block, key2);
    return block;
}

uint64_t double_des_decrypt(uint64_t block, uint64_t key1, uint64_t key2) {
    block = des_decrypt_block(block, key2);
    block = des_decrypt_block(block, key1);
    return block;
}


int main() {
    uint64_t plaintext = 0x0123456789ABCDEF;
    uint64_t key1 = 0x133457799BBCDFF1;
    uint64_t key2 = 0x1F1F1F1F0E0E0E0E;

    printf("Plaintext:  %016llX\n", plaintext);

    uint64_t ciphertext = double_des_encrypt(plaintext, key1, key2);
    printf("Ciphertext: %016llX\n", ciphertext);

    uint64_t decrypted = double_des_decrypt(ciphertext, key1, key2);
    printf("Decrypted:  %016llX\n", decrypted);

    return 0;
}
