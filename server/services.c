#include "services.h"




ServiceError service_help(struct Service_Request* req, struct Service_Response* res){
    if(res->response!=NULL) return RESPONSE_NOT_EMPTY;

    char buffer[2048] = {'\0'};
    sprintf(buffer,
        "asciigenerator!>%s>Usage of the service:\n"
        "Follow the syntax: asciigenerator?>[username]>[service]>[argument]>[parameters]\n"
        "Available services:\n"
        "- help: shows this screen\n"
        "  usage: >help\n"
        "- fonts: shows all available fonts\n"
        "  usage: >fonts\n"
        "- generate: turn text into ascii\n"
        "  usage: >generate>[font]>[optional parameters]>[text]\n"
        "- settings: change user specific settings\n"
        "  usage: >settings>[setting]>[value]\n"
        "More info available on github https://github.com/SolidPXL/pxl_nwp-asciiartgenerator",
        req->username
    );

    res->size = strlen(buffer);
    res->response = malloc(res->size);
    memcpy(res->response,buffer,res->size);

    return SUCCESS;
}

ServiceError service_fonts(struct Service_Request* req, struct Service_Response* res){
    if(res->response!=NULL) return RESPONSE_NOT_EMPTY;

    char buffer[2048] = {'\0'};
    sprintf(buffer,"asciigenerator!>%s>currently the following fonts are available:\n",req->username);

    uint8_t** fonts = get_fonts();
    
    int i=0;
    while (fonts[i]!=NULL)
    {
        char temp[124] = {'\n'};
        sprintf(temp,"- %s\n",fonts[i]);
        strcat(buffer,temp);
        i++;
    }

    res->size = strlen(buffer);
    res->response = malloc(res->size);
    memcpy(res->response,buffer,res->size);

    free(fonts[0]);
    free(fonts);

    return SUCCESS;
}

ServiceError service_generate(struct Service_Request* req, struct Service_Response* res){
    if(res->response!=NULL) return RESPONSE_NOT_EMPTY;

    char buffer[2048] = {'\0'};
    sprintf(buffer,"asciigenerator!>%s>\n",req->username);

    uint8_t* text = generate_text(req->parameterlist[0],req->parameterlist[2],atoi(req->parameterlist[1]),1);
    if(text==NULL){
        return OTHER_ERROR;
    }
    strcat(buffer,text);
    // uint16_t len = strlen(buffer);
    // buffer[len-2]='\0';

    res->size = strlen(buffer);
    res->response = malloc(res->size);
    memcpy(res->response,buffer,res->size);

    free(text);

    return SUCCESS;
}

ServiceError service_settings(struct Service_Request* req, struct Service_Response* res){
    if(res->response!=NULL) return RESPONSE_NOT_EMPTY;

    char buffer[2048] = {'\0'};
    sprintf(buffer,"asciigenerator!>%s>You have requested the %s service, sadly this is still unavailable",req->username,service_to_string(req->service));

    SettingKey key = parse_settingkey(req->parameterlist[0]);
    if(key == SETTING_INVALID){
        return WRONG_ARUMENTS;
    }
    
    ServiceError result = set_settings(req->username,key,req->parameterlist[1]);
    if(result!=SUCCESS){
        return result;
    }

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



