#include<bits/stdc++.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<iostream>
#include<sys/stat.h>
using namespace std;
off_t get_file_size(int fd){
    struct stat st;
    if(fstat(fd,&st)==0){
        return st.st_size;
    }
    return -1;
}
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
    cout<<"Enter the file to upload : ";
    string path;
    getline(cin,path);

    const int fd = open(path.c_str(),O_RDONLY);
    if(fd<0){
        perror("Open");
        return -1;
    }
    char buffer[1024];
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    if(inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr)<=0){
        perror("inet-pton");
        close(sockfd);
        return 1;
    }
    int nbyte = 0;
    packet st{};
    st.size = INT16_MAX;
    int num_bytes = get_file_size(fd);
    int num_packet = (num_bytes+1023)/1024;
    strcpy(st.data,to_string(num_packet).c_str());
    char out[sizeof(packet)];
    serialize(st,out);
    auto p2 = deserialize(out);
    cout<<"Sending the file containing "<<num_packet<<" packets."<<endl;
    sendto(
        sockfd,
        out,
        sizeof(out),
        0,
        (sockaddr*)&server_addr,
        sizeof(server_addr)
    );
    int cnt = 0;

    while( (nbyte =read(fd,buffer,sizeof(buffer)))>0){
        struct packet p{};
        p.size = nbyte;
        memcpy(p.data,buffer,nbyte);
        serialize(p,out);
        sendto(
            sockfd,
            out,
            sizeof(out),
            0,
            (sockaddr*)&server_addr,
            sizeof(server_addr)
        );
        cout<<"packet "<<cnt<<" is sent to server"<<endl;
        
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
        cout<<"Server : "<<buffer<<" send bytes"<<nbyte<<endl;
        cnt++;
    }
    close(sockfd);
    close(fd);


}