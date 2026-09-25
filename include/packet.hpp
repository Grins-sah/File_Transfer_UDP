#include<bits/stdc++.h>
using namespace std;
struct packet{
    uint32_t seq_no;
    uint16_t size;
    char data[1024];
};
size_t serialize(packet& p,char* out){
    size_t offset = 0;
    memcpy(out+offset,&p.seq_no,sizeof(p.seq_no));
    offset+=sizeof(p.seq_no);
    memcpy(out+offset,&p.size,sizeof(p.size));
    offset+=sizeof(p.size);
    memcpy(out+offset,&(p.data),p.size);
    offset+=p.size;
    return offset;

}
struct packet deserialize(char* buffer){
    struct packet p;
    size_t offset = 0;
    memcpy(&p.seq_no,buffer+offset,sizeof(p.seq_no));
    offset+=sizeof(p.seq_no);
    memcpy(&p.size,buffer+offset,sizeof(p.size));
    offset+=sizeof(p.size);
    memcpy(&p.data,buffer+offset,p.size);
    return p;
};