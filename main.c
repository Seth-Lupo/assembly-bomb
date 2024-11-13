#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>

#include "client.h"
#include "phases.h"


int main(int argc, char *argv[]) {

    FILE *stream = stdin;  

    if (argc > 1) {
        stream = fopen(argv[1], "r");
        if (stream == NULL) {
            perror("Error opening file");
            return 1;
        }
    }

   

    init();
    
    phases(stream);

    if (stream != stdin) {
        fclose(stream);
    }

    return 0;
}