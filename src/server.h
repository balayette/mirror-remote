#include <microhttpd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifndef SERVER_H
#define SERVER_H

int respond_conn(void *cls, struct MHD_Connection *connection, const char *url,
                 const char *method, const char *version,
                 const char *upload_data, size_t *upload_data_size,
                 void **con_cls);

#endif
