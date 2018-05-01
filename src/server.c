#include "server.h"
#include <stdio.h>
#include <string.h>

int respond_conn(void *cls, struct MHD_Connection *connection, const char *url,
                 const char *method, const char *version,
                 const char *upload_data, size_t *upload_data_size,
                 void **con_cls) {

    (void)cls;
    (void)con_cls;

    printf("Url : %s | Method : %s | Version : %s | Data : %s | Size : %lu\n",
           url, method, version, upload_data, *upload_data_size);

    struct MHD_Response *resp = MHD_create_response_from_buffer(
        strlen("Hello, World!"), (void *)"Hello, World!",
        MHD_RESPMEM_PERSISTENT);

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, resp);
    MHD_destroy_response(resp);

    return ret;
}
