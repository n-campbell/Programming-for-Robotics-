#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

bool is_uppercase(char *letter) {
    return *letter >= 'A' && *letter <= 'Z';
}

bool is_lowercase(char *letter) {
    return *letter >= 'a' && *letter <= 'z';
}

bool is_alphabet(char *letter) {
    return *letter <= 'z' && *letter >= 'A';
}

char* only_alpha(char* string) {
    int j = 0;
    int count = 0;
    for (int i = 0; i < strlen(string); i++) {
        if (is_alphabet(&string[i])) {
            string[j] = string[i];
            j = j+1;
        }
        else {
            count = count +1;
        }
    }
    string[strlen(string)-count] = 0;
    return string;
}

char* encrypt(char *passcode, char *encryption_code) {
    char c; 
    int loc; // location of pass_letter (0-25)
    int shift = -1; // the number of rotations the encript_letter is 
    char shift_c;
    encryption_code = only_alpha(encryption_code);
    if (strlen(encryption_code) == 0) {
        printf("%s\n", passcode);
        return 0;
    }
    for (int i = 0; i < strlen(passcode); i++) {
        if (is_alphabet(&passcode[i])) {
            shift = (shift+1)%strlen(encryption_code);
            shift_c = encryption_code[shift];
            //printf("shift character %c\n",shift_c);
            if (is_uppercase(&passcode[i])) {
                loc = passcode[i] - 'A';
                if (is_lowercase(&shift_c)){
                    shift_c = (shift_c + ('A'-'a')) - 'A';
                }
                if (is_uppercase(&shift_c)) {
                    shift_c = shift_c - 'A';
                }
                c = (char)((loc + shift_c) % 26 + 'A');
            } 
            if (is_lowercase(&passcode[i])) {
                loc = passcode[i] - 'a';
                if (is_lowercase(&shift_c)) {
                    shift_c = shift_c - 'a';
                }
                if (is_uppercase(&shift_c)) {
                    shift_c = (shift_c - ('A'-'a')) - 'a';
                }
                c = (char)((loc + shift_c) % 26 + 'a');
            }
            passcode[i] = c;
        }
    }
    printf("%s\n",passcode);
    return passcode;
}

char* decrypt (char *passcode, char *encryption_code) {
    char c; 
    int loc; // location of pass_letter (0-25)
    int shift = -1; // the number of rotations the encript_letter is 
    char shift_c; // character version of shift
    encryption_code = only_alpha(encryption_code);
    if (strlen(encryption_code) == 0) {
        printf("%s\n", passcode);
        return 0;
    }
    for (int i = 0; i < strlen(passcode); i++) {
        if (is_alphabet(&passcode[i])) {
            shift = (shift+1)%strlen(encryption_code);
            shift_c = encryption_code[shift];
            //printf("shift character %c\n",shift_c);
            if (is_uppercase(&passcode[i])) {
                loc = passcode[i] - 'A';
                if (is_lowercase(&shift_c)){
                    shift_c = (shift_c + ('A'-'a')) - 'A';
                }
                if (is_uppercase(&shift_c)) {
                    shift_c = shift_c - 'A';
                }
                c = (char)((loc - shift_c) % 26 + 'A');
            } 
            if (is_lowercase(&passcode[i])) {
                loc = passcode[i] - 'a';
                if (is_lowercase(&shift_c)) {
                    shift_c = shift_c - 'a';
                }
                if (is_uppercase(&shift_c)) {
                    shift_c = (shift_c - ('A'-'a')) - 'a';
                }
                if (loc < shift_c) {
                    c = passcode[i] + 26 - shift_c;
                }
                else {
                    c = (char)((loc - shift_c) % 26 + 'a');
                }
            }
            passcode[i] = c;
        }
    }
    printf("%s\n",passcode);
    return passcode;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <encrypt|decrypt> <password> <message>\n", argv[0]);
        exit(1);
    }
    if (strcmp(argv[1], "encrypt") == 0) {
        encrypt(argv[3],argv[2]);
    }
    else if (strcmp(argv[1], "decrypt") == 0) {
        decrypt(argv[3],argv[2]);
    } 
    else {
        fprintf(stderr, "expected command encrypt or decryp. got '%s'\n", argv[0]);
        exit(1);
    }
    return 0;
}