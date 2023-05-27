#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "common.h"

#define BUFFER_SIZE 1024

void* receive_messages(void* arg);

pthread_mutex_t print_mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex3 = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("Usage: %s [address] [port]\n", argv[0]);
        return 1;
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating client socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        return 1;
    }

    printf("Connected to server\n");

    pthread_t receive_thread;
    if(pthread_create(&receive_thread, NULL, receive_messages, (void*) &client_socket) != 0) {
        perror("Error creating receive thread");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    int option;

    while(option != 4) {
        //printf("Choose an option\n1: Chat\n2: Realtime cryptocurrency price\n3: New cryptocurrency coin\n4: Exit\n");
        scanf("%d", &option);
        getchar();
        //sprintf(buffer, "%d", option);
        //send(client_socket, buffer, strlen(buffer) + 1, 0);

        if (option == 1) {
            send(client_socket, "start", 6, 0);
            while (1) {
                printf("Enter message: ");
                fgets(buffer, BUFFER_SIZE, stdin);
                buffer[strlen(buffer) - 1] = '\0';

                if (send(client_socket, buffer, strlen(buffer) + 1, 0) < 0) {
                    perror("Error sending message");
                    break;
                }

                if (strcmp(buffer, "4") == 0) {
                    printf("Exiting the chat...\n");
                    break;
                }
            }
        } else if(option == 2) {
            send(client_socket, "get_realtime_price", 20, 0);
            //sleep(2);
            printf("Receiving realtime price content...\n");

            char price_buffer_from_server[BUFFER_SIZE];
            ssize_t received_bytes;
            if ((received_bytes = recv(client_socket, price_buffer_from_server, BUFFER_SIZE - 1, 0)) > 0) {
                price_buffer_from_server[received_bytes] = '\0';
                pthread_mutex_lock(&print_mutex2);
                printf("%s", price_buffer_from_server);
                pthread_mutex_unlock(&print_mutex2);
            }
            //printf("\nreal time price content has been received and printed.\n\n\n");

        } else if(option == 3) {
            send(client_socket, "get_new_coin", 14, 0);
            printf("Receiving new coin content...\n");

            char coin_buffer_from_server2[BUFFER_SIZE];
            ssize_t received_new_coin_bytes;
            if ((received_new_coin_bytes = recv(client_socket, coin_buffer_from_server2, BUFFER_SIZE - 1, 0)) > 0) {
                coin_buffer_from_server2[received_new_coin_bytes] = '\0';
                pthread_mutex_lock(&print_mutex3);
                printf("%s", coin_buffer_from_server2);
                pthread_mutex_unlock(&print_mutex3);
            }
            //printf("\n new coin content has been received and printed.\n\n\n");

        } else if (option == 4) {
            printf("Exiting....\n");
            send(client_socket, "exit", 5, 0);
        } else {
            printf("Invalid option. Exiting....\n");
            send(client_socket, "exit_invalid", 13, 0);
            break;
        }
    }

    pthread_join(receive_thread, NULL);

    close(client_socket);
    pthread_mutex_destroy(&print_mutex1);
    pthread_mutex_destroy(&print_mutex2);
    pthread_mutex_destroy(&print_mutex3);
    return 0;
}

void* receive_messages(void* arg) {

    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        pthread_mutex_lock(&print_mutex1);
        //sleep(1);
        printf("%s\n", buffer);
        pthread_mutex_unlock(&print_mutex1);
    }

    return NULL;
}
