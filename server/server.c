#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#define PORT 8000
#define bs 1024
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[bs] = {0};
    char *hello = "Hello from server";
    // char * filenames[100];
    long double percentage;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 10) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    bzero(buffer, bs);
    struct stat st;     
    while (recv(new_socket, buffer, bs, 0) > 0) {
        printf("Received request for file %s\n", buffer);
        stat(buffer, &st);
        int x = S_ISREG(st.st_mode);
        if (x != 1) {
            printf("Error: Not a regular file\n");
            char sizearr[40];
            sprintf(sizearr, "%d", -1);
            send(new_socket, sizearr, 40, 0);
            bzero(buffer, bs);
            continue;
        }
        FILE * fp = fopen(buffer, "r");
        if (fp == NULL) {
            printf("Error accessing file\n");
            char sizearr[40];
            sprintf(sizearr, "%d", -1);
            send(new_socket, sizearr, 40, 0);
            bzero(buffer, bs);
            continue;
        }
        bzero(buffer, bs);
        fseek(fp, 0L, SEEK_END);
        long long int size = ftell(fp);
        char sizearr[40];
        sprintf(sizearr, "%lld", size);
        send(new_socket, sizearr, 40, 0);
        recv(new_socket, buffer, bs, 0);
        bzero(buffer, bs);
        fseek(fp, 0L, SEEK_SET);
        if (size < bs) {
            bzero(buffer, bs);
            fread(buffer, bs, 1, fp);
            send(new_socket, buffer, bs, 0);
            printf("100.00 percent");
        }
        else {
            bzero(buffer, bs);
            long long int sent = 0;
            while (sent < size) {
                bzero(buffer, bs);
                fseek(fp, sent, SEEK_SET);
                fread(buffer, bs, 1, fp);
                send(new_socket, buffer, bs, 0);
                sent = sent + bs;
                if (sent > size) {
                    sent = size;
                }
                percentage = (float) sent/size * 100;
                printf("\r%Lf percent", percentage);
            }
            bzero(buffer, bs);
        }
        fclose(fp);
        bzero(buffer, bs);
        printf("\nFinished\n");
        bzero(buffer, bs);
    }
    return 0;
}
