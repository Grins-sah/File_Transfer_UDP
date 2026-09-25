#include<bits/stdc++.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include "packet.hpp"
using namespace std;
int rename_by_fd(int fd, const char *new_name) {
    char proc_path[256];
    char actual_path[PATH_MAX];
    // 1. Construct the /proc path for this file descriptor
    snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);
    // 2. Read the symbolic link to get the real path
    ssize_t len = readlink(proc_path, actual_path, sizeof(actual_path) - 1);
    if (len == -1) {
        perror("readlink failed");
        return -1;
    }
    actual_path[len] = '\0'; // Null-terminate the string
    // 3. Rename the file using its real path
    if (rename(actual_path, new_name) == -1) {
        perror("rename failed");
        return -1;
    }

    return 0;
}
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
        if(p.seq_no==INT16_MAX){
            p.data[p.size]=='\0';
            cout<<"Request has came to receive "<<p.data<<" packets"<<" "<<p.data<<endl;
            rename_by_fd(fd,p.data);
            continue;
        }

        write(fd,p.data,(p.size));
        cout<<"Received the packet byte"<<p.size<<" with the seq_no "<<p.seq_no<<endl;
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