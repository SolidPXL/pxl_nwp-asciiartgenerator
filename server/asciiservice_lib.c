#include "asciiservice_lib.h"
#include <dirent.h>
#include <stdlib.h>


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

void remove_extension(char *filename) {
    char *dot = strrchr(filename, '.');
    if (dot && dot != filename) {
        *dot = '\0'; // Remove extension
    }
}

uint16_t strcpy_stashed(char* buf,char* src,uint16_t start,uint16_t max){
    //join all strings seperated by \0
    int i = start;
    int j = 0;

    while (i<max)
    {
        buf[i] = src[j];
        i++;
        j++;
        if(src[j-1] == '\0') break;
    }
    return i;
}

uint8_t** get_fonts(){
    const char *folder_path = "./fonts";
    struct dirent *entry;
    DIR *dir = opendir(folder_path);

    if (!dir) {
        perror("opendir");
        return NULL;
    }

    char name_buffer[1024] = {'\0'};
    int entries=0;
    int l=0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            char filename[256];
            strncpy(filename, entry->d_name, sizeof(filename));
            filename[sizeof(filename) - 1] = '\0'; // Ensure null-termination

            remove_extension(filename);
            l = strcpy_stashed(name_buffer,filename,l,sizeof(name_buffer)-1);
            entries++;
            
            //printf("%s\n", filename);
        }
    }

    if(entries==0) {
        closedir(dir);
        return NULL;
    }

    //copy name buffer with an exact size
    uint8_t* str = malloc(l);
    memcpy(str,name_buffer,l);

    //create array with pointers to start of filenames. last entry being NULL
    uint8_t** ptrarr = malloc(entries*sizeof(uint8_t*)+1);
    int i = 0;
    int e = 1;
    ptrarr[0] = str;
    while (e<entries)
    {
        if(str[i]=='\0'){
            ptrarr[e] = &(str[i+1]);
            e++;
        }
        i++;
    }
    ptrarr[entries] = NULL;
    
    //Note to self: dont forget to first free the pointer at[0] before freeing the pointer itself
    

    closedir(dir);
    return ptrarr;
}

uint16_t get_fony_offset(uint8_t* fontheights, uint8_t size){
    uint16_t res = 0;
    for(int i=0;i<16;i++){
        if(i==size-1)break;
        res += fontheights[i]+1;
    }

    return res;
}

uint16_t get_char_position(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    } else {
        return -1; // Invalid character
    }
}

uint16_t getLineNum(char character, uint16_t lines_per_font){
    uint16_t res = 1;
    res += (get_char_position(character) * lines_per_font);

    return res;
}

uint32_t buffer_length(uint8_t* buf, uint16_t max){
    uint32_t res = 0;
    while(buf[res]!='\0'&&res<max){
        res++;
    }

    return res+1;
}

void setLineStart(uint16_t linenum,FILE* file){
    rewind(file);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int current_line = 1;

    while ((read = getline(&line, &len, file)) != -1) {
        if (current_line == linenum) {
            break;
        }
        current_line++;
    }

    free(line);

}

void copy_string(uint8_t* dst, uint8_t* src){
    int i = 0;
    while(dst[i] != '\0'){  
        i++;
    }
    int j=0;
    while(src[j] != '\0' && src[j] != '\n' && src[j] != '\r'){
        dst[i] = src[j];
        j++;
        i++;
        // printf("j %d i %d\n",j,i);
    }
}

uint8_t* generate_text(uint8_t* font, uint8_t* text,uint8_t size ,uint8_t spacing){
    uint8_t fontpath[64];
    strcpy(fontpath,"./fonts/");
    strcat(fontpath,font);
    strcat(fontpath,".txt");
    FILE *fp = fopen(fontpath, "r");
    if (!fp) {
        return NULL;
    }

    //Parse font header
    char* header_buf = NULL;
    size_t len = 0;
    getline(&header_buf,&len,fp);

    uint8_t fontheights[16] = {0};//Assuming there are not more than 16 fonts height variations not exceeding 32 characters high
    uint16_t lines_per_font = 0;
    Generator_Overlap overlap = NOOVERLAP;
    
    uint8_t* tok = strtok(header_buf,";");
    tok = strtok(NULL,";");

    //parse overlap
    if(tok == NULL){
        return NULL;
    }
    overlap = atoi(tok);

    //parse fontsizes
    tok = strtok(header_buf, ":");
    {
        int i=0;
        while(tok!=NULL){
            if(i>=16) break;
            fontheights[i] = atoi(tok);
            i++;
            
            tok = strtok(NULL,":");
        }  
    }

    for(int i=0;i<16;i++){
        if(fontheights[i]==0) break;
        lines_per_font += fontheights[i]+1;
    }


    //Create line buffers for generator
    uint8_t fontheight = fontheights[size-1];
    char** lines = malloc(fontheight*sizeof(uint8_t*));
    for(int i=0;i<fontheight;i++){
        lines[i] = malloc(MAX_LINE_WIDTH);
        memset(lines[i],'\0',MAX_LINE_WIDTH);
    }


    //Run font lookups
    {
        char spacings[64] = {'\0'};
        for(int j=0;j<spacing;j++){
            spacings[j]=' ';
        }

        int i=0;
        while(text[i]!='\0'){
            char* char_header_buf = NULL;
            //char* line_buf = NULL;
            size_t len = 0;
            //set line to line before character header
            setLineStart(getLineNum(text[i],lines_per_font)+get_fony_offset(fontheights,size),fp);

            //read character information
            char currentchar;
            uint8_t height;
            uint8_t width;
            uint8_t overlap_prio;
            uint8_t right_overlap;
            getline(&char_header_buf,&len,fp);
            char* tok = strtok(char_header_buf,";");
            currentchar = tok[0];
            tok = strtok(NULL,";");
            height = atoi(tok);
            tok = strtok(NULL,";");
            width = atoi(tok);
            tok = strtok(NULL,";");
            overlap_prio = atoi(tok);
            tok = strtok(NULL,";");
            right_overlap = atoi(tok);
            
            //read character
            char* line_buf = NULL;
            for(int i=0;i<fontheight;i++){
                getline(&line_buf,&len,fp);

                copy_string(lines[i],line_buf);
                copy_string(lines[i],spacings);
            }
            

            i++;
        }
    }

    // for(int i=0;i<fontheight;i++){
    //     printf("%s\n",lines[i]);
    // }

    // printf("\n");

    //stitch buffer
    uint16_t temp[32] = {0};
    uint32_t total_buf = 0;
    for(int i=0;i<fontheight;i++){
        temp[i] = buffer_length(lines[i],MAX_LINE_WIDTH);
        lines[i][temp[i]-1] = '\n';
        lines[i][temp[i]] = '\r';
        total_buf += temp[i];
    }

    uint8_t* returnbuf = malloc(total_buf);
    uint32_t compound = 0;
    for(int i=0;i<fontheight;i++){
        memcpy(returnbuf + compound,lines[i],temp[i]);
        compound += temp[i];
        free(lines[i]);
    }

    // for(int i=0;i<fontheight;i++){
    //     printf("%s\n",lines[i]);
    // }
    printf("%s\n-----",returnbuf);

    //clean bottom gunk
    int linecounter = 0;
    int j=0;
    while(j<total_buf){
        if(returnbuf[j]=='\r'||returnbuf[j]=='\n'){
            linecounter++;
        }
        if(linecounter>=(fontheight)){
            returnbuf[j]='\0';
        }
        j++;
    }




    //return result
    fclose(fp);
    return returnbuf;

    
};
