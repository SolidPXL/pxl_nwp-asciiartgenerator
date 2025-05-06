#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define SEND_PORT "tcp://benternet.pxl-ea-ict.be:24041"
#define RECEIVE_PORT "tcp://benternet.pxl-ea-ict.be:24042"

void *send_messages(void *context) {
    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sender, SEND_PORT);
    
    char buffer[256];
    while (1) {
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        zmq_send(sender, buffer, strlen(buffer), 0);
    }

    zmq_close(sender);
    return NULL;
}

char prefix[] = "asciigenerator!>";

void *receive_messages(void *context) {
    void *receiver = zmq_socket(context, ZMQ_SUB);
    zmq_connect( receiver, "tcp://benternet.pxl-ea-ict.be:24042" );
    zmq_setsockopt(receiver,ZMQ_SUBSCRIBE,prefix,sizeof(prefix)-1);
    //zmq_bind(receiver, RECEIVE_PORT);

    char buffer[256];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        zmq_recv(receiver, buffer, sizeof(buffer) - 1, 0);
        printf("\nReceived: %s\n> ", buffer);
        fflush(stdout);
    }

    zmq_close(receiver);
    return NULL;
}

int main() {
    void *context = zmq_ctx_new();
    pthread_t send_thread, recv_thread;

    pthread_create(&recv_thread, NULL, receive_messages, context);
    pthread_create(&send_thread, NULL, send_messages, context);

    pthread_join(send_thread, NULL);
    pthread_cancel(recv_thread);

    zmq_ctx_destroy(context);
    return 0;
}