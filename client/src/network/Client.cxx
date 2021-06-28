#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Client.hpp"

#define PORT     8080
#define MAXLINE  1024

namespace ScreenShare::Network {
	Client::Client() {
	}

	Client::~Client() {
	}

	void Client::Run() {
		int sockfd;
		char buffer[MAXLINE];
		char *hello = "Hello from server";
		struct sockaddr_in servaddr;

		// Creating socket file descriptor
		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("socket creation failed");
			exit(EXIT_FAILURE);
		}
		printf("Socket created : %d\n", sockfd);

		memset(&servaddr, 0, sizeof(servaddr));

		// Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

		int n;
		socklen_t len;

		while(true) {
			sendto(sockfd, (const char*)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));
			printf("Hello message sent.\n");
			n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
			buffer[n] = '\0';
			printf("Server : %s\n", buffer);
		}

    close(sockfd);
	}
}
