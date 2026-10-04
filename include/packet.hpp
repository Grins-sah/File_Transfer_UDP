#include<bits/stdc++.h>
#include<arpa/inet.h>

using namespace std;

struct packet{
    uint32_t seq_no;
    uint16_t size;
    uint32_t checksum;
    char data[1024];
};
struct ack{
    uint32_t seq_no;
    uint32_t checksum;
};

uint32_t crc32(const char* data,size_t len){
    uint32_t crc = UINT32_MAX;
    for(size_t i = 0;i<len;i++){
        crc ^= static_cast<uint8_t>(data[i]);
        for(int j = 0;j<8;j++){
            if(crc&1){
                crc = (crc>>1)^0xEDB88320;
            }else{
                crc >>= 1;
            }
        }
    }
    return crc^UINT32_MAX;
}

size_t serialize(packet& p,char* out){
    size_t offset = 0;
    memcpy(out+offset,&p.seq_no,sizeof(p.seq_no));
    offset+=sizeof(p.seq_no);
    memcpy(out+offset,&p.size,sizeof(p.size));
    offset+=sizeof(p.size);
    memcpy(out+offset,&p.checksum,sizeof(p.checksum));
    offset+=sizeof(p.checksum);
    memcpy(out+offset,&(p.data),p.size);
    offset+=p.size;
    return offset;

}
struct packet deserialize(const char* buffer){
    struct packet p;
    size_t offset = 0;
    memcpy(&p.seq_no,buffer+offset,sizeof(p.seq_no));
    offset+=sizeof(p.seq_no);
    memcpy(&p.size,buffer+offset,sizeof(p.size));
    offset+=sizeof(p.size);
    memcpy(&p.checksum,buffer+offset,sizeof(p.checksum));
    offset+=sizeof(p.checksum);
    memcpy(&p.data,buffer+offset,p.size);
    return p;
};

size_t serialize_ack(ack& a, char* out){
    auto seq_no = (a.seq_no);
    auto checksum = (a.checksum);
    size_t offset = 0;
    memcpy(out,&seq_no,sizeof(seq_no));
    offset+=sizeof(seq_no);
    memcpy(out+offset,&checksum,sizeof(checksum));
    offset+=sizeof(checksum);
    return offset;
}

struct ack deserialize_ask(const char* out) {
    struct ack a;
    uint32_t net_seq = 0;
    uint32_t net_chk = 0;
    size_t offset = 0;
    memcpy(&net_seq, out + offset, sizeof(net_seq));
    offset += sizeof(net_seq);
    memcpy(&net_chk, out + offset, sizeof(net_chk));
    a.seq_no = (net_seq);
    a.checksum = (net_chk);
    return a;
}

bool verify_checksum_packet(packet& p){
    return crc32(p.data,p.size)==p.checksum;
}
bool verify_checksum_ack(ack& p){
    return crc32(reinterpret_cast<const char*>(&p.seq_no),sizeof(p.seq_no))==p.checksum;
}