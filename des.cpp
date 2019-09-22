#include <iostream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <vector>

using namespace std;


const int PLAINTEXT = 64;
const int KEY = 56;
const int HALF = PLAINTEXT / 2;
const int HALF_KEY = KEY / 2;
const int GENERATED_KEY = 48;
const int ROUNDS = 16;
const int EXPANDED = 48;
const int COUNT = 8;
const int ROWS = 4;
const int COLUMNS = 16;


const int IP[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};


const int FP[] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25
};


const int D_BOX[] = {
    32,  1, 2,  3,  4,  5,
     4,  5, 6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 31, 31, 32,  1
};


const int S[COUNT][ROWS][COLUMNS] = {
    {
        { 14,  4, 13, 1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9, 0,  7 },
        {  0, 15,  0, 4, 14,  2, 13, 10,  3,  6, 12, 11,  9,  5, 3,  8 },
        {  4,  1, 14, 8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10, 5,  0 },
        { 15, 12,  8, 2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0, 6, 13 }
    },
    {
        { 15,  1,  8, 14,  6, 11,  3,  4,  9, 7,  2, 13, 12, 0,  5, 10 },
        {  3, 13,  4,  7, 15,  2,  8, 14, 12, 0,  1, 10,  6, 9, 11,  5 },
        {  0, 14,  7, 11, 10,  4, 13,  1,  5, 8, 12,  6,  9, 3,  2, 15 },
        { 13,  8, 10,  1,  3, 15,  4,  2, 11, 6,  7, 12,  0, 5, 14,  9 }
    },
    {
        { 10,  0,  9, 14, 6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8 },
        { 13,  7,  0,  9, 3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1 },
        { 13,  6,  4,  9, 8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7 },
        {  1, 10, 13,  0, 6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 }
    },
    {
        {  7, 13, 14, 3,  0,  6,  9, 10,  1, 2, 8,  5, 11, 12,  4, 15 },
        { 13,  8, 11, 5,  6, 15,  0,  3,  4, 7, 2, 12,  1, 10, 14,  9 },
        { 10,  6,  9, 0, 12, 11,  7, 13, 15, 1, 3, 14,  5,  2,  8,  4 },
        {  3, 15,  0, 6, 10,  1, 13,  8,  9, 4, 5, 11, 12,  7,  2, 14 }
    },
    {
        {  2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13, 0, 14,  9 },
        { 14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3, 9,  8,  6 },
        {  4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6, 3,  0, 14 },
        { 11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10, 4,  5,  3 }
    },
    {
        { 12,  1, 10, 15, 9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11 },
        { 10, 15,  4,  2, 7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8 },
        {  9, 14, 15,  5, 2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6 },
        {  4,  3,  2, 12, 9,  5, 15, 10, 11, 14,  1,  7, 10,  0,  8, 13 }
    },
    {
        {  4, 11,  2, 14, 15, 0,  8, 13,  3, 12, 9,  7,  5, 10, 6,  1 },
        { 13,  0, 11,  7,  4, 9,  1, 10, 14,  3, 5, 12,  2, 15, 8,  6 },
        {  1,  4, 11, 13, 12, 3,  7, 14, 10, 15, 6,  8,  0,  5, 9,  2 },
        {  6, 11, 13,  8,  1, 4, 10,  7,  9,  5, 0, 15, 14,  2, 3, 12 }
    },
    {
        { 13,  2,  8, 4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7 },
        {  1, 15, 13, 8, 10,  3,  7,  4, 12,  5,  6, 11, 10, 14,  9,  2 },
        {  7, 11,  4, 1,  9, 12, 14,  2,  0,  6, 10, 10, 15,  3,  5,  8 },
        {  2,  1, 14, 7,  4, 10,  8, 13, 15, 12,  9,  9,  3,  5,  6, 11 }
    }
};


const int KEY_SCHEDULE[] {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};


