#include "log.h"

#include "tcp_client.h"
#include "log.h"

#include "tcp_client.h"

#include "arg_parser.h"
#include "file_parser.h"


#define NUM_SPACES_IN_REQUEST 2
#define MESSAGE_LENGTH_SIZE 27
#define HEADER_SIZE 32
#define ACTION_SIZE 5
#define MAX_MESSAGE_SIZE 2^MESSAGE_LENGTH_SIZE
#define UPPERCASE 0x01
#define LOWERCASE 0x02
#define REVERSE 0x04
#define SHUFFLE 0x08
#define RANDOM 0x10

static int get_tens_places(int input_num);
static int encode_action(char *action);
static uint32_t create_header(char* action, char* message);
static uint32_t get_message_length(char *data);

/*
Description:
    Parses the commandline arguments and options given to the program.
Arguments:
    int argc: the amount of arguments provided to the program (provided by the main function)
    char *argv[]: the array of arguments provided to the program (provided by the main function)
    Config *config: An empty Config struct that will be filled in by this function.
Return value:
    Returns a 1 on failure, 0 on success
*/
int tcp_client_parse_arguments(int argc, char *argv[], Config *config){
    arg_parser_init(argc, argv, config);
    return 1;
}

///////////////////////////////////////////////////////////////////////
/////////////////////// SOCKET RELATED FUNCTIONS //////////////////////
///////////////////////////////////////////////////////////////////////

/*
Description:
    Creates a TCP socket and connects it to the specified host and port.
Arguments:
    Config config: A config struct with the necessary information.
Return value:
    Returns the socket file descriptor or -1 if an error occurs.
*/
int tcp_client_connect(Config config){
    int client_fd = 0;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(config.host, config.port, &hints, &servinfo) != 0){
        log_error("Get address info failed!\n");
        return -1;
    }

    for(p = servinfo; p!= NULL; p = p->ai_next){
        if((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            log_debug("Socket creation failed on iteration");
            continue;
        }else{
            log_info("Socket created successfully");
        }
        if(connect(client_fd, p->ai_addr, p->ai_addrlen) == -1){
            log_debug("Socket could not connect on iteration");
            close(client_fd);
        }
        break;
    }
    if(p == NULL){
        log_error("Failed to connect");
        return -1;
    }else{
        log_info("Connection sucessful to: %s", config.host);
    }

    freeaddrinfo(servinfo);

    return client_fd;
}

/*
Description:
    Creates and sends request to server using the socket and configuration.
Arguments:
    int sockfd: Socket file descriptor
    char *action: The action that will be sent
    char *message: The message that will be sent
Return value:
    Returns a 1 on failure, 0 on success
*/
int tcp_client_send_request(int sockfd, char *action, char *message){
    char *request;
    int message_length = strlen(message);
    int request_size = 4 + message_length;
    request = (char *)malloc(request_size*sizeof(char));

    uint32_t header = create_header(action, message);
    memcpy(request, &header, 4);
    memcpy(request + 4, message, message_length);

    log_debug("Request: %s %d %s", action, message_length, message);
    
    unsigned long total_num_sent = 0;
    while(total_num_sent < request_size){
        log_debug("Attempting to send: %s", request);
        int num_sent = send(sockfd, request + total_num_sent, request_size, 0);

        if(num_sent == 0){
            exit(EXIT_SUCCESS);
        }
        if(num_sent == -1){
            log_error("Sent failed");
            return 1;
        }
        log_debug("Sent: %s", request + total_num_sent);

        total_num_sent += num_sent;
    }
    log_info("Sent: %s", request);

    free(request);
    
    return 0;
}

