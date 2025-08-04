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

void main()
{
    // Khai bao socket de connect server
    int sockfd = -1;
    // Khai bao struct chua dia chi server
    struct sockaddr_in server_addr;
    // Khai bao buffer chua data gui/nhan voi server
    char recv_buffer[1024] = {0};
    char send_buffer[1024] = {0};
    // Bien time dinh kem voi bang tin
    time_t ticks;
    ssize_t bytes_received;
    
    memset(recv_buffer, 0, sizeof(recv_buffer));
    memset(&server_addr, 0, sizeof(server_addr));

    // Khai bao struct address
    // Khoi tao socket
    // Thong so domain: AF_INET -> Chi dinh bang tin phai di ra ben ngoai
    // Data truyen theo kieu SOCK_STRESM 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Khoi tao struct dia chi server
    server_addr.sin_family = AF_INET;
    // Convert chuoi string IP cua server ve dia chi IP theo kieu so nguyen
    // De dung trong mang network
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Dia chi Port
    server_addr.sin_port = htons(5000);

    // bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // Client phai connect voi server
    // Thanh cong tra ve 0, Fail tra ve so khac
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))== 0)
    {
        // Client doc gia tri server tra ve
        // read(sockfd, recv_buffer, sizeof(recv_buffer) - 1);
        printf("Connected to server\n");
    }
    while(1)
    {
        printf("Client: ");
        fgets(send_buffer, sizeof(send_buffer), stdin);
        send(sockfd, send_buffer, strlen(send_buffer), 0);

        // Client nhan du lieu tu server
        bytes_received = recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);
        recv_buffer[bytes_received] = '\0';
        printf("Server sent: %s\n", recv_buffer);
    }

    close(sockfd);
}