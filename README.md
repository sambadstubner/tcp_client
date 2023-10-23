# TCP Client

An example of of a pipelined TCP CLI Client supporting a custom protocol

## Usage
```
Usage: tcp_client [--help] [-v] [-h HOST] [-p PORT] FILE

Arguments:
  FILE   A file name containing actions and messages to
         send to the server. If "-" is provided, stdin will
         be read.

Options:
  --help
  -v, --verbose
  --host HOSTNAME, -h HOSTNAME
  --port PORT, -p PORT
```

## Protocol
Each message will be of the following format: ```ACTION MESSAGE```.

The available actions are:
- uppercase
- lowercase
- reverse
- shuffle
- random

The client will parse the action and message and send it to it's corresponding server, which will processed the action and message and send a response

The request is formatted by the client as follows:
``` 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Action |                    Message Length                   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Data                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Action: 5 bits
Message Length: 27 bits
Data: variable
```

The response from the server is expected to be formatted as follows:
``` 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         Message Length                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Data                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Message Length: 32 bits
Data: variable
```
