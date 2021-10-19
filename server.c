#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

void handle_error(char *msg);

void handle_error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void serve_connection(int sockfd);

int main(int argc, char const *argv[]) {
    int res;

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0
    // htons converts a short integer (e.g. port) to a network representation
    address.sin_port = htons(PORT);
    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    // Creating socket file descriptor
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        handle_error("In socket");
    }

    res = bind(server_fd, (struct sockaddr *)&address, addrlen);
    if (res < 0) {
        handle_error("In bind");
    }

    res = listen(server_fd, 10);
    if (res < 0) {
        handle_error("In listen");
    }

    while (1) {
        printf("\n---Waiting for new connection---\n\n");

        int new_socket = accept(server_fd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
        if (new_socket < 0) {
            handle_error("In accept");
        }

        // TODO print info about incoming connection
        serve_connection(new_socket);
    }
    return EXIT_SUCCESS;
}

const char *SERVER_ERROR =
    "HTTP/1.1 500 Internal Server Error\n"
    "Content-Type: text/plain\n"
    "Content-Length: 12\n"
    "\n"
    "Server Error";

const char *HELLO =
    "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello "
    "world!";

void serve_connection(int sockfd) {
    //   if (send(sockfd, "*", 1, 0) < 1) {
    //     handle_error("send");
    //   }
    char method[6] = {'\0'};
    char path[PATH_MAX + 1] = {'\0'};

    char verb[4];  // len("GET ")
    int len = recv(sockfd, verb, sizeof(verb), 0);
    if (len < 0) {
        handle_error("recv");
    }
    if (strncmp(verb, "GET", 3) != 0) {
        printf("Unsupported HTTP verb");
        write(sockfd, SERVER_ERROR, strlen(SERVER_ERROR));
        close(sockfd);
        return;
    }
    strcpy(method, "GET");

    while (1) {
        char buf[1024];
        int len = recv(sockfd, buf, sizeof(buf), 0);

        if (len < 0) {
            handle_error("recv");
        } else if (len == 0) {
            break;
        }

        for (int i = 0; i < len; ++i) {
            if (buf[i] == ' ') {
                printf("METHOD: %s\nPATH: %s\n", method, path);
                // TODO read file path and send response
                write(sockfd, HELLO, strlen(HELLO));
                close(sockfd);
                return;
            } else {
                // append symbol to path
                strncat(path, &buf[i], sizeof(char));
            }
        }
    }
}
