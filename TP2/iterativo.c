#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Cria um socket TCP/IP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Falha na criação do socket");
        exit(EXIT_FAILURE);
    }

    // Configura o endereço e a porta
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associa o socket ao endereço e porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Falha na associação do socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Configura o servidor para escutar conexões
    if (listen(server_fd, 3) < 0)
    {
        perror("Falha ao configurar o servidor para escutar");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Aguardando conexão...\n");

        // Aceita uma nova conexão
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Falha ao aceitar conexão");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("Conexão aceita.\n");

        // Lê dados do cliente
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        printf("Mensagem recebida: %s\n", buffer);

        // Envia uma resposta para o cliente
        char response[] = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "Content-Length: 17\r\n"
                          "\r\n"
                          "Hello from server";

        send(new_socket, response, strlen(response), 0);
        printf("Mensagem enviada: %s\n", response);

        // Fecha o socket da conexão atual
        close(new_socket);
        printf("Conexão encerrada.\n");
    }

    // Fecha o socket do servidor
    close(server_fd);
    return 0;
}
