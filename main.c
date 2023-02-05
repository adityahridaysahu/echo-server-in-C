#include "header.h"
void run_server(const char *ip_address, int port)
{
    struct sockaddr_in server_address, client_address;
    socklen_t client_len;
    char buf[MAX_BUF];
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Error creating socket");
        exit(1);
    }
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_aton(ip_address, &server_address.sin_addr) == 0)
    {
        perror("Invalid IP address");
        exit(1);
    }
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error binding socket");
        exit(1);
    }
    if (listen(server_socket, 1) < 0)
    {
        perror("Error listening for incoming connections");
        exit(1);
    }
    printf("Server is waiting for a client to connect...\n");
    client_len = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
    if (client_socket < 0)
    {
        perror("Error accepting incoming connection");
        exit(1);
    }
    printf("Client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    memset(buf, 0, MAX_BUF);
    int msg_len = recv(client_socket, buf, MAX_BUF, 0);
    if (msg_len < 0)
    {
        perror("Error receiving from socket");
        exit(1);
    }
    printf("Received from client: %s\n", buf);
    int sent_len = send(client_socket, buf, strlen(buf), 0);
    if (sent_len < 0)
    {
        perror("Error sending to socket");
        exit(1);
    }
    printf("Echoed back to the client\n");
    close(client_socket);
    close(server_socket);
}

void run_client(const char *ip_address, int port)
{
    struct sockaddr_in server_address, client_address;
    char buf[MAX_BUF];
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Error creating socket");
        exit(1);
    }
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_aton(ip_address, &server_address.sin_addr) == 0)
    {
        perror("Invalid IP address");
        exit(1);
    }
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error connecting to server");
        exit(1);
    }
    socklen_t len = sizeof(client_address);
    if (getsockname(client_socket, (struct sockaddr *) &client_address, &len) == -1)
        perror("getsockname");
    else
        printf("%s:%d is connected to %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), ip_address, port);
    
    printf("Enter a message to send to the server: ");
    fgets(buf, MAX_BUF, stdin);
    int sent_len = send(client_socket, buf, strlen(buf), 0);
    if (sent_len < 0)
    {
        perror("Error sending to socket");
        exit(1);
    }
    memset(buf, 0, MAX_BUF);
    int msg_len = recv(client_socket, buf, MAX_BUF, 0);
    if (msg_len < 0)
    {
        perror("Error receiving from socket");
        exit(1);
    }
    printf("Received from server: %s\n", buf);
    close(client_socket);
}

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <ip_address> <port> <mode>\n", argv[0]);
        printf("<mode> can be 'c' for client or 's' for server\n");
        exit(1);
    }
    const char *ip_address = argv[1];
    int port = atoi(argv[2]);
    char mode = argv[3][0];
    if (mode == 'c')
        run_client(ip_address, port);
    else if (mode == 's')
        run_server(ip_address, port);
    else
        printf("Invalid mode specified");
    return 0;
}