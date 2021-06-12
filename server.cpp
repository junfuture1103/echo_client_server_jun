#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BACKLOG 3
#define TRUE 1
#define FALSE 0

using namespace std;

typedef struct param{
    short echo = FALSE;
    short broad_cast = FALSE;
}param;

void usage(){
    cout << "USAGE : ./server <port>"<< " [-e[-b]]" <<endl;
    cout << "EXAMPLE : ./server 1234"<< " -e -b"<<endl;
    exit(1);
}

void param_parsing(int argc, char** argv, param* option){
    if(argc == 3 && strcmp(argv[2], "-e") == 0){ //echo 옵션이 켜져있다면
        option->echo = TRUE;
    }
    else if(argc == 4 && strcmp(argv[3], "-d") == 0 && strcmp(argv[2], "-e") == 0){ //broad cast 옵션까지 켜져있다면
            option->broad_cast = TRUE;
    }
    else{
        usage();
    }
}

int main(int argc, char* argv[])
{
    param option;

    if(argc < 2 || argc > 4){
        usage();
        return -1;
    }else if (argc >2){
        param_parsing(argc, argv, &option);
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
        printf("accept success.. Hello server to client sock_client : %d\n", sock_client);
        if(option.echo == TRUE){
            if(write(sock_client, "echo", strlen("echo")) == -1){ //client로 보내기
                    printf("write error");
                    exit(1);
            }
        }
        else{
            if(write(sock_client, "none", strlen("none")) == -1){ //client로 보내기
                    printf("write error");
                    exit(1);
            }
        }
    }

    while(1){
        memset(buf, 0, 256); // 버퍼 초기화
        if(read(sock_client, buf, sizeof(buf)-1) == -1){
            printf("read error");
            break;
        }; 
        
        printf("%s\n", buf); //클라한테 받은내용 출력

        //echo 모드가 켜져있다면
        if(option.echo == TRUE){
            if(write(sock_client, buf, strlen(buf)) == -1){ //client로 보내기
                printf("write error");
                break;
            }
        }
    }

    close(sock_client);
    close(sock_server);

    return 0;
   
}