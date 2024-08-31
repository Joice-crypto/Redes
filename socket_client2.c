#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <locale.h>
#include <unistd.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    int linha, coluna;
    int jogador;

    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));

    if (strstr(buff, "Jogador 1") != NULL)
    {
        jogador = 1;
    }
    else if (strstr(buff, "Jogador 2") != NULL)
    {
        jogador = 2;
    }

    printf("%s\n", buff);

    while (1)
    {
        bzero(buff, sizeof(buff));

        if (read(sockfd, buff, sizeof(buff)) == 0)
        {
            printf("Conexão com o servidor foi fechada.\n");
            break;
        }

        printf("%s\n", buff);

        if (strstr(buff, "ganhou") != NULL || strstr(buff, "Ninguém ganhou") != NULL)
        {
            break;
        }
        
        if (strstr(buff, "É sua vez.") != NULL)
        {
            printf("Insira a linha (1-3): ");
            scanf("%d", &linha);
            printf("Insira a coluna (1-3): ");
            scanf("%d", &coluna);

            if (linha < 1 || linha > 3 || coluna < 1 || coluna > 3)
            {
                printf("Jogada inválida. Tente novamente.\n");
                continue; 
            }

            snprintf(buff, sizeof(buff), "%d %d", linha, coluna);
            write(sockfd, buff, sizeof(buff));
        }
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Criação do socket falhou...\n");
        exit(0);
    }
    printf("Socket criado com sucesso para Cliente..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Conexão com o servidor falhou...\n");
        exit(0);
    }
    printf("Conectado ao servidor para Cliente..\n");

    func(sockfd);

    close(sockfd);
}
