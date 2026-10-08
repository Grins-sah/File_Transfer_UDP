#include<iostream>
#include<arpa/inet.h>
#define RETRY_LIMIT 5
using namespace std;
struct ClientKey{
    uint32_t ip;
    uint16_t port;
    
    bool operator ==(const ClientKey& other) const{
        return ip==other.ip && port ==other.port;
    }
    ClientKey(sockaddr_in t){
        ip = t.sin_addr.s_addr;
        port = t.sin_port;
    }
    
};

struct ClientState{
    uint32_t expected_seq;
    int fd = -1;
    int retry_no = 0;
    string file_name;

};

struct ClientKeyHash{
    size_t operator()(const ClientKey& key) const{
        return hash<uint32_t>()(key.ip)^(hash<uint8_t>()(key.port)<<1);
    }
};