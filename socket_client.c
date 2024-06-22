#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <locale.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    int linha, coluna;

    printf("\n\tBem Vindo(a) Ao Jogo da Velha:\n");
    printf("\n\tJogador 1: X\n");
    printf("\n\tJogador 2: O\n");

    while (1)
    {
        // Limpar o buffer e ler a mensagem do servidor
        bzero(buff, sizeof(buff));

        if (read(sockfd, buff, sizeof(buff)) == 0)
        {
            // Conexão fechada pelo servidor
            printf("Conexão com o servidor foi fechada.\n");
            break;
        }

        printf("%s\n", buff);

        // Verificar se o jogo terminou
        if (strstr(buff, " Jogador ganhou") != NULL || strstr(buff, "Ninguém ganhou") != NULL)
        {
            break;
        }

        // Pedir a jogada do jogador
        printf("Insira a linha em que deverá ser posto seu símbolo (1-3):\n");
        scanf("%d", &linha);
        fflush(stdout);

        printf("Insira a coluna em que deverá ser posto seu símbolo (1-3):\n");
        scanf("%d", &coluna);
        fflush(stdout);

        // Formatar os números em uma única string
        snprintf(buff, sizeof(buff), "%d %d", linha, coluna);

        // Enviar a string formatada para o servidor
        write(sockfd, buff, sizeof(buff));

        // Limpar o buffer e ler a resposta do servidor
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));

        printf("Do servidor: %s\n", buff);

        // Verificar se o espaço está ocupado e repetir a jogada se necessário
        if (strstr(buff, "O espaço escolhido já está ocupado") != NULL)
        {
            continue;
        }
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Criação do socket falhou...\n");
        exit(0);
    }
    else
        printf("Socket criado com sucesso..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Conexão com o servidor falhou...\n");
        exit(0);
    }
    else
        printf("Conectado ao servidor..\n");

    func(sockfd);

    close(sockfd);
}
