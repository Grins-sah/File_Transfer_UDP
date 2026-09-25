#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

using namespace std;

int main(){
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0){
        perror("Socket");
        return 1;
    }
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd,(sockaddr*)&server_addr,sizeof(server_addr))<0){
        perror("Bind");
        close(sockfd);
        return 1;
    }
    cout<<"The udp server is listening on port 8080 host to server hton"<<endl;
    char buffer[1024];
    while(true){
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        ssize_t bytes_received = recvfrom(
            sockfd,
            buffer,
            sizeof(buffer)-1,
            0,
            (sockaddr*)&client_addr,
            &client_len
        );
        if(bytes_received<=0){
            perror("recvfrom");
            continue;
        }
        buffer[bytes_received] = '\0';
        cout<<"Client : "<<buffer<<endl;
        const char* response = "Message Received";
        sendto(
            sockfd,
            response,
            strlen(response),
            0,
            (sockaddr*)&client_addr,
            client_len
        );
    }
    close(sockfd);
    return 0;

}