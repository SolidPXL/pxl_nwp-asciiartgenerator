#ifndef ASCIISERVICELIB_H
#define ASCIISERVICELIB_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <zmq.h>

#include <dirent.h>
//TODO add windows conversions for fileIO

typedef enum Service Service;
enum Service{
    SERVICE_HELP,
    SERVICE_FONTS,
    SERVICE_GENERATE,
    SERVICE_SETTINGS,
    SERVICE_INVALID,
    SERVICE_AMOUNT
};

typedef enum Service_Error ServiceError;
enum Service_Error{
    SUCCESS,
    NOT_ENOUGH_ARGUMENTS,
    WRONG_ARUMENTS,
    RESPONSE_NOT_EMPTY
};

struct Service_Request {
    uint8_t username[64];
    enum Service service;
    uint8_t* parameterlist[8];
};

struct Service_Response{
    uint16_t size;
    uint8_t* response;
};


Service parse_service(char* buf);
char* service_to_string(Service service);

ServiceError parse_request(uint8_t* buffer,uint32_t size, struct Service_Request* req);

void handle_error(ServiceError error, struct Service_Request* req, struct Service_Response* res);
void send_response(void* socket, struct Service_Response*);
void send_and_clean_response(void* socket, struct Service_Response*);
void clean_req(struct Service_Request* req);
void clean_res(struct Service_Response* req);

void print_request(struct Service_Request* req);

uint8_t** get_fonts();

#endif