const int PC1[] {
    57, 49, 41, 33, 25, 17,  9,  1,
    58, 50, 42, 34, 26, 18, 10,  2,
    59, 51, 43, 35, 27, 19, 11,  3,
    60, 52, 44, 36, 63, 55, 47, 39,
    31, 23, 15,  7, 62, 54, 46, 38,
    30, 22, 14,  6, 61, 53, 45, 37,
    29, 21, 13,  5, 28, 20, 12,  4
};


const int PC2[] {
    14, 17, 11, 24,  1,  5,  3, 28,
    15,  6, 21, 10, 23, 19, 12,  4,
    26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56,
    34, 53, 46, 42, 50, 36, 29, 32,
};


template<int SIZE>
string toHex(bitset<SIZE> bits) {
    stringstream res;
    res << hex << uppercase << bits.to_ulong();
    return res.str();
}

template<int SIZE, int HALF>
bitset<HALF> leftSplit(bitset<SIZE> input) {
    bitset<HALF> half = bitset<HALF>();
    for (int i = 0; i < HALF; i++) {
        half[i] = input[i];
    }
    return half;
}

template<int SIZE, int HALF>
bitset<HALF> rightSplit(bitset<SIZE> input) {
    bitset<HALF> half = bitset<HALF>();
    for (int i = 0; i < HALF; i++) {
        half[i] = input[i + HALF];
    }
    return half;
}

template<int SIZE>
void shiftLeft(bitset<SIZE>& input) {
    bool temp = input[0];
    for (int i = 0; i < input.size() - 1; i++) {
        input[i] = input[i + 1];
    }
    input[input.size() - 1] = temp;
}

template<int SIZE>
void shiftRight(bitset<SIZE>& input) {
    bool temp = input[input.size() - 1];
    for (int i = input.size() - 1; i > 0; i--) {
        input[i] = input[i - 1];
    }
    input[0] = temp;
}


template<int SIZE1, int SIZE2>
bitset<SIZE1 + SIZE2> concatenate(bitset<SIZE1> left, bitset<SIZE1> right) {
    bitset<SIZE1 + SIZE2> res = bitset<SIZE1 + SIZE2>();
    for (int i = 0; i < SIZE1 + SIZE2; i++) {
        if (i < SIZE1) {
            res[i] = left[i];
        } else {
            res[i] = right[i - SIZE1];
        }
    }
    return res;
}


bitset<HALF> func(bitset<HALF> input, bitset<GENERATED_KEY> key) {
    bitset<EXPANDED> expanded = bitset<EXPANDED>();
    
    for (int i = 0; i < expanded.size(); i++) {
        expanded[i] = input[D_BOX[i] - 1];
    }
    
    bitset<EXPANDED> r = key ^ expanded;
    bitset<HALF> res = bitset<HALF>();

    for (int i = 0; i < res.size(); i += 4) {
        bitset<2> row = bitset<2>();
        bitset<4> column = bitset<4>();
        row[0] = r[i / 4 * 6];
        column[0] = r[i / 4 * 6 + 1];
        column[1] = r[i / 4 * 6 + 2];
        column[2] = r[i / 4 * 6 + 3];
        column[3] = r[i / 4 * 6 + 4];
        row[1] = r[i / 4 * 6 + 5];
        int v = S[i / 4][row.to_ulong()][column.to_ulong()];
        bitset<4> out = bitset<4>(v);
        res[i] = out[0];
        res[i + 1] = out[1];
        res[i + 2] = out[2];
        res[i + 3] = out[3];
    }

    return res;
}


