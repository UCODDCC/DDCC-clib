#include <stdio.h>

#include "libddcd.h"


int main() {
    int retval;
    int response_len = 0;
    char* response = (char*)malloc(sizeof(char));
    char* request = "hello,, world";
    retval = ddcdSendMessageToRegion(
            "spain",
            request,
            (int)strlen(request),
            &response,
            &response_len
    );
    printf("retval: %i, response from main {%s}%lu\n", retval, response, strlen(response));
    return 0;
}