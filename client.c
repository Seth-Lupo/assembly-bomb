#include "ws_stream.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
// #include <sys/ptrace.h>

#include "ws_stream.h"

// int ANTI_DEBUG = 0;

char seed[7];
ws_stream* stream;
int debugger = 0;

// void anti_debug() {

//     if (ptrace(PTRACE_TRACEME, 0, NULL, 0) == -1) {
//         printf("I see you are using a debugger...\n");
//         printf("While using a debugger you cannot solve any phases, but you can still blow up!\n");
//         printf("This is to prevent any shenanigans that I couldn't account for.\n");
//         debugger = 1;
//     }
// }


void init() {
     

    // if (ANTI_DEBUG) anti_debug();
    stream = ws_stream_connect("108.49.20.247", false);
   
    if (!stream) {
        fprintf(stderr, "Failed to create WebSocket stream.\n");
        exit(1);
    }

    size_t total_bytes_read = 0;

    while (total_bytes_read < 7) {
        
        if (ws_stream_data_available(stream)) {
            ssize_t bytes_read = ws_stream_read(stream, seed + total_bytes_read, 7 - total_bytes_read);
            if (bytes_read <= 0) {
                fprintf(stderr, "Error or disconnection while reading data\n");
                ws_stream_close(stream);
                return;
            }
            total_bytes_read += bytes_read;
        } else {
            usleep(100000);
        }
    }

}

extern void explode() {

    int success = ws_stream_write_char(stream, seed[0]);
    if (!success) {
        printf("Terminate with connection error.");
        exit(1);
    }

    printf("KABOOM! YOU EXPLODED!!! WTF R U DOING!?!?!?!!?\n");
    printf("*** the class has been notified ***\n");

    exit(1);

}

extern void invalidate() {

    int success = ws_stream_write_char(stream, 0);
    if (!success) {
        printf("Terminate with connection error.");
        exit(0);
    }

    printf("You invalidated the code via tampering.\n");
    printf("*** the class has been notified ***\n");

    exit(1);

}

extern void pass_phase(int phase_number) {
    
    assert(1 <= phase_number && phase_number <= 6);
    
    if (debugger == 0) {
        int success = ws_stream_write_char(stream, seed[phase_number]);
        if (!success) {
            printf("Terminate with connection error.");
            exit(1);
        }
    } else {
        printf("With the debugger... ");
    }

    printf("You passed phased %d!\n", phase_number);

}