bitset<PLAINTEXT> process(bitset<PLAINTEXT> plain, bitset<PLAINTEXT> key, bool reverse=false) {
    vector<bitset<GENERATED_KEY>> keys;

    bitset<KEY> cipherKey = bitset<KEY>();
    for (int i = 0; i < cipherKey.size(); i++) {
        cipherKey[i] = key[PC1[i] - 1];
    }
    
    bitset<HALF_KEY> leftKey = leftSplit<KEY, HALF_KEY>(cipherKey);
    bitset<HALF_KEY> rightKey = rightSplit<KEY, HALF_KEY>(cipherKey);

    for (int i = 0; i < ROUNDS; i++) {
        for (int j = 0; j < KEY_SCHEDULE[i]; j++) {
            shiftLeft<HALF_KEY>(leftKey);
            shiftRight<HALF_KEY>(rightKey);
        }

        bitset<KEY> combined = concatenate<HALF_KEY, HALF_KEY>(leftKey, rightKey);
        bitset<GENERATED_KEY> subKey = bitset<GENERATED_KEY>();

        for (int j = 0; j < subKey.size(); j++) {
            subKey[j] = combined[PC2[j] - 1];
        }

        keys.push_back(subKey);
    }

    bitset<PLAINTEXT> temp = bitset<PLAINTEXT>();
    for (int i = 0; i < PLAINTEXT; i++) {
        temp[i] = plain[IP[i] - 1];
    }

    bitset<HALF> left = leftSplit<PLAINTEXT, HALF>(temp);
    bitset<HALF> right = rightSplit<PLAINTEXT, HALF>(temp);

    for (int i = 0; i < ROUNDS; i++) {
        bitset<HALF> temp = reverse? left ^ func(right, keys[ROUNDS - i - 1]) : left ^ func(right, keys[i]);
        left = right;
        right = temp;
    }

    bitset<PLAINTEXT> r = concatenate<HALF, HALF>(right, left);

    bitset<PLAINTEXT> res = bitset<PLAINTEXT>();

    for (int i = 0; i < PLAINTEXT; i++) {
        res[i] = r[FP[i] - 1];
    }

    return res;
}


bitset<PLAINTEXT> encrypt(bitset<PLAINTEXT> plain, bitset<PLAINTEXT> key) {
    return process(plain, key, false);
}

bitset<PLAINTEXT> decrypt(bitset<PLAINTEXT> plain, bitset<PLAINTEXT> key) {
    return process(plain, key, true);
}

bitset<PLAINTEXT> encryptDoubleDes(
    bitset<PLAINTEXT> plain,
    bitset<PLAINTEXT> key1,
    bitset<PLAINTEXT> key2
) {
    return encrypt(decrypt(plain, key1), key2);
}

bitset<PLAINTEXT> decryptDoubleDes(
    bitset<PLAINTEXT> plain,
    bitset<PLAINTEXT> key1,
    bitset<PLAINTEXT> key2
) {
    return encrypt(decrypt(plain, key2), key1);
}

bitset<PLAINTEXT> encryptTripleDes(
    bitset<PLAINTEXT> plain,
    bitset<PLAINTEXT> key1,
    bitset<PLAINTEXT> key2,
    bitset<PLAINTEXT> key3
) {
    return decrypt(encrypt(decrypt(plain, key1), key2), key3);
}

bitset<PLAINTEXT> decryptTripleDes(
    bitset<PLAINTEXT> plain,
    bitset<PLAINTEXT> key1,
    bitset<PLAINTEXT> key2,
    bitset<PLAINTEXT> key3
) {
    return encrypt(decrypt(encrypt(plain, key3), key2), key1);
}


int main() {
    bitset<PLAINTEXT> plain = bitset<PLAINTEXT>(0x0123456789ABCDEF);
    bitset<PLAINTEXT> key1 = bitset<PLAINTEXT>(0xCDDD34EFF0011EF5);
    bitset<PLAINTEXT> key2 = bitset<PLAINTEXT>(0x133457799BBCDFF1);
    bitset<PLAINTEXT> key3 = bitset<PLAINTEXT>(0xEDBBA24FF210EBCE);

    cout << setfill('0') << setw(PLAINTEXT / 4) << toHex<PLAINTEXT>(plain) << endl;
    cout << setfill('0') << setw(PLAINTEXT / 4) << toHex<PLAINTEXT>(encryptDoubleDes(plain, key1, key2)) << endl;
    cout << setfill('0') << setw(PLAINTEXT / 4) << toHex<PLAINTEXT>(decryptDoubleDes(encryptDoubleDes(plain, key1, key2), key1, key2)) << endl;

    return 0;
}