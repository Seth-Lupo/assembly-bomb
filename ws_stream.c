#include "ws_stream.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define RX_BUFFER_SIZE 65536

struct ws_stream {
    struct lws_context* context;
    struct lws* wsi;
    pthread_t service_thread;
    char* rx_buffer;
    size_t rx_data_len;
    pthread_mutex_t rx_mutex;
    bool debug;
    bool connected;
    bool should_exit;
};

static void debug_log(ws_stream* stream, const char* msg) {
    if (stream->debug) {
        fprintf(stderr, "ws_stream: %s\n", msg);
    }
}

static int callback(struct lws* wsi, enum lws_callback_reasons reason,
                   void* user, void* in, size_t len) {
    ws_stream* stream = (ws_stream*)user;
    
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            stream->connected = true;
            debug_log(stream, "Connection established");
            break;
            
        case LWS_CALLBACK_CLIENT_RECEIVE:
            pthread_mutex_lock(&stream->rx_mutex);
            if (stream->rx_data_len + len <= RX_BUFFER_SIZE) {
                memcpy(stream->rx_buffer + stream->rx_data_len, in, len);
                stream->rx_data_len += len;
                debug_log(stream, "Data received");
            }
            pthread_mutex_unlock(&stream->rx_mutex);
            break;
            
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            debug_log(stream, "Connection error");
            stream->connected = false;
            break;
            
        case LWS_CALLBACK_CLOSED:
            debug_log(stream, "Connection closed");
            stream->connected = false;
            break;
            
        default:
            break;
    }
    
    return 0;
}

static void* service_thread(void* arg) {
    ws_stream* stream = (ws_stream*)arg;
    
    while (!stream->should_exit) {
        lws_service(stream->context, 50);
    }
    
    return NULL;
}

ws_stream* ws_stream_connect(const char* url, bool debug) {
    printf("*** connecting ***\n");
    usleep(1000000);

    lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

    ws_stream* stream = calloc(1, sizeof(ws_stream));
    if (!stream) return NULL;

    stream->debug = debug;
    stream->connected = false; // Initialize connection status
    stream->rx_buffer = malloc(RX_BUFFER_SIZE);
    if (!stream->rx_buffer) {
        free(stream);
        return NULL;
    }

    pthread_mutex_init(&stream->rx_mutex, NULL);

    struct lws_context_creation_info info = {0};
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = (struct lws_protocols[]) {
        { "ws-protocol", callback, sizeof(ws_stream), RX_BUFFER_SIZE },
        { NULL, NULL, 0, 0 }
    };
    info.gid = -1;
    info.uid = -1;

    stream->context = lws_create_context(&info);
    if (!stream->context) {
        debug_log(stream, "Failed to create context");
        goto cleanup_stream;
    }

    struct lws_client_connect_info connect_info = {0};
    connect_info.context = stream->context;
    connect_info.address = url;
    connect_info.port = 80;
    connect_info.path = "/ws";
    connect_info.protocol = "ws-protocol";
    connect_info.userdata = stream;

    stream->wsi = lws_client_connect_via_info(&connect_info);
    if (!stream->wsi) {
        debug_log(stream, "Failed to connect");
        goto cleanup_context;
    }

    // Wait for connection to be established or fail
    for (int i = 0; i < 10 && !stream->connected; i++) {
        lws_service(stream->context, 100);
        usleep(10000); // Small delay
    }

    if (!stream->connected) {  // Connection was not established
        debug_log(stream, "Connection timed out or failed");
        lws_context_destroy(stream->context);
        free(stream->rx_buffer);
        free(stream);
        return NULL;
    }

    pthread_create(&stream->service_thread, NULL, service_thread, stream);
    return stream;

cleanup_context:
    lws_context_destroy(stream->context);

cleanup_stream:
    free(stream->rx_buffer);
    free(stream);
    return NULL;
}



void ws_stream_close(ws_stream* stream) {
    if (!stream) return;
    
    stream->should_exit = true;
    pthread_join(stream->service_thread, NULL);
    
    if (stream->wsi) {
        lws_wsi_close(stream->wsi, LWS_CLOSE_STATUS_NORMAL);
    }
    if (stream->context) {
        lws_context_destroy(stream->context);
    }
    
    pthread_mutex_destroy(&stream->rx_mutex);
    free(stream->rx_buffer);
    free(stream);
}

ssize_t ws_stream_read(ws_stream* stream, void* buf, size_t len) {
    if (!stream || !buf || !len) return -1;
    
    pthread_mutex_lock(&stream->rx_mutex);
    size_t to_copy = len < stream->rx_data_len ? len : stream->rx_data_len;
    
    if (to_copy > 0) {
        memcpy(buf, stream->rx_buffer, to_copy);
        memmove(stream->rx_buffer, stream->rx_buffer + to_copy,
                stream->rx_data_len - to_copy);
        stream->rx_data_len -= to_copy;
    }
    
    pthread_mutex_unlock(&stream->rx_mutex);
    return to_copy;
}

ssize_t ws_stream_write(ws_stream* stream, const void* buf, size_t len) {
    if (!stream || !buf || !len) return -1;
    
    unsigned char* ws_buf = malloc(LWS_PRE + len);
    if (!ws_buf) return -1;
    
    memcpy(ws_buf + LWS_PRE, buf, len);
    int result = lws_write(stream->wsi, ws_buf + LWS_PRE, len, LWS_WRITE_TEXT);
    
    free(ws_buf);
    return result;
}

int ws_stream_write_char(ws_stream* stream, char c) {
    if (!stream) {
        return 0; // Return error if stream is NULL
    }

    ssize_t bytesWritten = ws_stream_write(stream, &c, 1);
    if (bytesWritten == 1) {
        return 1; // Successfully wrote one character
    } else {
        return 0; // Error occurred during write
    }
}

bool ws_stream_data_available(ws_stream* stream) {
    if (!stream) return false;
    
    pthread_mutex_lock(&stream->rx_mutex);
    bool available = stream->rx_data_len > 0;
    pthread_mutex_unlock(&stream->rx_mutex);
    
    return available;
}