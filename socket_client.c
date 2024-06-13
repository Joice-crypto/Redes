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
    int n;
    int i, j, linha, coluna, contador;
    char matriz[3][3];

    printf("\n\t Bem Vindo (a) Ao Jogo da Velha:\n");
    printf("\n\t Jogador 1: X\n");
    printf("\n\t Jogador 2: O\n");

    setlocale(LC_ALL, ""); /*Acentuação*/
    for (i = 0; i < 3; i++)
    {
        putchar('\n');
        for (j = 0; j < 3; j++)
        {
            putchar('\t');
            matriz[i][j] = '.';
            printf("%c", matriz[i][j]);
        }
        putchar('\n');
        putchar('\n');
    }

    for (contador = 0; contador < 9; contador++)
    {
        bzero(buff, sizeof(buff));

        printf("\nInsira a linha em que deverá ser posto seu símbolo:\n");
        scanf("%d", &linha);
        fflush(stdout);

        printf("Insira a coluna em que deverá ser posto seu símbolo:\n");
        scanf("%d", &coluna);
        fflush(stdout);

        // Formatar os números em uma única string
        snprintf(buff, sizeof(buff), "%d %d", linha, coluna);

        // Enviar a string formatada para o servidor
        write(sockfd, buff, sizeof(buff));

        linha--;
        coluna--;

        // Limpar o buffer e ler a resposta do servidor
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));

        printf("Do servidor : %s", buff);
        if ((strncmp(buff, "sair", 4)) == 0)
        {
            printf("Client Sair...\n");
            break;
        }

        putchar('\n');
        if (matriz[linha][coluna] == '.')
        {

            if (contador % 2)
            { /*O módulo por 2 dará 1 ou 0: Se for 1, equivale a true e entra no if,
               *se for 0 equivale a false e entra no else*/
                matriz[linha][coluna] = 'O';
            }
            else
            {
                matriz[linha][coluna] = 'X';
            }
            for (i = 0; i < 3; i++)
            {
                putchar('\n');
                for (j = 0; j < 3; j++)
                {
                    putchar('\t');
                    printf("%c", matriz[i][j]);
                }
                putchar('\n');
                putchar('\n');
            }
            if ((matriz[0][0] == matriz[0][1] && matriz[0][0] == matriz[0][2] && matriz[0][0] != '.') ||
                (matriz[0][0] == matriz[1][1] && matriz[0][0] == matriz[2][2] && matriz[0][0] != '.') ||
                (matriz[0][0] == matriz[1][0] && matriz[0][0] == matriz[2][0] && matriz[0][0] != '.') ||
                (matriz[0][1] == matriz[1][1] && matriz[0][1] == matriz[2][1] && matriz[0][1] != '.') ||
                (matriz[0][2] == matriz[1][2] && matriz[0][2] == matriz[2][2] && matriz[0][2] != '.') ||
                (matriz[1][0] == matriz[1][1] && matriz[1][0] == matriz[1][2] && matriz[1][0] != '.') ||
                (matriz[2][0] == matriz[2][1] && matriz[2][0] == matriz[2][2] && matriz[2][0] != '.') ||
                (matriz[2][0] == matriz[1][1] && matriz[2][0] == matriz[0][2] && matriz[2][0] != '.'))
            {

                printf("\nJogador %d ganhou!", (contador % 2) + 1); /*O jogador que deveria ser 0 é acrescido de 1, tornando-se jogador 1*/
                exit(0);                                            /*O jogador que deveria ser 1 é acrescido de 1, tornando-se jogador 2*/
            }
        }
        else
        {
            printf("\nO espaço escolhido já está ocupado, repita a operação para um espaço válido\n");
            contador--;
        }
        }
    printf("\nNinguém ganhou :(");
    exit(0);
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf(" Criação do socket falhou...\n");
        exit(0);
    }
    else
        printf("Socket criado com sucesso..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Conexão com o servidor falhou...\n");
        exit(0);
    }
    else
        printf("Conectado com o servidor..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}