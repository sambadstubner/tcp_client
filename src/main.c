#include <stdio.h>
#include <stdbool.h>

#include "tcp_client.h"

#include "file_parser.h"
#include "log.h"

char host[1024];
char port[10];
char file[1024];

int line_count = 0;
int socketfd;

void init_config(Config *config);

void response_callback(char * response){
    static int num_received = 0;

    printf("%s\n", response);

    num_received++;
    log_debug("callback num received: %d", num_received);

    if(num_received >= line_count){
        if(tcp_client_close(socketfd) < 0){
            log_error("Failed to close socket");
        }
    }
}


int main(int argc, char *argv[]) {
    Config config;
    init_config(&config);
    tcp_client_parse_arguments(argc, argv, &config);

    FILE *file = tcp_client_open_file(config.file);
    if(file == NULL){
        log_error("Could not open file");
        exit(EXIT_FAILURE);
    }

    
    socketfd = tcp_client_connect(config);
    if(socketfd == -1){
        exit(EXIT_FAILURE);
    }

    char *action;
    char *message;
    int get_line_result;
    while((get_line_result = tcp_client_get_line(file, &action, &message))!=-1){
        if((action != NULL) && get_line_result > 1){
            line_count++;
            if(tcp_client_send_request(socketfd, action, message)){
                exit(EXIT_FAILURE);
            }
        }else{
            log_debug("Action is invalid or line is empty");
        }
        if(get_line_result > 1){
            free(action);
            free(message);
        }else{
        }
    }

    if(tcp_client_close_file(file)){
        exit(EXIT_FAILURE);
    }

    if(tcp_client_receive_response(socketfd, response_callback)){
        exit(EXIT_FAILURE);
    }
}


void init_config(Config *config){
    config->file = file;
    config->host = host;
    config->port = port;
}