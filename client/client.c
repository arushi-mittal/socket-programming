// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#define bs 1024
#define PORT 8000

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[bs] = {0};
    long double percentage;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }

  	int flag = 0;
  	if (argc > 1) {
  		if (strcmp(argv[1],"exit") == 0) {
  			close(sock);
  			return 0;
  		}
  		else if (strcmp(argv[1], "get")) {
  			printf("Incorrect argument\n");
        return 0;
  		}
  	}
  	else {
  		printf("Insufficient arguments provided\n");
      return 0;
  	}

  	if (argc == 2) {
  		printf("No file names given\n");
  		return -1;
  	}

  	for (int i = 2; i < argc; i++) {
  		send(sock, argv[i], strlen(argv[i]), 0);
  		recv(sock, buffer, 40, 0);
  		if (strcmp(buffer, "-1") == 0) {
  			printf("Error accessing file\n");
        bzero(buffer, bs);
  			continue;
  		}
      send(sock, argv[i], strlen(argv[i]), 0);
  		FILE * fp = fopen(argv[i], "w");
  		if (fp == NULL) {
  			int fd = open(argv[i], O_CREAT | O_WRONLY);
  			close(fd);
  			fp = fopen(argv[i], "w");
  			if (fp == NULL) {
  				printf("Error opening file\n");
          bzero(buffer, bs);
  				continue;
  			}
  		}

  		long long int size = atoi(buffer);
  		printf("File size: %lld\n", size);
  		bzero(buffer, bs);
  		if (size < bs) {
  			recv(sock, buffer, bs, 0);
  			fwrite(buffer, bs, 1, fp);
  			bzero(buffer, bs);
        printf("100.00 percent");
  		}
  		else {
  			bzero(buffer, bs);
  			long long int received = 0;
  			while (received < size) {
  				recv(sock, buffer, bs, 0);
  				fwrite(buffer, bs, 1, fp);
  				received = received + bs;
          if (received > size) {
            received = size;
          }
          percentage = (float) received / size * 100;
          printf("\r%Lf percent", percentage);
  				bzero(buffer, bs);
  			}
  		}
  		bzero(buffer, bs);
  		fclose(fp);
      printf("\nFinished\n");
      bzero(buffer, bs);
  	}

    close(sock);
    return 0;
}
