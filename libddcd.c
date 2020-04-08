#include <libddcd.h>

const char* ddcdGetBestRegion() {
    return "spain";
}


int ddccAppendToBuffer(char** buffer, unsigned int* buffer_current_size, unsigned int* buffer_max_size, const char* additional_buffer, unsigned int additional_buffer_length) {
    if ((*buffer_current_size) + additional_buffer_length > (*buffer_max_size)) {
        (*buffer_max_size) += DDCD_RESERVE_MEMORY_EACH;
        (*buffer) = (char*)realloc((*buffer), sizeof(char) * (*buffer_max_size));
    }
    memcpy((char*)(*buffer) + (*buffer_current_size), additional_buffer, additional_buffer_length);
    (*buffer_current_size) += additional_buffer_length;
    return 0;
}

int ddccSendMessageToRegion(
        const char* region,
        const char* request,
        unsigned int request_len,
        char** response,
        unsigned int* response_len
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
        printf("sending request to \"%s\"\n", full_address);
        printf("request \"%s\"%i\n", formatted_request, request_len);
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
        // checks for errors
        if (nbytes < 0)
            return DDCD_READ_ERROR + errno;
        // add the read bytes to the response length
        (*response_len) += nbytes;
        #ifdef DEBUG
            printf("resizing response to %i\n", *response_len);
        #endif
        // reallocate the memory with the new size
        (*response) = (char*)realloc((*response), sizeof(char) * (*response_len));
        // copy the newly read buffer into the final response
        // can't use strcat Using it is running off the end of the array and corrupting the data structures
        // used by realloc
        // start to copy at the start of the newly reallocated memory
        memcpy((char*)*response + (*response_len) - nbytes, buffer, nbytes);
        #ifdef DEBUG
            printf("currently read \"%s\"%lu\n", *response, strlen(*response));
        #endif
    } while (buffer[strlen(buffer)-1]!='>'); // this indicates the end of payload
    #ifdef DEBUG
        printf("full response \"%s\"%lu\n", *response, strlen(*response));
    #endif

    // return the ok status (0)
    return DDCD_OK;
}



int ddccMatrixMultiplication(float** Ma, int size_ax, int size_ay, float** Mb, int size_bx, int size_by, float*** result) {
    char *opcode = "matrix", *subopcode = "multiplication", *payload, *response, buffer[256];
    unsigned int payload_current_size = 0, response_len = 0, payload_max_size = DDCD_RESERVE_MEMORY_EACH;
    payload = (char*)malloc(sizeof(char) * DDCD_RESERVE_MEMORY_EACH);
    response = (char*)malloc(sizeof(char));

    // operation field
    bzero(buffer, 256); sprintf((char*)buffer, "[%s|%s|%dx%d,%dx%d]{", opcode, subopcode, size_ax, size_ay, size_bx, size_by);
    ddccAppendToBuffer(&payload, &payload_current_size, &payload_max_size, buffer, strlen(buffer));
    // write the data
    for (int x = 0; x < size_ax; ++x) {
        for (int y = 0; y < size_ay; ++y) {
            bzero(buffer, 256); sprintf((char*)buffer, "%f,", Ma[x][y]);
            ddccAppendToBuffer(&payload, &payload_current_size, &payload_max_size, buffer, strlen(buffer));
        }
    }
    for (int x = 0; x < size_ay; ++x) {
        for (int y = 0; y < size_ax; ++y) {
            bzero(buffer, 256); sprintf((char*)buffer, "%f,", Mb[x][y]);
            ddccAppendToBuffer(&payload, &payload_current_size, &payload_max_size, buffer, strlen(buffer));
        }
    }
    memcpy((char*)payload + payload_current_size -1 , "}", 1); // overwrites the last comma
    return ddccSendMessageToRegion(
            "spain",
            payload,
            payload_current_size,
            &response,
            &response_len
    );
}


int ddccVectorAddition(float* Va, int size_a, float* Vb, int size_b, float** result) {
    char *opcode = "vector", *subopcode = "addition", *payload, *response, buffer[256];
    unsigned int payload_current_size = 0, response_len = 0, payload_max_size = DDCD_RESERVE_MEMORY_EACH;
    payload = (char*)malloc(sizeof(char) * DDCD_RESERVE_MEMORY_EACH);
    response = (char*)malloc(sizeof(char));

    // operation field
    bzero(buffer, 256); sprintf((char*)buffer, "[%s|%s|%d,%d]{", opcode, subopcode, size_a, size_b);
    ddccAppendToBuffer(&payload, &payload_current_size, &payload_max_size, buffer, strlen(buffer));
    //data
    for (int i = 0; i < size_a; ++i) {
        bzero(buffer, 256); sprintf((char*)buffer, "%f,", Va[i]);
        ddccAppendToBuffer(&payload, &payload_current_size, &payload_max_size, buffer, strlen(buffer));
    }
    for (int i = 0; i < size_b; ++i) {
        bzero(buffer, 256); sprintf((char*)buffer, "%f,", Vb[i]);
        ddccAppendToBuffer(&payload, &payload_current_size, &payload_max_size, buffer, strlen(buffer));
    }
    // overwrites the last comma with data finalization marker
    memcpy((char*)payload + payload_current_size -1 , "}", 1);
    return ddccSendMessageToRegion(
            "spain",
            payload,
            payload_current_size,
            &response,
            &response_len
    );
}