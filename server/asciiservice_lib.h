#ifndef ASCIISERVICELIB_H
#define ASCIISERVICELIB_H

#include <stdio.h>
#include <stdint.h>

enum Service{
    SERVICE_HELP,
    SERVICE_FONTS,
    SERVICE_GENERATE,
    SERVICE_SETTINGS

};

struct Service_Request {
    uint8_t username[64] = {'\0'};
    enum Service service = SERVICE_HELP;
    uint8_t* parameterlist[8] = {NULL};
};

struct Servide_Response{
    uint16_t size;
    uint8_t* response;
};

uint8_t parse_request(uint8_t* buffer,uint32_t size, struct Service_Request* req);

#endif