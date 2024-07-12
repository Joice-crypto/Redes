#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <locale.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(int sockfd1, int sockfd2)
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
            int current_sockfd = (contador % 2 == 0) ? sockfd1 : sockfd2;
            int waiting_sockfd = (contador % 2 == 0) ? sockfd2 : sockfd1;
            char current_player = (contador % 2 == 0) ? 'X' : 'O';

            // Enviar tabuleiro e mensagem de turno para o jogador atual
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

            write(current_sockfd, buff, sizeof(buff));
            snprintf(buff, sizeof(buff), "Olá, jogador %d! É sua vez.\n", (contador % 2) + 1);
            write(current_sockfd, buff, sizeof(buff));

            // Enviar mensagem de espera para o jogador que está aguardando
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
            snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "\nAguarde sua vez...\n");
            write(waiting_sockfd, buff, sizeof(buff)); 
            // Ler a jogada do cliente atual
            bzero(buff, sizeof(buff));
            if (read(current_sockfd, buff, sizeof(buff)) == 0)
            {
               
                printf("Cliente desconectado.\n");
                break;
            }
            sscanf(buff, "%d %d", &linha, &coluna);

            linha--;
            coluna--;

            // Processar a jogada
            if (matriz[linha][coluna] == '.')
            {
                matriz[linha][coluna] = current_player;

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
                    write(sockfd1, buff, sizeof(buff));
                    write(sockfd2, buff, sizeof(buff));
                    close(sockfd1); // Fechar a conexão do lado do servidor
                    close(sockfd2); 
                    exit(0);
                }
            }
            else
            {
                snprintf(buff, sizeof(buff), "O espaço escolhido já está ocupado. Tente novamente.\n");
                write(current_sockfd, buff, sizeof(buff));
                contador--;
                continue;
            }

            // Enviar tabuleiro atualizado para ambos os jogadores
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
            snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff), "\nJogador %d fez a jogada em (%d, %d)\n", (contador % 2) + 1, linha + 1, coluna + 1);
            write(sockfd1, buff, sizeof(buff));
            write(sockfd2, buff, sizeof(buff));
        }

        snprintf(buff, sizeof(buff), "Ninguém ganhou :(\n");
        write(sockfd1, buff, sizeof(buff));
        write(sockfd2, buff, sizeof(buff));
        close(sockfd1); // Fechar a conexão do lado do servidor
        close(sockfd2); 
        exit(0);
    }
}

int main()
{
    int sockfd, connfd1, connfd2, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Criação do socket falhou..\n");
        exit(0);
    }
    else
    {
        printf("Socket criado com sucesso..\n");
    }
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
    {
        printf("Socket successfully binded..\n");
    }

    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
    {
        printf("Server listening..\n");
    }
    len = sizeof(cli);

    // Aceitar o primeiro cliente
    connfd1 = accept(sockfd, (SA *)&cli, &len);
    if (connfd1 < 0)
    {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
    {
        printf("Server accepted the first client...\n");
    }

    // Aceitar o segundo cliente
    connfd2 = accept(sockfd, (SA *)&cli, &len);
    if (connfd2 < 0)
    {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
    {
        printf("Server accepted the second client...\n");
    }

    func(connfd1, connfd2);

    close(sockfd);
}
