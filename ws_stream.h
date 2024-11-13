#ifndef WS_STREAM_H
#define WS_STREAM_H

#include <libwebsockets.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct ws_stream ws_stream;

// Creates a new WebSocket stream connection
ws_stream* ws_stream_connect(const char* url, bool debug);

// Closes the WebSocket stream and frees resources
void ws_stream_close(ws_stream* stream);

// Reads data from the WebSocket stream
// Returns number of bytes read or -1 on error
ssize_t ws_stream_read(ws_stream* stream, void* buf, size_t len);

// Writes data to the WebSocket stream
// Returns number of bytes written or -1 on error
ssize_t ws_stream_write(ws_stream* stream, const void* buf, size_t len);

int ws_stream_write_char(ws_stream* stream, char c);

// Returns true if there is data available to read
bool ws_stream_data_available(ws_stream* stream);



#endif // WS_STREAM_H