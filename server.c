#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "common.h"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

//typedef struct {
//    int socket;
//    int number;
//} client_info;

void crawling();
void* handle_client(void* arg);
void send_message_to_all_clients(const char* message, int client_number, int sender_socket);

int connected_clients = 0;
pthread_mutex_t connected_clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex2 = PTHREAD_MUTEX_INITIALIZER;

int server_running = 1;

int client_sockets[MAX_CLIENTS];
int client_sockets_count = 0;

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("Usage: %s [port]\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        perror("Error opening server socket");
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    client_addr_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding server socket");
        return 1;
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening to server socket");
        return 1;
    }

    printf("Server listening on port %d\n", port);

    pthread_t client_threads[MAX_CLIENTS];

    // Accept clients
    while (server_running && client_sockets_count < MAX_CLIENTS) {
        int new_client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (new_client_sock == -1) {
            perror("Error accepting client connection");
            continue;
        }
        printf("New client connection : %s\n", inet_ntoa(client_addr.sin_addr));

        pthread_mutex_lock(&connected_clients_mutex);
        client_sockets[client_sockets_count++] = new_client_sock;
        connected_clients++;
        pthread_mutex_unlock(&connected_clients_mutex);

        pthread_t client_thread;

        client_info new_client;
        new_client.socket = new_client_sock;
        new_client.number = client_sockets_count;

        if (pthread_create(&client_thread, NULL, handle_client, (void*) &new_client_sock) != 0) {
            perror("Error creating client thread");
            break;
        }
        client_threads[client_sockets_count - 1] = client_thread;
    }

    for(int i = 0 ; i < client_sockets_count ; ++i) {
        pthread_join(client_threads[i], NULL);
    }

    pthread_exit(NULL); 
    close(server_socket);
    pthread_mutex_destroy(&connected_clients_mutex);
    pthread_mutex_destroy(&send_mutex1);
    pthread_mutex_destroy(&send_mutex2);
    return 0;
}

void send_file_to_client(int client_socket, const char* filename) {
    char buffer[BUFFER_SIZE];
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        snprintf(buffer, sizeof(buffer), "Error: Could not open the file!");
        send(client_socket, buffer, strlen(buffer) + 1, 0);
        return;
    }

    while (!feof(file)) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
        if (ferror(file)) {
            perror("Error reading from file");
            break;
        }
        send(client_socket, buffer, bytes_read, 0);
    }
    fclose(file);
}

void realtime_price_crawling() {
    int result = system("python3 coin_crawler_top_coin.py");
    if (result == -1) {
        perror("Error running python script");
    } else {
        printf("Good to bring Information\n");
    }
}

void new_coin_crawling() {
    int result = system("python3 coin_crawler_new_coin.py");
    if (result == -1) {
        perror("Error running python script");
    } else {
        printf("Good to bring Information\n");
    }
}

void* handle_client(void* arg) {
    //client_info* client = (client_info*)arg;
    int client_socket = *(int*)arg;
    //int client_socket = client->socket;
    int client_number = client_socket;

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Send the menu to the client
    //const char* menu_str = "Choose an option\n1: Chat\n2: Realtime cryptocurrency price\n3: New cryptocurrency coin\n4: Exit\n";
    //send(client_socket, menu_str, strlen(menu_str) + 1, 0);

    while(server_running) {

        const char* menu_str = "\n=============Welcome to LEC's cryptocurrency Program=============\n\n\t\tChoose an option\n\t\t1: Chat\n\t\t2: Realtime cryptocurrency price\n\t\t3: New cryptocurrency coin\n\t\t4: Exit\n\n=================================================================\n";
        send(client_socket, menu_str, strlen(menu_str) + 1, 0);

        if ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';

            //printf("Received message from client %d: %s\n", client_socket, buffer);
            if (strcmp(buffer, "start") == 0) { 
                send_message_to_all_clients("Client %d has joined the chat\n", client_number, client_socket);
                printf("Client %d has joined the chat\n", client_number);
                while (server_running && (bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
                    buffer[bytes_received] = '\0';
                    
                    char client_message[BUFFER_SIZE + 24]; 
                    snprintf(client_message, sizeof(client_message),  "Client %d : %s", client_number, buffer);

                    if (strcmp(buffer, "4") == 0) {
                        printf("Client %d has left the chat\n", client_number);
                        send_message_to_all_clients("Client %d has left the chat\n", client_number, client_socket);
                        break;
                    }
                    
                    printf("Chat message from client %d: %s\n", client_socket, buffer);
                    send_message_to_all_clients(client_message, client_number, client_socket);
                }
            } else if (strcmp(buffer, "get_realtime_price") == 0) {
                printf("Client %d requested Info\n", client_number);
                realtime_price_crawling();
                pthread_mutex_lock(&send_mutex1);
                send_file_to_client(client_socket, "cryptocurrency_realtime_price.txt");
                pthread_mutex_unlock(&send_mutex1);
                //menu_str = "1. Chat\n2. Realtime cryptocurrency price\n3. Exit\n";
                //send(client_socket, menu_str, strlen(menu_str) + 1, 0);

            } else if (strcmp(buffer, "get_new_coin") == 0) {
                printf("Client %d requested Info\n", client_number);
                new_coin_crawling();
                pthread_mutex_lock(&send_mutex2);
                send_file_to_client(client_socket, "cryptocurrency_newcoin.txt");
                pthread_mutex_unlock(&send_mutex2);

            } else if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "exit_invalid") == 0) {
                printf("Client %d has left the server\n", client_number);
                send_message_to_all_clients("Client %d has left the server\n", client_number, client_socket);
                pthread_mutex_lock(&connected_clients_mutex);
                connected_clients--;
                if(connected_clients == 0) {
                    server_running = 0;
                    printf("No clients remaining. Shutting down server...\n");
                }
                pthread_mutex_unlock(&connected_clients_mutex);
                close(client_socket);
                break;
            }
        }
    }

    close(client_socket);
    return NULL;
}

void send_message_to_all_clients(const char* message, int client_number, int sender_socket) {
    
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), message, client_number);
    
    for (int i = 0; i < client_sockets_count; ++i) {
        if (client_sockets[i] != sender_socket) {
            send(client_sockets[i], buffer, strlen(buffer) + 1, 0);
        }
    }
}
