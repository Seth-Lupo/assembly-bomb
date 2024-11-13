gcc -o bomb main.c ws_stream.c client.c phases.c -no-pie /usr/lib/x86_64-linux-gnu/libwebsockets.a /usr/lib/x86_64-linux-gnu/libssl.a /usr/lib/x86_64-linux-gnu/libcrypto.a /usr/lib/x86_64-linux-gnu/libz.a /usr/lib/x86_64-linux-gnu/libpthread.a /usr/lib/x86_64-linux-gnu/libm.a /usr/lib/x86_64-linux-gnu/libcap.a -static


# gcc -o bomb main.c ws_stream.c client.c phases.c -no-pie ls /usr/local/lib/libwebsockets.a ls /usr/local/lib/libssl.a ls /usr/local/lib/libcrypto.a -lz -lpthread -lm -lcap
# gcc -o bomb main.c ws_stream.c client.c phases.c -no-pie ls /usr/local/lib/libwebsockets.a ls -lssl.a -lcrypto.a -lz -lpthread -lm -lcap

