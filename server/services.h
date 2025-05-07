#ifndef SERVICES_H
#define SERVICES_H

#include "asciiservice_lib.h"

ServiceError service_help(struct Service_Request* req, struct Service_Response* res);
ServiceError service_fonts(struct Service_Request* req, struct Service_Response* res);
ServiceError service_generate(struct Service_Request* req, struct Service_Response* res);
ServiceError service_settings(struct Service_Request* req, struct Service_Response* res);
ServiceError service_invalid(struct Service_Request* req, struct Service_Response* res);


#endif