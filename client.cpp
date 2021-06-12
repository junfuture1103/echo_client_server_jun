#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

void usage(char *argv){
    cout << "Usage : " << argv << " [ip] [port]" << endl;
    cout << "Example) " << argv << " 127.0.0.1 1234" << endl;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        usage(argv[0]);
        return -1;
    }

    struct sockaddr_in addr_server = {};

    addr_server.sin_family = AF_INET; 
    addr_server.sin_addr.s_addr = inet_addr(argv[1]); 
    addr_server.sin_port = htons(atoi(argv[2]));

    char buf[256];
    char get_buf[256];

    int sock_client = socket(AF_INET, SOCK_STREAM, 0); // 클라이언트 소켓 생성
    if(sock_client == -1){
        printf("socket error");
        close(sock_client);
        exit(1);
    }

    if(connect(sock_client, (struct sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 연결 요청
        printf("connect error");
        close(sock_client);
        exit(1);
    }
   
    while(1){
        cin >> buf; // 버퍼에 문자열 입력
        memset(get_buf,0,256);
        if(strlen(buf)>255) {
            break;
        }

        if(write(sock_client, buf, strlen(buf)) == -1){ //server로 보내기
            printf("write error");
            break;
        }
        
        if(read(sock_client, get_buf, sizeof(buf)-1) == -1){
            printf("read error");
            break;
        };

        printf("receive from server : %s\n", get_buf);
    }
    close(sock_client); // 연결 종료
    return 0;
}

