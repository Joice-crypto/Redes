#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <unistd.h> // Adicionado para o uso de close() e read()

void *Servidor(void *arg)
{
    /*Buffer de entrada (armazena buffer do cliente)*/
    char buffer_do_cliente[1024];
    /*Cast do ponteiro*/
    int sockEntrada = *(int *)arg;

    /*Loop "infinito"*/
    printf("Aguardando as mensagens...\n");
    for (;;)
    {
        /*Le o que vem do cliente*/
        int bytes_read = read(sockEntrada, buffer_do_cliente, sizeof(buffer_do_cliente) - 1);
        if (bytes_read <= 0)
        {
            // Se não conseguiu ler nada ou houve um erro, encerra a conexão
            close(sockEntrada);
            pthread_exit((void *)0);
        }
        buffer_do_cliente[bytes_read] = '\0'; // Certifica-se de que a string esteja terminada em '\0'

        printf("Mensagem recebida: %s\n", buffer_do_cliente);

        // Responde ao cliente com uma mensagem HTTP
        char response[] = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "Content-Length: 17\r\n"
                          "\r\n"
                          "Hello from server";

        write(sockEntrada, response, sizeof(response) - 1);

        // Verifica se o cliente enviou "sair" para fechar a conexão
        if (strstr(buffer_do_cliente, "sair") != NULL)
        {
            printf("Conexão encerrada.\n");
            close(sockEntrada);
            pthread_exit((void *)0);
        }

        printf("Resposta enviada: Hello from server\n");
        // Encerra a conexão após responder (HTTP/1.1 espera conexão fechada após resposta)
        close(sockEntrada);
        pthread_exit((void *)0);
    }
}

int configuracaoServidor()
{
    /*Cria o descritor*/
    int sockfd;
    /*Declaracao da estrutura*/
    struct sockaddr_in serverAddr;
    /*Cria o socket*/
    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Erro no Socket\n");
        exit(1);
    }
    /*Zera a estrutura*/
    memset(&serverAddr, 0, sizeof(serverAddr));
    /*Seta a familia*/
    serverAddr.sin_family = AF_INET;
    /*Seta os IPS (A constante INADDR_ANY e todos os ips ou qualquer ip) htonl -> conversao*/
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /*Define a porta*/
    serverAddr.sin_port = htons(6881);
    /*Faz a bindagem (cola, gruda, conecta seja o que for)*/
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("Erro no Socket\n");
        exit(1);
    }
    /*Fica na escuta de ate 5 clientes*/
    if (listen(sockfd, 5) < 0)
    {
        printf("Erro no Socket\n");
        exit(1);
    }
    return sockfd;
}

int main()
{
    system("clear");
    /*Declaracao da estrutura*/
    struct sockaddr_in serverAddr;
    /*Retorna da funcao e o descritor*/
    int sockfd = configuracaoServidor();

    /*Loop "infinito"*/
    while (1)
    {
        int clienteSockfd;
        struct sockaddr_in clienteAddr;
        /*tamanho da estrutura*/
        unsigned int clntLen;
        clntLen = sizeof(clienteAddr);
        /*declara uma thread*/
        pthread_t thread;
        /*Fica no aguardo da conexao do cliente*/
        if ((clienteSockfd = accept(sockfd, (struct sockaddr *)&clienteAddr, &clntLen)) < 0)
        {
            printf("Erro no Socket\n");
            exit(1);
        }
        /*Inicializa a thread*/
        if (pthread_create(&thread, NULL, Servidor, &clienteSockfd) != 0)
        {
            printf("Erro na Thread\n");
            exit(1);
        }

        pthread_detach(thread);
    }
    exit(0);
}
