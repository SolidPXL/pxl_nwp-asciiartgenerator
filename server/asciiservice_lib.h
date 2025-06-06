#ifndef ASCIISERVICELIB_H
#define ASCIISERVICELIB_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <zmq.h>

#include <dirent.h>
//TODO add windows conversions for fileIO
#define MAX_LINE_WIDTH 256

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
    TOO_MANY_ARGUMENTS,
    WRONG_ARUMENTS,
    RESPONSE_NOT_EMPTY,
    FONT_NOT_FOUND,
    FILESYSTEM_ERROR,
    OTHER_ERROR
};

typedef enum ANSI_Color ANSIColor;
enum ANSI_Color{
    ANSI_RESET,
    ANSI_BLACK,
    ANSI_RED,
    ANSI_GREEN,
    ANSI_YELLOW,
    ANSI_BLUE,
    ANSI_PURPLE,
    ANSI_CYAN,
    ANSI_WHITE
};

typedef enum Setting_Key SettingKey;
enum Setting_Key{
    SETTING_INVALID,
    SETTING_COLOR
};


typedef enum Generator_Overlap GeneratorOverlap;
enum Generator_Overlap{
    NOOVERLAP,
    LEFTONTOP,
    RIGHTONTOP,
    PRIORITY
};

struct User_Settings{
    ANSIColor color;
};

struct Service_Request {
    uint8_t username[64];
    enum Service service;
    uint8_t* parameterlist[8];
    struct User_Settings settings;
};

struct Service_Response{
    uint16_t size;
    uint8_t* response;
};

Service parse_service(char* buf);
char* service_to_string(Service service);

ServiceError parse_request(uint8_t* buffer,uint32_t size, struct Service_Request* req);
ServiceError fetch_settings(char* user, struct User_Settings* settings);

void handle_error(ServiceError error, struct Service_Request* req, struct Service_Response* res);
void send_response(void* socket, struct Service_Response*);
void send_and_clean_response(void* socket, struct Service_Response*);
void clean_req(struct Service_Request* req);
void clean_res(struct Service_Response* req);

void print_request(struct Service_Request* req);

void remove_extension(char *filename);
uint8_t** get_fonts();

uint8_t* generate_text(uint8_t* font, uint8_t* text,uint8_t size,uint8_t spacing);

SettingKey parse_settingkey(char* str);
ANSIColor parse_color(char* value);
const char* ansi_color_to_str(ANSIColor color);

ServiceError set_settings(struct User_Settings* settings,char* user,SettingKey key, char* value);

#endif