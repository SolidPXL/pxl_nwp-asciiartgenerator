#include "asciiservice_lib.h"


Service parse_service(char* buf){
    if(strcmp(buf,"generate")==0){
        return SERVICE_GENERATE;
    }
    if(strcmp(buf,"fonts")==0){
        return SERVICE_FONTS;
    }
    if(strcmp(buf,"settings")==0){
        return SERVICE_SETTINGS;
    }
    return SERVICE_HELP;
}

char* service_to_string(Service service){
    switch (service)
    {
        case SERVICE_FONTS:{
            return "FONTS";
        }
        case SERVICE_GENERATE:{
            return "GENERATE";
        }
        case SERVICE_HELP:{
            return "HELP";
        }
        case SERVICE_SETTINGS:{
            return "SETTINGS";
        }
        default:{
            return "invalid service";
        }
    }
}

ServiceError parse_request(uint8_t* buffer,uint32_t size, struct Service_Request* req){
    char * tok;
    tok = strtok (buffer,">");
    tok = strtok (NULL,">");
    if(tok == NULL){
        return NOT_ENOUGH_ARGUMENTS;
    }
    strcpy(req->username,tok);
    tok = strtok (NULL, ">");
    if(tok == NULL){
        return NOT_ENOUGH_ARGUMENTS;
    }
    req->service=parse_service(tok);
    tok = strtok (NULL, ">");
    int i=0;
    while(tok!=NULL){
        char* parameter = malloc(64);
        strcpy(parameter,tok);
        req->parameterlist[i] = parameter;

        tok = strtok (NULL, ">");
        i++;

    }
}

void handle_error(ServiceError error, struct Service_Request* req, struct Service_Response* res){
    if(res->response!=NULL) return;
    char buffer[2048] = {'\0'};
    switch (error)
    {
        case NOT_ENOUGH_ARGUMENTS: {
            sprintf(buffer,"asciigenerator!>%s>Not enough arguments were provided.",req->username);
            break;
        }
        default: {
            sprintf(buffer,"asciigenerator!>%s>Something went wrong, did you follow the right request structure?",req->username);
            break;
        }
    }
    res->size = strlen(buffer);
    res->response = malloc(res->size);
    memcpy(res->response,buffer,res->size);

    clean_req(req);
};

void send_response(void* socket, struct Service_Response* res){
    zmq_send(socket,res->response, res->size,0);
};

void send_and_clean_response(void* socket, struct Service_Response* res){
    send_response(socket,res);
    clean_res(res);
}

void clean_req(struct Service_Request* req){
    //free memory of parameterlist
    for(int i=0;i<8;i++){
        if(req->parameterlist[i]!=NULL){
            free(req->parameterlist[i]);
        }
        
    }
};

void clean_res(struct Service_Response* res){
    if(res->response != NULL){
        free(res->response);
    }
    res->size = 0;
}

void print_request(struct Service_Request* req){
    printf("%s requested \"%s\"\n",req->username,service_to_string(req->service));
    for(int i=0;i<8;i++){
        if(req->parameterlist[i]==NULL) break;
        printf("param %d : %s\n",i,req->parameterlist[i]);
    }
}