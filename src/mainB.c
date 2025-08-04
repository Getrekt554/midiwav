#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int parse_note(const char* note) {
    if (strcmp(note, "A3") == 0) return -12;
    else if (strcmp(note, "A2#") == 0 || strcmp(note, "B2b") == 0) return -11;
    else if (strcmp(note, "B2") == 0  || strcmp(note, "C2b") == 0) return -10;
    else if (strcmp(note, "B2#") == 0 || strcmp(note, "C2") == 0) return -9;
    else if (strcmp(note, "C2#") == 0 || strcmp(note, "D2b") == 0) return -8;
    else if (strcmp(note, "D2") == 0) return -7;
    else if (strcmp(note, "D2#") == 0 || strcmp(note, "E2b") == 0) return -6;
    else if (strcmp(note, "E2")  == 0 || strcmp(note, "F2b") == 0) return -5;
    else if (strcmp(note, "E2#") == 0 || strcmp(note, "F2") == 0) return -4;
    else if (strcmp(note, "F2#") == 0 || strcmp(note, "G2b") == 0) return -3;
    else if (strcmp(note, "G2") == 0) return -2;
    else if (strcmp(note, "G2#") == 0 || strcmp(note, "A3b") == 0) return -1;

    else if (strcmp(note, "A3") == 0) return 0;
    else if (strcmp(note, "A3#") == 0 || strcmp(note, "B3b") == 0) return 1;
    else if (strcmp(note, "B3") == 0  || strcmp(note, "C3b") == 0) return 2;
    else if (strcmp(note, "B3#") == 0 || strcmp(note, "C3") == 0) return 3;
    else if (strcmp(note, "C3#") == 0 || strcmp(note, "D3b") == 0) return 4;
    else if (strcmp(note, "D3") == 0) return 5;
    else if (strcmp(note, "D3#") == 0 || strcmp(note, "E3b") == 0) return 6;
    else if (strcmp(note, "E3")  == 0 || strcmp(note, "F3b") == 0) return 7;
    else if (strcmp(note, "E3#") == 0 || strcmp(note, "F3") == 0) return 8;
    else if (strcmp(note, "F3#") == 0 || strcmp(note, "G3b") == 0) return 9;
    else if (strcmp(note, "G3") == 0) return 10;
    else if (strcmp(note, "G3#") == 0 || strcmp(note, "A3b") == 0) return 11;

    else if (strcmp(note, "A4") == 0) return 12;
    else if (strcmp(note, "A4#") == 0 || strcmp(note, "B4b") == 0) return 13;
    else if (strcmp(note, "B4") == 0  || strcmp(note, "C4b") == 0) return 14;
    else if (strcmp(note, "B4#") == 0 || strcmp(note, "C4") == 0) return 15;
    else if (strcmp(note, "C4#") == 0 || strcmp(note, "D4b") == 0) return 16;
    else if (strcmp(note, "D4") == 0) return 17;
    else if (strcmp(note, "D4#") == 0 || strcmp(note, "E4b") == 0) return 18;
    else if (strcmp(note, "E4")  == 0 || strcmp(note, "F4b") == 0) return 19;
    else if (strcmp(note, "E4#") == 0 || strcmp(note, "F4") == 0) return 20;
    else if (strcmp(note, "F4#") == 0 || strcmp(note, "G4b") == 0) return 21;
    else if (strcmp(note, "G4") == 0) return 22;
    else if (strcmp(note, "G4#") == 0 || strcmp(note, "A4b") == 0) return 23;
    else if (strcmp(note, "A5") == 0) return 24;
    
    return -1;
}

int* decode_notes(char* notes_str, int* out_len) {
    char* notes = strdup(notes_str);  
    char* tok = strtok(notes, " ");

    int capacity = 16;
    int* ints = malloc(sizeof(int) * capacity);
    int index = 0;

    while (tok != NULL) {
        if (index >= capacity) {
            capacity *= 2;
            ints = realloc(ints, sizeof(int) * capacity);
        }

        ints[index++] = parse_note(tok);
        tok = strtok(NULL, " ");
    }

    free(notes);
    *out_len = index;
    return ints;
}

int main() {
    int len = 0;
    int* notes = decode_notes("C3 E3b A3b C4", &len);

    for (int i = 0; i < len; i++) {
        printf("%d\n", notes[i]);
    }

    free(notes);
    return 0;
}
