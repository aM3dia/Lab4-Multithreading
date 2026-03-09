/*
Anupa Ragoonanan (N01423202)
CPAN 226 - Networking and Telecommunications
Lab 4 - Multithreading and Network Concurrency
March 9, 2026
Note: optimized to function on MacBook
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define SOCKET int
#define INVALID_SOCKET -1

/* Task 2 */
typedef struct {
    int client_socket;
    int client_id;
} thread_args_t;

void* handle_client(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;
    int client_socket = args->client_socket;
    int client_id = args->client_id;
    free(arg);

    char *message = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello Client!";
    
    printf("[Server] Handling client %d...\n", client_id);
    
    // Simulating "bottleneck" or heavy processing
    printf("[Server] Processing request for 5 seconds...\n");
    sleep(5); 
    
    send(client_socket, message, (int)strlen(message), 0);
    printf("[Server] Response sent to client %d. Closing connection.\n", client_id);
    
    close(client_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_count = 0;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Server listening on port %d...\n", PORT);
    printf("NOTE: This server is SEQUENTIAL. It can only handle one client at a time!\n\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket != INVALID_SOCKET) {
            client_count++;
            
            // Allocate memory for thread arguments
            thread_args_t *args = malloc(sizeof(thread_args_t));
            args->client_socket = client_socket;
            args->client_id = client_count;

            // Create a thread to handle the client
            pthread_t thread;
            if (pthread_create(&thread, NULL, handle_client, args) == 0) {
                // Detach the thread so that it cleans up after itself
                pthread_detach(thread);
                printf("[Server] Created thread for client %d\n", client_count);
            } else {
                printf("[Server] Failed to create thread for client %d\n", client_count);
                free(args);
                close(client_socket);
            }
        }
    }

    close(server_socket);
    return 0;
}
