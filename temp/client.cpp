#include<bits/stdc++.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
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
    if(inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr)<=0){
        perror("inet-pton");
        close(sockfd);
        return 1;
    }
    while(true){
        string msg;
        cout<<"Enter the msg : ";
        getline(cin,msg);
        sendto(
            sockfd,
            msg.c_str(),
            msg.size(),
            0,
            (sockaddr*)&server_addr,
            sizeof(server_addr)
        );
        sockaddr_in from_addr{};
        socklen_t from_len = sizeof(from_addr);
        char buffer[1024];
        ssize_t bytes_received = recvfrom(
            sockfd,
            buffer,
            sizeof(buffer)-1,
            0,
            (sockaddr*)&from_addr,
            &from_len
        );
        if(bytes_received <0){
            perror("recvfrom");
            close(sockfd);
            return 1;
        }
        buffer[bytes_received] = '\0';
        cout<<"Server : "<<buffer<<endl;

    }
    close(sockfd);

}
