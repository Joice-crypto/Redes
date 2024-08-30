#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>

#define PORT 6881
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Lê a solicitação do cliente
    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0)
    {
        perror("Read error");
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0'; // Termina a string

    // Exibe a solicitação recebida
    printf("Received request:\n%s\n", buffer);

    // Monta a resposta HTTP
    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 13\r\n"
                           "\r\n"
                           "Hello, world!";

    // Envia a resposta para o cliente
    write(client_socket, response, strlen(response));

    // Fecha o socket do cliente
    close(client_socket);
}

int main()
{
    int server_socket, client_socket, max_sd, sd, activity, i;
    int client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    fd_set read_fds;
    struct timeval timeout;

    // Inicializa todos os sockets de clientes para -1
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = -1;
    }

    // Cria o socket do servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Define o socket como reutilizável
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Define o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Faz o bind do socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Faz o listen do socket
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Loop principal
    while (1)
    {
        // Limpa o conjunto de descritores
        FD_ZERO(&read_fds);

        // Adiciona o socket do servidor ao conjunto de descritores
        FD_SET(server_socket, &read_fds);
        max_sd = server_socket;

        // Adiciona os sockets dos clientes ao conjunto de descritores
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i];
            if (sd > 0)
            {
                FD_SET(sd, &read_fds);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        // Define o timeout
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // Aguarda atividade nos sockets
        activity = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0)
        {
            perror("Select error");
        }

        // Verifica se há alguma atividade no socket do servidor
        if (FD_ISSET(server_socket, &read_fds))
        {
            if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addrlen)) < 0)
            {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }
            printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Adiciona o novo socket ao array de sockets de clientes
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == -1)
                {
                    client_sockets[i] = client_socket;
                    break;
                }
            }
        }

        // Verifica a atividade em todos os sockets de clientes
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i];
            if (FD_ISSET(sd, &read_fds))
            {
                handle_client(sd);
                client_sockets[i] = -1; // Remove o socket do array de clientes
            }
        }
    }

    close(server_socket);
    return 0;
}
