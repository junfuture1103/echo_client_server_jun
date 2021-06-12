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

    struct sockaddr_in addr_server = {};
    struct sockaddr_in addr_client = {};
    socklen_t addr_client_len = sizeof(addr_client);

    int sock_server = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    if(sock_server == -1){
        printf("socket error\n");
        close(sock_server);
        exit(1);
    }

    if(bind(sock_server, (sockaddr*)&addr_server, sizeof(addr_server))==-1){
        printf("bind error\n");
        close(sock_server);
        exit(1);
    }

    if(listen(sock_server, BACKLOG)==-1){
        printf("listen error\n");
        close(sock_server);
        exit(1);
    }

    int sock_client = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
    if(sock_client == -1){
        printf("accept error\n");
        close(sock_server);
        exit(1);
    }

    char msg_to_client[] = "Hello this is jun server";
    write(sock_client, msg_to_client, sizeof(msg_to_client));

    close(sock_client);
    close(sock_server);

    return 0;
}
