#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include "asciiservice_lib.h"
#include "services.h"

//build command
//gcc main.c asciiservice_lib.c services.c -o main -D_DEFAULT_SOURCE -lzmq

#ifdef __unix__
    #include <unistd.h>
    #include <time.h>

    static inline void Sleep(unsigned int milliseconds) {
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;  // Seconds
        ts.tv_nsec = (milliseconds % 1000) * 1000000; // Nanoseconds
        nanosleep(&ts, NULL);
    }
#else
    #include <windows.h>

#endif

void * context;

int main( int argc, char * argv[] )
{
	context = zmq_ctx_new();
    char prefix[] = "asciigenerator?>";


	void * sender = zmq_socket( context, ZMQ_PUSH );
	zmq_connect( sender, "tcp://benternet.pxl-ea-ict.be:24041" );

    void* receiver = zmq_socket(context, ZMQ_SUB);
    zmq_connect( receiver, "tcp://benternet.pxl-ea-ict.be:24042" );
    zmq_setsockopt(receiver,ZMQ_SUBSCRIBE,prefix,sizeof(prefix)-1);

    char anounce[] = "asciigenerator!>Ascii generator is running on the network";
	zmq_send( sender, anounce, sizeof(anounce), 0 );

    sleep(1);
    //Receiving requests
    while(1){
        char buffer[256] = {'\0'};
        int size = zmq_recv(receiver,buffer,sizeof(buffer),0);
        ServiceError result = SUCCESS;


        if(size<0){
            printf("Nothing received");
        }

        //Parse arguments
        struct Service_Request req = {
            .service = SERVICE_HELP,
            .username = {'\0'},
            .parameterlist = NULL,
            .settings = {
                .color = ANSI_RESET
            }
        };
        struct Service_Response res = {
            .response = NULL,
            .size = 0
        };
        result = parse_request(buffer,sizeof(buffer),&req);
        if(result!=SUCCESS){
            handle_error(result,&req,&res);
            send_and_clean_response(sender,&res);
            continue;
        }
        result = fetch_settings(req.username,&(req.settings));
        if(result!=SUCCESS){
            handle_error(result,&req,&res);
            send_and_clean_response(sender,&res);
            continue;
        }
        printf("Loaded user settings with color set to %d\n",req.settings.color);
        

        //Service handler
        ServiceError (*servicefunc[SERVICE_AMOUNT])(struct Service_Request*,struct Service_Response*) = {
            service_help,
            service_fonts,
            service_generate,
            service_settings,
            service_invalid
        };
        result = servicefunc[req.service](&req,&res);
        if(result!=SUCCESS){
            handle_error(result,&req,&res);
            send_and_clean_response(sender,&res);
            continue;
        }

        print_request(&req);

        send_and_clean_response(sender,&res);
        

        //free memory of parameterlist
        for(int i=0;i<8;i++){
            if(req.parameterlist[i]!=NULL){
                free(req.parameterlist[i]);
            }
            
        }


    }

	zmq_close( sender );
    zmq_close( receiver );
	zmq_ctx_shutdown( context ); //optional for cleaning lady order (get ready you l*zy f*ck)
	zmq_ctx_term( context ); //cleaning lady goes to work
	return 0;
}