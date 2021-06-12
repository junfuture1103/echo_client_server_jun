#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BACKLOG 3

using namespace std;

void usage(char* argv){
    cout << "USAGE :" << argv <<" [port]"<<endl;
    cout << "EXAMPLE :" << argv <<" 1234"<<endl;
}

int main(int argc, char* argv[])
{
    if(argc != 2){
        usage(argv[0]);
        return -1;
    }
    char buf[256];
    struct sockaddr_in addr_server = {};
    struct sockaddr_in addr_client = {};
    socklen_t addr_client_len = sizeof(addr_client);
    
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(atoi(argv[1]));
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock_server = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    if(sock_server == -1){
        printf("socket error\n");
        close(sock_server);
        exit(1);
    }

    if(bind(sock_server, (sockaddr*)&addr_server, sizeof(addr_server))==-1){ //bind
        printf("bind error\n");
        close(sock_server);
        exit(1);
    }

    if(listen(sock_server, BACKLOG)==-1){ //기다리기
        printf("listen error\n");
        close(sock_server);
        exit(1);
    }

    int sock_client = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
    if(sock_client == -1){
        printf("accept error\n");
        close(sock_server);
        exit(1);
    }else{
        printf("accept success.. Hello server to client\n");
    }

    while(1){
        memset(buf, 0, 256); // 버퍼 초기화
        if(read(sock_client, buf, sizeof(buf)-1) == -1){
            printf("read error");
            break;
        }; 
        
        printf("%s\n", buf); //클라한테 받은내용 출력
    }

    close(sock_client);
    close(sock_server);

    return 0;
}
