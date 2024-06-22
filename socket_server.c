#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <locale.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    int n;
    int i, j, linha, coluna, contador;
    char matriz[3][3] = {{'.', '.', '.'}, {'.', '.', '.'}, {'.', '.', '.'}};

    for (;;)
    {
        // Limpar o buffer
        bzero(buff, MAX);
        setlocale(LC_ALL, "");

        for (contador = 0; contador < 9; contador++)
        {
            // Enviar tabuleiro e mensagem de turno
            bzero(buff, sizeof(buff));
            snprintf(buff, sizeof(buff), "Tabuleiro atual:\n");
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "%c ", matriz[i][j]);
                }
                snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "\n");
            }
            snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "\nJogador %d, é a sua vez.\n", (contador % 2) + 1);
            write(sockfd, buff, sizeof(buff));

            // Ler a jogada do cliente
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            sscanf(buff, "%d %d", &linha, &coluna);

            linha--;
            coluna--;

            // Processar a jogada
            if (matriz[linha][coluna] == '.')
            {
                matriz[linha][coluna] = (contador % 2) ? 'O' : 'X';

                bzero(buff, sizeof(buff));
                snprintf(buff, sizeof(buff), "Tabuleiro atual:\n");
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "%c ", matriz[i][j]);
                    }
                    snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "\n");
                }

                // Verificar se alguém ganhou
                if ((matriz[0][0] == matriz[0][1] && matriz[0][0] == matriz[0][2] && matriz[0][0] != '.') ||
                    (matriz[1][0] == matriz[1][1] && matriz[1][0] == matriz[1][2] && matriz[1][0] != '.') ||
                    (matriz[2][0] == matriz[2][1] && matriz[2][0] == matriz[2][2] && matriz[2][0] != '.') ||
                    (matriz[0][0] == matriz[1][0] && matriz[0][0] == matriz[2][0] && matriz[0][0] != '.') ||
                    (matriz[0][1] == matriz[1][1] && matriz[0][1] == matriz[2][1] && matriz[0][1] != '.') ||
                    (matriz[0][2] == matriz[1][2] && matriz[0][2] == matriz[2][2] && matriz[0][2] != '.') ||
                    (matriz[0][0] == matriz[1][1] && matriz[0][0] == matriz[2][2] && matriz[0][0] != '.') ||
                    (matriz[0][2] == matriz[1][1] && matriz[0][2] == matriz[2][0] && matriz[0][2] != '.'))
                {
                    snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "\nJogador %d ganhou!\n", (contador % 2) + 1);
                    write(sockfd, buff, sizeof(buff));
                    close(sockfd); // Fechar a conexão do lado do servidor
                    exit(0);
                }
            }
            else
            {
                snprintf(buff, sizeof(buff), "O espaço escolhido já está ocupado. Tente novamente.\n");
                write(sockfd, buff, sizeof(buff));
                contador--;
                continue;
            }

            write(sockfd, buff, sizeof(buff));
        }

        snprintf(buff, sizeof(buff), "Ninguém ganhou :(\n");
        write(sockfd, buff, sizeof(buff));
        close(sockfd); // Fechar a conexão do lado do servidor
        exit(0);
    }
}

int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Criação do socket falhou..\n");
        exit(0);
    }
    else
        printf("Socket criado com sucesso..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind falhou...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0)
    {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

    func(connfd);

    close(sockfd);
}
