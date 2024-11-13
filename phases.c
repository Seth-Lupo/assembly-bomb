
#include "client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void read_line(FILE* stream, char *buffer, size_t size) {
    if (fgets(buffer, size, stream) == NULL) {
        printf("Ran out of lines to process... ending bomb.\n");
        exit(1);
    }
    // Remove newline if it exists
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

void phases(FILE* stream) {

    printf("Welcome tprofessor... \n");

    char *input[1024];

    printf("Let's see if this first one will stump you!\n");
    
    read_line(stream, input, 1024);
    phase_1(input);
    pass_phase(1);

    printf("OK that was easy, time for round TWO!!\n");

    read_line(stream, input, 1024);
    phase_2(input);
    pass_phase(2);

    printf("Damn your good at this. Time for number 3.\n");

    read_line(stream, input, 1024);
    phase_3(input);
    pass_phase(3);

    printf("Getting closer...\n");

    read_line(stream, input, 1024);
    phase_4(input);
    pass_phase(4);

    printf("If you thought those were easy, brace yourself.\n");

    read_line(stream, input, 1024);
    phase_5(input);
    pass_phase(5);

    printf("ALMOST THERE :)\n");

    read_line(stream, input, 1024);
    phase_6(input);
    pass_phase(6);

    printf("Ha... ha... you got me. :/\n");

    printf("You defused the bomb.\n");

     printf("Anyways this was a great introduction to networking in C so thank's for playing :)\n");

}

void phase_1(char* input) {
    
    char c;
    int x;
    int args = sscanf(input, "%c %d", &c, &x);
    if (args < 2) explode();

    if ((c-'a'+1) == x) return;

    explode();

}

int fun_2(char *s, char c) {
    int count = 0;
    while (*s) {
        if (*s == c) {
            count++;
        }
        s++;
    }
    return count;
}

void phase_2(char* input) {
    if (fun_2(input, 'c') != 10) explode();
    if (fun_2(input, 's') != 10) explode();
}

void phase_3(char* input) {
     
    int x;
    int args = sscanf(input, "%d", &x);
    if (args < 1) explode();

    if (x % 101 != 5) explode();
    if (x % 103 != 23) explode();
    if (x % 107 != 47) explode();

}

int fun_4(char *s) {
    int left = 0;
    int right = strlen(s) - 1;
    
    while (left < right) {
        if (s[left] != s[right]) {
            return 0; 
        }
        left++;
        right--;
    }
    
    return 1; 
}

void phase_4(char *input) {
    
    if (strlen(input) < 10) {
        explode();
    }

    if (input[0] == input[1]) explode();

    if (!fun_4(input)) explode();

}

int arr_5[10] = {3,6,10,31,16,57,35,36,14,80};

void phase_5(char* input) {

    int x, y, z;
    int args = sscanf(input, "%d %d %d", &x, &y, &z);
    if (args < 3) explode();

    if (x < 0 || 10 <= x) explode();
    if (y < 0 || 10 <= y) explode();
    if (z < 0 || 10 <= z) explode();

    int temp = arr_5[z];
    arr_5[z] = arr_5[y];
    arr_5[y] = arr_5[x];
    arr_5[x] = temp;

    for (int i = 0; i < 9; i++) {
        if (arr_5[i] > arr_5[i + 1]) {
            explode(); 
        }
    }

}

int fun_6(const char *str, int num) {
    int len = strlen(str);
    for (int i = len - 1; i >= 0; --i) {
        int bit = num & 1;
        if ((bit == 1 && str[i] != 'X') || (bit == 0 && str[i] != 'x')) {
            return 0;
        }
        num >>= 1;
    }
    return (num == 0);
}

void phase_6(char* input) {
    
    if (strlen(input) != 32) {
        explode();
    }

    if (fun_6(input, 12345)) {
        return;
    }

    explode();
   
}
