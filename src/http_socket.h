#ifndef HTTP_SOCKET_H
#define HTTP_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

int do_socket(void);

#endif
