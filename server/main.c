#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>

//build command
//gcc 

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

        char username[64] = {'\0'};
        char service[64] = {'\0'};
        //First item in parameterlist is argument since some services have no argument
        char* parameterlist[8] = {NULL};

        if(size<0){
            printf("Nothing received");
        }

        //Parse arguments
        char * tok;
        tok = strtok (buffer,">");
        tok = strtok (NULL,">");
        strcpy(username,tok);
        tok = strtok (NULL, ">");
        strcpy(service,tok);
        tok = strtok (NULL, ">");
        int i=0;
        while(tok!=NULL){
            printf("loop tok\n");
            char* parameter = malloc(64);
            strcpy(parameter,tok);
            parameterlist[i] = parameter;

            tok = strtok (NULL, ">");
            i++;

        }
        char responsebuf[480] = {'\0'};
        sprintf(responsebuf,"asciigenerator!>%s>You have requested the %s service, sadly this is still unavailable",username,service);

        zmq_send(sender,responsebuf, sizeof(responsebuf),0);
        

        //free memory of parameterlist
        for(int i=0;i<8;i++){
            if(parameterlist[i]!=NULL){
                free(parameterlist[i]);
            }
            
        }

        //return 0;

    }

	zmq_close( sender );
    zmq_close( receiver );
	zmq_ctx_shutdown( context ); //optional for cleaning lady order (get ready you l*zy f*ck)
	zmq_ctx_term( context ); //cleaning lady goes to work
	return 0;
}