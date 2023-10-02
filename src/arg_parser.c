#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg_parser.h"
#include "log.h"


#define NUM_ACTIONS 5
#define MAX_INPUT_SIZE 1024
#define MESSAGE_BUFFER MAX_INPUT_SIZE + 1
#define NO_ACTION_SELECTED 100
#define INT_MIN 0
#define INT_MAX 65536
#define MAX_PORT 65535

// char *actions[NUM_ACTIONS] = {"uppercase", "lowercase", "reverse", "shuffle", "random"};
// static char action[20];
// static char message[MAX_INPUT_SIZE];


void set_port(char *inputPort, Config *config);
void set_host(char *inputHost, Config *config);
// void set_action(char *input_action, Config *config);
// void set_message(char *input_message, Config *config, int word_index);
void print_help();
int numPlaces (int n);
void set_file(char *input_file, Config *config);

static struct option long_options[] = {
   {"help", no_argument, 0, 0},
   {"verbose", no_argument, 0, 'v'},
   {"host", required_argument, 0, 'h'},
   {"port", required_argument, 0, 'p'},
   {0, 0, 0, 0}
};


void arg_parser_init(int argc, char *argv[], Config *config){
    log_set_level(LOG_ERROR);

    int option_index = 0;
    int opt = 0;
    while((opt = getopt_long(argc, argv, "vh:p:", long_options, &option_index)) != -1){

        switch (opt)
        {
        case 'v':
            log_set_level(LOG_DEBUG);
            log_debug("Verbose flag set");
            break;
        case 'h':
            set_host(optarg, config);
            break;
        case 'p':
            set_port(optarg, config);
            break;
        case 0:
            // Handle help since no short option
            if(strcmp(long_options[option_index].name, "help") == 0){
                print_help();
                exit(EXIT_SUCCESS);
            }

        case '?':
            // Handle invalid options or missing arguments
            log_error("Invalid or missing arguments");
            print_help();
            exit(EXIT_FAILURE);

        default:
            abort();
            break;
        }
    }

    static int count = 0;
    for (int index = optind; index < argc; index++) {
        if (count == 0){
            set_file(argv[index], config);
        }
        count++;
    }
    if (count < 1){
        log_error("No FILE provided");
        print_help();
        exit(EXIT_FAILURE);
    }
    if (count > 1){
        log_error("Too many arguments");
        print_help();
        exit(EXIT_FAILURE);
    }
}

void set_file(char *input_file, Config *config){
    log_debug("Setting file to %s", input_file);
    strcpy(config->file, input_file);
}

// FIXME: This needs to be more robust still.
void set_port(char *input_port, Config *config){
    int len_port = strlen(input_port);
    int port_as_int = atoi(input_port);
    if (!port_as_int){
        log_error("Not int Port must be an positive integer");
        print_help();
        exit(EXIT_FAILURE);
    }
    else if(numPlaces(port_as_int) != len_port){
        log_error("Port must be an positive integer less than 65535");
        print_help();
        exit(EXIT_FAILURE);
    }
    else if(port_as_int <= 0){
        log_error("Port must be an positive integer less than 65535");
        print_help();
        exit(EXIT_FAILURE);
    }else if(port_as_int >= MAX_PORT){
        log_error("Port must be an positive integer less than 65535");
        print_help();
        exit(EXIT_FAILURE);
    }
    else{
        config->port = input_port;
        log_debug("Port set to %s", config->port);
    }
}

void set_host(char *inputHost, Config *config){
    if(strlen(inputHost) <= 0){
        log_error("A host must be provided");
        print_help();
        exit(EXIT_FAILURE);
    }
    strcpy(config->host, inputHost);
    log_debug("Host set to %s", config->host);
}


// void get_request(char *destination_request, Config *config){
//     strcpy(destination_request, action);
//     strcat(destination_request, " ");
//     char message_length[MAX_INPUT_SIZE];
//     sprintf(message_length, "%lu", strlen(message));
//     strcat(destination_request, message_length);
//     strcat(destination_request, " ");
//     strcat(destination_request, message);
//     log_debug("Request: %s", destination_request);
// }

int numPlaces (int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

void print_help(){
    printf(ARG_PARSER_HELP_MESSAGE);
}