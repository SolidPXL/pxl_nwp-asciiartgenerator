#include "services.h"




ServiceError service_help(struct Service_Request* req, struct Service_Response* res){
    if(res->response!=NULL) return RESPONSE_NOT_EMPTY;

    char buffer[2048] = {'\0'};
    sprintf(buffer,"asciigenerator!>%s>Usage of the service:",req->username);

    res->size = strlen(buffer);
    res->response = malloc(res->size);
    memcpy(res->response,buffer,res->size);

    return SUCCESS;
}

ServiceError service_invalid(struct Service_Request* req, struct Service_Response* res){
    if(res->response!=NULL) return RESPONSE_NOT_EMPTY;

    char buffer[2048] = {'\0'};
    sprintf(buffer,"asciigenerator!>%s>You have requested the %s service, sadly this is still unavailable",req->username,service_to_string(req->service));

    res->size = strlen(buffer);
    res->response = malloc(res->size);
    memcpy(res->response,buffer,res->size);

    return SUCCESS;
}

