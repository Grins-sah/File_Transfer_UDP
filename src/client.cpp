#include<bits/stdc++.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include "packet.hpp"
#include<sys/select.h>

using namespace std;
off_t get_file_size(int fd){
    struct stat st;
    if(fstat(fd,&st)==0){
        return st.st_size;
    }
    return -1;
}
int main(){
    cout<<"Enter the file to upload : ";
    string path;
    getline(cin,path);
    string name;
    cout<<"Enter the file name :";
    getline(cin,name);

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
    st.size = name.size();
    st.seq_no = INT16_MAX;
    memcpy(&st.data,name.c_str(),name.size());
    st.checksum = crc32(st.data,name.size());
    char out[sizeof(packet)];
    size_t send_size = serialize(st,out);
    sendto(
        sockfd,
        out,
        send_size,
        0,
        (sockaddr*)&server_addr,
        sizeof(server_addr)
    );
    int cnt = 0;

    while( (nbyte =read(fd,buffer,sizeof(buffer)))>0){
        sleep(1);
        struct packet p{};
        p.seq_no = cnt;
        p.size = nbyte;
        memcpy(p.data,buffer,nbyte);
        p.checksum = crc32(p.data,p.size);
        size_t out_len = serialize(p,out);
        sendto(
            sockfd,
            out,
            out_len,
            0,
            (sockaddr*)&server_addr,
            sizeof(server_addr)
        );
        cout<<"packet "<<cnt<<" is sent to server"<<endl;
        
        sockaddr_in from_addr{};
        socklen_t from_len = sizeof(from_addr);
        char buffer[1024];
        while(true){
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sockfd,&readfds);
            timeval timeout{};
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            int res = select(sockfd+1,&readfds,NULL,NULL,&timeout);
            if(res>0){
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
                auto a = deserialize_ask(buffer);
                bool flag = false;
                if(a.seq_no==cnt && verify_checksum_ack(a)){
                    cout<<"Recieved the acknowledgement for : "<<a.seq_no<<endl;
                    cnt++;
                    flag = true;
                    break;
                }else{
                    cout<<"Retransmission of the packet "<<cnt<<" "<<a.seq_no<<endl;
                    sendto(
                        sockfd,
                        out,
                        out_len,
                        0,
                        (sockaddr*)&server_addr,
                        sizeof(server_addr)
                    );
                }
                if(flag){
                    break;
                }
            }else if(res==0){
                cout<<"Retransmission of the packet "<<cnt<<endl;
                sendto(
                    sockfd,
                    out,
                    out_len,
                    0,
                    (sockaddr*)&server_addr,
                    sizeof(server_addr)
                );
            }else{
                perror("select");
                break;
            }
            
        }

    }
    st.size = name.size();
    st.seq_no = INT16_MAX-1;
    memcpy(&st.data,name.c_str(),name.size());
    st.checksum = crc32(st.data,name.size());
    send_size = serialize(st,out);
    send_size = serialize(st,out);
    sendto(
        sockfd,
        out,
        send_size,
        0,
        (sockaddr*)&server_addr,
        sizeof(server_addr)
    );
    close(sockfd);
    close(fd);


}