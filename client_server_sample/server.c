#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

void handle_client(int client_sockfd)
{
    char recv_buffer[1024] = {0};
    ssize_t bytes_received;

    while(1)
    {
        bytes_received = recv(client_sockfd, recv_buffer, sizeof(recv_buffer), 0);
        recv_buffer[bytes_received] = '\0';
        printf("Client sent: %s\n", recv_buffer);
        printf("Server send: ");
        fgets(recv_buffer, sizeof(recv_buffer), stdin);
        send(client_sockfd, recv_buffer, strlen(recv_buffer), 0);
    }
}

void main()
{
    int listenfd = -1;
    int connfd = -1;
    struct sockaddr_in server_addr;
    char send_buffer[1024];
    time_t ticks;

    memset(send_buffer, 0, sizeof(send_buffer));
    memset(&server_addr, 0, sizeof(server_addr));

    // Server tao ra socket chuyen listen cac bang tin yeu cau ket noi
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // Server khoi tao dia chi cua chinh no
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(5000);

    // Server phai bind de dinh kem cai IP cua chinh no 
    // vao` socket ma` no tao ra
    bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // Cho phep 10 connect
    listen(listenfd, 10);
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    printf("Connected to client\n");
    handle_client(connfd);

    // while(1)
    // {
    //     // Ham quyet dinh co dong y ket noi hay ko
    //     // Dong y ket noi thi ham accept tra ve` 1 socket moi
    //     // Socket moi dc tao ra nay dung de thiet lap point to point doi voi client 
    //     // Socket ban dau dc sever khoi tao chi? dung` de listen bang tin dac biet(yeu cau ket noi)
    //     connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    //     ticks = time(NULL);
    //     sprintf(send_buffer, "Server reply %s", ctime(&ticks));
    //     write(connfd, send_buffer, strlen(send_buffer));
    //     close(connfd);
    // }
    close(listenfd);
    close(connfd);
}