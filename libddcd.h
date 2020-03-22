#ifndef _DDCD_CLIB_
#define _DDCD_CLIB_

#define DDCD_PORT 7654
#define DDCD_ADDRESS "ucoddcd.xyz"
#define DDCD_BUFF_SIZE 2048

#define DDCD_OK 0
#define DDCD_SOCKET_ERROR 1000
#define DDCD_CONNECT_ERROR 2000
#define DDCD_SEND_ERROR 3000
#define DDCD_READ_ERROR 4000

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <stdio.h>

const char* ddcdGetBestRegion() {
    return "spain";
}

/**
 * @warning This is a blocking function
 * Sends a command to the server and waits for the response
 *
 * @param region region sub-domain example: spain, france, italy, canada...
 *          your region might not be active!
 *          please check the project main page to see the active regions
 * @param request payload for the regional master node
 * @param request_len length of the payload
 * @param response pointer to initialized dynamic memory char array
 * @param response_len pointer to future container of response length
 * @return error code xyyy where x is the type or error and yyy is the specific error
 *          1 series error is refereed to socket errors      (socket  unix function)
 *          2 series error is refereed to connection errors  (connect unix function)
 *          3 series error is refereed to sending errors     (send    unix function)
 *          4 series error is refereed to receiving errors   (read    unix function)
 *
 *          yyy errors should be checked on the linux manual of each function
 */
int ddcdSendMessageToRegion(
        const char* region,
        const char* request,
        const int request_len,
        char** response,
        int* response_len
) {
    // variables initialization
    int socket_fd, retval, nbytes;
    char full_address[256];
    char buffer[DDCD_BUFF_SIZE];
    char* formatted_request;
    struct sockaddr_in server_dir;

    // initializes the socket and check for errors
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        return DDCD_SOCKET_ERROR + errno;

    // prepares the full server address based on sub-domain and domain
    bzero(full_address, 256);
    bzero((char*)&server_dir, sizeof(server_dir));
    strcpy(full_address, region);
    strcat(full_address, ".");
    strcat(full_address, DDCD_ADDRESS);
    strcat(full_address, "\0");

    // builds the sockaddr_in struct containing the necessary information to identify the server
    server_dir.sin_family = AF_INET;
    #ifdef LOCAL
        server_dir.sin_addr.s_addr = inet_addr("127.0.0.1");
    #else
        server_dir.sin_addr.s_addr = inet_addr(full_address);
    #endif
    server_dir.sin_port = htons(DDCD_PORT);

    // connects tho the server
    retval = connect(
            socket_fd,
            (const struct sockaddr*) &server_dir,
            sizeof(server_dir)
    );
    if (retval < 0)
        return DDCD_CONNECT_ERROR + errno;

    // prepare the formatted request, this should follow the next sintax
    // {+,-}[control message]<[payload]>
    // the + indicates all is ok, normally the control message is not needed here
    // the - indicates the presence of errors, they should be indicated on the control message field
    // as we are sending the first message, an error is not expected here, there should be no control message.
    formatted_request = (char*)malloc(sizeof(char) * (request_len + 3));
    strcpy(formatted_request, "+<");
    strcat(formatted_request, request);
    strcat(formatted_request, ">");

    #ifdef DEBUG
        printf("sending request to {%s}\n", full_address);
        printf("request {%s}%i\n", formatted_request, request_len);
    #endif
    // sends the formatted payload to the server
    nbytes = send(
            socket_fd,
            formatted_request,
            request_len + 3,
            0
    );
    // free the formatted payload memory
    free(formatted_request);
    // check for errors in the send
    if (nbytes < 0)
        return DDCD_SEND_ERROR + errno;

    *response_len = 0;
    bzero(*response, strlen(*response));
    do {
        // read from the socket file desriptor
        bzero(buffer, DDCD_BUFF_SIZE);
        nbytes = read(
                socket_fd,
                buffer,
                DDCD_BUFF_SIZE
        );
        // add the read bytes to the response length
        (*response_len) += nbytes;
        #ifdef DEBUG
            printf("resizing response to %i\n", *response_len);
        #endif
        // realocate the memory with the new size
        (*response) = (char*)realloc((*response), sizeof(char) * (*response_len));
        // copy the newly read buffer into the final response
        // can't use strcat Using it is running off the end of the array and corrupting the data structures
        // used by realloc
        memcpy((char*)*response + (*response_len) - nbytes, buffer, nbytes);
        #ifdef DEBUG
            printf("currently read {%s}%lu\n", *response, strlen(*response));
        #endif
    } while (buffer[strlen(buffer)-1]!='>'); // this indicates the end of payload
    #ifdef DEBUG
        printf("full response {%s}%lu\n", *response, strlen(*response));
    #endif
    // checks for errors
    if (nbytes < 0)
        return DDCD_READ_ERROR + errno;

    // return the ok status (0)
    return DDCD_OK;
}


#endif
