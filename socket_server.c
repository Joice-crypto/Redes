
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.
void func(int sockfd)
{
    char buff[MAX];
    int n;
    int linha, coluna;
    // infinite loop for chat
    for (;;)
    {
        // limpa o buffer
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));

        // Verificar se o cliente quer sair
        if (strncmp(buff, "sair", 4) == 0)
        {
            printf("Servidor saindo...\n");
            break;
        }

        // Extrair os números da string recebida
        sscanf(buff, "%d %d", &linha, &coluna);

        // Processar os números (neste exemplo, apenas formatando-os de volta)
        //   bzero(buff, MAX);
        snprintf(buff, sizeof(buff), "Recebido linha: %d, coluna: %d", linha, coluna);

        // Enviar a resposta ao cliente
        write(sockfd, buff, sizeof(buff));
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Criação do socket falhou..\n");
        exit(0);
    }
    else
        printf("Socket criado com sucesso..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind falhou...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0)
    {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);
}