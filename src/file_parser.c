#include "log.h"

#include "file_parser.h"

#define NUM_ACTIONS 5
#define DEFAULT_ACTIONS_SIZE 20
#define DEFAULT_REQUEST_SIZE 1024
#define DEFAULT_MESSAGE_SIZE DEFAULT_REQUEST_SIZE - DEFAULT_ACTIONS_SIZE

char *file_parser_actions[NUM_ACTIONS] = {"uppercase", "lowercase", "reverse", "shuffle", "random"};

bool is_valid_action(char *input_action);
void file_parser_get_request(char *input_line, char *request);
void get_action(char *input_line, char **action);
void get_message(char *input_line, char **message);

/*
Description:
    Opens a file.
Arguments:
    char *file_name: The name of the file to open
Return value:
    Returns NULL on failure, a FILE pointer on success
*/
FILE *file_parser_open_file(char *file_name){
    if(strcmp(file_name, "-") == 0){
        log_debug("Setting input file to STDIN");
        return fdopen(0, "r");
    }
    return fopen(file_name, "r");
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
int file_parser_get_line(FILE *file, char **action, char **message){
    if (file == NULL) {
        log_error("Error opening filestream");
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    int count = getline(&line, &len, file);
    if(count > 1){
        line[strcspn(line, "\n")] = '\0';
        log_debug("Line: %s", line);
        get_action(line, action);
        get_message(line, message);
    }
    free(line);
    return count;
}


/*
Description:
    Closes a file.
Arguments:
    FILE *fd: The file pointer to close
Return value:
    Returns a 1 on failure, 0 on success
*/
int file_parser_close_file(FILE *file){
    return -1*fclose(file);
}


bool is_valid_action(char *input_action){
    for (int i = 0; i < NUM_ACTIONS; i++){
        if(strcmp(input_action, file_parser_actions[i]) == 0){
            return true;
        }
    }
    log_debug("Invalid action");
    return false;
}

//  FIXME make sure this is a valid action
void get_action(char *input_line, char **action){
    const char * space_index = strchr(input_line, ' ');
    int action_size = space_index - input_line + 1;
    *action = (char*) malloc(sizeof(char *) * (action_size + 1));
    if(action == NULL){
        log_error("Action allocation failed");
    }
    memset(*action, '\0', action_size);
    strncpy(*action, input_line, space_index-input_line);
    if(!is_valid_action(*action)){
        *action = NULL;
    }
    log_debug("Get action: %s", *action);
}

void get_message(char *input_line, char **message){
    const char * space_index = strchr(input_line, ' ');
    int message_size = strlen(space_index);
    *message = (char*) malloc(sizeof(char *) * (message_size));
    if(message == NULL){
        log_error("Action allocation failed");
    }
    int message_length = strlen(space_index + 1);
    memset(*message, '\0', message_size);
    strncpy(*message, space_index + 1, message_length);
    log_debug("Get Message: %s", *message);
}