/*
Description:
    Receives the response from the server. The caller must provide a function pointer that handles
the response and returns a true value if all responses have been handled, otherwise it returns a
    false value. After the response is handled by the handle_response function pointer, the response
    data can be safely deleted. The string passed to the function pointer must be null terminated.
Arguments:
    int sockfd: Socket file descriptor
    int (*handle_response)(char *): A callback function that handles a response
Return value:
    Returns a 1 on failure, 0 on success
*/
int tcp_client_receive_response(int sockfd, int (*handle_response)(char *)){
    char *buffer;
    static int buffer_size = 10;
    buffer = (char *)malloc(sizeof(char)*buffer_size);
    memset(buffer, '\0', buffer_size);
    bool is_server_done = false;
    int total_buffer_contents = 0;

    while(1){
        static int buffer_offset = 0;
        int num_read = 0;
        if(!is_server_done){
            num_read = recv(sockfd, buffer + buffer_offset, buffer_size - buffer_offset, 0);
            total_buffer_contents += num_read;
        }
        log_debug("num_read: %d", num_read);
        // Check to see if done reading
        if(num_read == -1){
            log_debug("Cannot read from buffer, socket closed");
            break;
        }
        if(num_read == 0){
            log_debug("Server done sending");
            is_server_done = true;
        }
        while(1){
            log_debug("buffer: %s", buffer);
            // Find sentinal value (space)
            // char *space_location = strchr(buffer, ' ');
            uint32_t response_length = get_message_length(buffer);
            log_debug("length: %d", response_length);
            int total_response_length = response_length + 4;
            log_debug("response_length: %d", response_length);

            // Reallocate the buffer if necessary and read again
            if(total_response_length > total_buffer_contents){
                log_debug("Reallocating buffer");
                buffer_size = total_response_length * 2;
                buffer = realloc(buffer, buffer_size);
                buffer_offset = total_buffer_contents;
                break;
            }else if(total_response_length > total_buffer_contents){
                buffer_offset = num_read - total_response_length;
                break;
            }
            // Copy the response from buffer into the response buffer
            char *response = (char *)malloc(sizeof(char *) * response_length);
            strncpy(response, buffer + 4, response_length);
            response[response_length] = '\0';
            log_debug("Response: %s", response);
            handle_response(response);
            free(response);
            
            // Shift the buffer down
            memmove(buffer, buffer + total_response_length, buffer_size - total_response_length);
            total_buffer_contents -= total_response_length;
            log_debug("buffer after shifting: %s", buffer);

            // Fix to if processed the whole buffer, not just what was read.
            if(total_buffer_contents == 0){
                buffer_offset = 0;
                break;
            }
        }
    }
    free(buffer);
    return 0;
}



/*
Description:
    Closes the given socket.
Arguments:
    int sockfd: Socket file descriptor
Return value:
    Returns a 1 on failure, 0 on success
*/
int tcp_client_close(int sockfd){
    log_info("Closing socket");
    return close(sockfd);
}

///////////////////////////////////////////////////////////////////////
//////////////////////// FILE RELATED FUNCTIONS ///////////////////////
///////////////////////////////////////////////////////////////////////

/*
Description:
    Opens a file.
Arguments:
    char *file_name: The name of the file to open
Return value:
    Returns NULL on failure, a FILE pointer on success
*/
FILE *tcp_client_open_file(char *file_name){
    return file_parser_open_file(file_name);
}

/*
Description:
    Gets the next line of a file, filling in action and message. This function should be similar
    design to getline() (https://linux.die.net/man/3/getline). *action and message must be allocated
    by the function and freed by the caller.* When this function is called, action must point to the
    action string and the message must point to the message string.
Arguments:
    FILE *fd: The file pointer to read from
    char **action: A pointer to the action that was read in
    char **message: A pointer to the message that was read in
Return value:
    Returns -1 on failure, the number of characters read on success
*/
int tcp_client_get_line(FILE *fd, char **action, char **message){
    return file_parser_get_line(fd, action, message);
}

/*
Description:
    Closes a file.
Arguments:
    FILE *fd: The file pointer to close
Return value:
    Returns a 1 on failure, 0 on success
*/
int tcp_client_close_file(FILE *fd){
    return file_parser_close_file(fd);
}


static int get_tens_places(int input_num){
    return snprintf(NULL, 0, "%d",input_num);
}

static int encode_action(char *action){
    if(strcmp("uppercase", action) == 0){
        return UPPERCASE;
    }
    if(strcmp("lowercase", action) == 0){
        return LOWERCASE;
    }
    if(strcmp("reverse", action) == 0){
        return REVERSE;
    }
    if(strcmp("shuffle", action) == 0){
        return SHUFFLE;
    }
    if(strcmp("random", action) == 0){
        return RANDOM;
    }
}

static uint32_t create_header(char* action, char* message){
    uint32_t action_int = (encode_action(action));
    uint32_t message_length = strlen(message);
    if(message_length > (MAX_MESSAGE_SIZE)){
        log_error("Message exceeds size");
    }
    uint32_t header_int = (action_int << 27) | message_length;

    log_debug("Header before htonl: %#04x", header_int);

    header_int = htonl(header_int);

    log_debug("Header after htonl: %#04x", header_int);

    return header_int;
}

static uint32_t get_message_length(char *data){
    uint32_t header = *(uint32_t *)data;
    header = ntohl(header);
    uint32_t length = header & 0x7FFFFFFF;
    return length;
}