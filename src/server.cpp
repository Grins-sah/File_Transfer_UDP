#include<bits/stdc++.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include "packet.hpp"
#include "client.hpp"
using namespace std;
int main(){
    int fd = -1;
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
    unordered_map<ClientKey,ClientState,ClientKeyHash> m;

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
        ClientKey client_key(client_addr);
        if(m.count(client_key)==0){
            m[client_key] = ClientState();
        }
        auto& status = m[client_key];

        if(received_bytes<=0){
            perror("recvfrom");
            continue;
        }
        auto p = deserialize(buffer);
        if(!verify_checksum_packet(p)){
            cerr<<"Checksum for the packet "<<p.seq_no<<" is wrong"<<endl;
            continue;
        }
        if(p.seq_no==INT16_MAX){
            p.data[p.size]='\0';
            // cout<<"New file request of size "<<p.size<<endl;
            char file_name[1024];
            memcpy(file_name,p.data,p.size);
            
            cout<<"Request has came to receive "<<file_name<<endl;
            int _fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            status.fd = _fd;
            status.expected_seq = -1;
            status.file_name = file_name;
            status.retry_no = 0;
            continue;
        }
        if(p.seq_no==INT16_MAX-1){
            close(status.fd);
            m.erase(client_key);
            cout<<"file transfer of the "<<status.file_name<<" is Completed"<<endl;
            continue;
        }

        if(status.expected_seq!=p.seq_no){
            ssize_t byte_written = write(status.fd,p.data,(p.size));
            cout<<status.file_name<<" "<<byte_written<<" "<<status.fd<<endl;
            status.expected_seq = p.seq_no;
        }
        cout<<"Received the packet byte"<<p.size<<" with the seq_no "<<p.seq_no<<endl;
        ack a;
        a.seq_no = p.seq_no;
        a.checksum = crc32(reinterpret_cast<const char*>(&a.seq_no),sizeof(a.seq_no));
        auto sz = serialize_ack(a,buffer);
        sendto(
            sockfd,
            buffer,
            sz,
            0,
            (sockaddr*)&client_addr,
            client_len
        );

    }
    close(sockfd);

}