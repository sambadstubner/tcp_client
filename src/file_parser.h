#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Description:
    Opens a file.
Arguments:
    char *file_name: The name of the file to open
Return value:
    Returns NULL on failure, a FILE pointer on success
*/
FILE *file_parser_open_file(char *file_name);


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
int file_parser_get_line(FILE *file, char **action, char **message);


/*
Description:
    Closes a file.
Arguments:
    FILE *fd: The file pointer to close
Return value:
    Returns a 1 on failure, 0 on success
*/
int file_parser_close_file(FILE *file);


#endif