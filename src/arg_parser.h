#ifndef ARG_PARSER_H_
#define ARG_PARSER_H_

#include <getopt.h>

#include "tcp_client.h"

#define ARG_PARSER_HELP_MESSAGE "Usage: tcp_client [--help] [-v] [-h HOST] [-p PORT] FILE\n\
\n\
Arguments:\n\
  FILE   A file name containing actions and messages to\n\
         send to the server. If \"-\" is provided, stdin will\n\
         be read\n\
\n\
Options:\n\
  --help\n\
  -v, --verbose\n\
  --host HOSTNAME, -h HOSTNAME\n\
  --port PORT, -p PORT\n"


void arg_parser_init();

int get_port();

void get_request(char *destination_request, Config *config);

#endif