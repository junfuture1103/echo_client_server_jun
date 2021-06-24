#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <sys/socket.h>

#define BACKLOG 5
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
    else if(argc == 4 && (strcmp(argv[3], "-b") == 0) && (strcmp(argv[2], "-e") == 0)){ //broad cast 옵션까지 켜져있다면
        option->echo = TRUE;
        option->broad_cast = TRUE;
    }
    else{
        usage();
    }
}

void* recv_client(int client_sd, param* option, int first_client, int* client_num){
    char buf[256];
    char tmp[256];
    memset(buf,0,sizeof(buf));
    int recv_len;

    while(1){
        memset(tmp,0,sizeof(buf));
        recv_len = read(client_sd, buf, sizeof(buf));
        strncpy(tmp, buf, recv_len);
        printf("get by client : %s\n", tmp); //클라한테 받은내용 출력
        printf("buf_size : %d\n", recv_len);
        if(option->echo){
            if(write(client_sd, tmp, recv_len) == -1){ //client로 보내기
                printf("write error");
            } 
        }
        printf("buf_size : %d", recv_len);
        if(option->broad_cast){
            for (int i = first_client; i<(*client_num)+1; i++){
                if (i != client_sd){
                    if(write(i, tmp, recv_len) == -1){ //client로 보내기
                        printf("write error");
                    } 
                }
            }
        }
    }

    close(client_sd);
}

int main(int argc, char* argv[]){
    param option;

    if(argc < 2 || argc > 4){
        usage();
        return -1;
    }else if (argc >2){
        param_parsing(argc, argv, &option);
    }

    struct sockaddr_in addr_server = {};
    struct sockaddr_in addr_client = {};
    socklen_t addr_client_len = sizeof(addr_client);
    struct timeval timeout;

    int sock_server, sock_client, read_check, client_num;
    int first_sd = 0;

    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(atoi(argv[1]));
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

    sock_server = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성
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

    while(1){
        sock_client = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락 
        if(sock_client == -1){
            printf("accept error\n");
            close(sock_server);
            exit(1);
        }
        printf("accept success.. Hello server to client sock_client : %d\n", sock_client);

        if(first_sd == 0){
            first_sd = sock_client;
        }

        client_num = sock_client;

        thread *t1 = new thread(recv_client, sock_client, &option, first_sd, &client_num);
        t1->detach();

    }

    close(sock_client);
    close(sock_server);

    return 0;
   
}