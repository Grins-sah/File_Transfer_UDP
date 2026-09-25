#include<bits/stdc++.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>

using namespace std;
struct packet{
    uint16_t size;
    char data[1024];
};
void serialize(packet& p,char* out){
    size_t offset = 0;
    memcpy(out+offset,&p.size,sizeof(p.size));
    offset+=sizeof(p.size);
    memcpy(out+offset,&p.data,sizeof(p.data));

}
struct packet deserialize(char* buffer){
    struct packet p;
    size_t offset = 0;
    memcpy(&p.size,buffer+offset,sizeof(p.size));
    offset+=sizeof(p.size);
    memcpy(&p.data,buffer+offset,sizeof(p.data));
    return p;
};
int main(){
    const char* filePath = "received.txt";
    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd<0){
        perror("File");
        return 1;
    }
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t server_addr_len = sizeof(server_addr);
    if(bind(sockfd,(sockaddr*)&server_addr,server_addr_len)<0){
        perror("Bind");
        return -1;
    }
    cout<<"The UDP server is running on the port 8080"<<endl;
    char buffer[sizeof(packet)];
    
    while(true){  
        sockaddr_in client_addr;
        socklen_t client_len= sizeof(client_addr);  
        ssize_t received_bytes = recvfrom(sockfd,
            buffer,
            sizeof(buffer),
            0,
            (sockaddr*)&client_addr,
            &client_len
        );
        if(received_bytes<=0){
            perror("recvfrom");
            continue;
        }
        auto p = deserialize(buffer);
        if(p.size==INT16_MAX){
            cout<<"Request has came to receive "<<p.data<<" packets"<<endl;
            continue;
        }

        write(fd,p.data,(p.size));
        cout<<"Received the packet byte"<<p.size<<endl;
        string msg = "received the bytes "+to_string(received_bytes);
        sendto(
            sockfd,
            msg.c_str(),
            strlen(msg.c_str()),
            0,
            (sockaddr*)&client_addr,
            client_len
        );

    }
    close(sockfd);

}