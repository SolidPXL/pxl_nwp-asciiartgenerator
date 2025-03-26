#include <stdio.h>
#include <zmq.h>
#include <windows.h>
#include <string.h>

void * context; //Global context, because you only need one !

int main( int argc, char * argv[] )
{
	context = zmq_ctx_new();
    char sendtopic[] = "example>quest?>Axel>";
    char rectopic[] = "example>quest!>Axel>";

	void * pusher = zmq_socket( context, ZMQ_PUSH );
	zmq_connect( pusher, "tcp://benternet.pxl-ea-ict.be:24041" );

    void* subber = zmq_socket(context, ZMQ_SUB);
    zmq_connect( subber, "tcp://benternet.pxl-ea-ict.be:24042" );
    zmq_setsockopt(subber,ZMQ_SUBSCRIBE,rectopic,sizeof(rectopic)-1);

    
    
	zmq_send( pusher, sendtopic, sizeof(sendtopic), 0 );
    Sleep(500);

    char buffer[256] = {'\0'};
    int size = zmq_recv(subber,buffer,sizeof(buffer),0);
    if(size<0){
        printf("Nothing received");
    }
    buffer[256] = '\0';
    printf("buffer: %s\n",buffer);

    //parse message and topic
    char quest_topic[64];
    char quest_message[256];

    char * tok;
    tok = strtok (buffer,"\"");
    int i = 0;
    while (tok != NULL)
    {
        if(i==1){
            strcpy(quest_topic,tok);

        } else if(i==3){
            strcpy(quest_message,tok);
        }
        tok = strtok (NULL, "\"");
        i++;
    }

    void* quester = zmq_socket(context, ZMQ_SUB);
    zmq_connect( quester, "tcp://benternet.pxl-ea-ict.be:24042" );
    zmq_setsockopt(quester,ZMQ_SUBSCRIBE,quest_topic,strlen(quest_topic));

    zmq_send(pusher,quest_message,strlen(quest_message),0);

    Sleep(400);

    char questbuf[256] = {'\0'};
    size = zmq_recv(quester,questbuf,sizeof(questbuf),0);
    if(size<0){
        printf("Nothing received");
    }
    buffer[256] = '\0';
    printf("buffer: %s\n",questbuf);


	zmq_close( pusher );
    zmq_close( subber );
    zmq_close( quester );
	zmq_ctx_shutdown( context ); //optional for cleaning lady order (get ready you l*zy f*ck)
	zmq_ctx_term( context ); //cleaning lady goes to work
	return 0;
}