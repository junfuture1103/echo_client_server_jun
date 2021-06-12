#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

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
    if(argc == 4 && (strcmp(argv[3], "-b") == 0) && (strcmp(argv[2], "-e") == 0)){ //broad cast 옵션까지 켜져있다면
        option->echo = TRUE;
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
    struct timeval timeout;
    socklen_t addr_client_len = sizeof(addr_client);

    int fd_max, fd_num, sock_server, sock_client, read_check, j;
    
    //for select
    fd_set reads, cpy_reads;

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

    FD_ZERO(&reads);
    FD_SET(sock_server, &reads);
    fd_max = sock_server;

    while(1){
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        fd_num = select(fd_max+1, &cpy_reads, 0,0,&timeout);
        if(fd_num == -1){
            break;
        }
        if(fd_num == 0){
            continue;
        }
        for (int i =0; i<fd_max+1; i++){
            if (FD_ISSET(i, &cpy_reads)){
                if (i == sock_server){
                    sock_client = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
                    if(sock_client == -1){
                        printf("accept error\n");
                        close(sock_server);
                        exit(1);
                    }
                    FD_SET(sock_client, &reads);
                    if(fd_max < sock_client){
                        fd_max = sock_client;
                    }
                    printf("accept success.. Hello server to client sock_client : %d\n", sock_client);
                    
                    if(option.echo != FALSE){
                        if(option.broad_cast == TRUE){
                            if(write(sock_client, "broad", strlen("broad")) == -1){ //client로 보내기
                                    printf("write error");
                                    exit(1);
                            }
                        }else{
                            if(write(sock_client, "echo", strlen("echo")) == -1){ //client로 보내기
                            printf("write error");
                            exit(1);
                            }
                        }

                    }
                    else{
                        if(write(sock_client, "none", strlen("none")) == -1){ //client로 보내기
                                printf("write error");
                                exit(1);
                        }
                    }
                    
                }
                else{
                    memset(buf, 0, 256); // 버퍼 초기화
                    read_check = read(i, buf, sizeof(buf)-1);
                    if(read_check == -1){
                        printf("read error");
                        break;
                    };
                    if(read_check == 0){
                        FD_CLR(i, &reads);
                        close(i);
                        printf("close client: %d \n", i);
                    }
                    
                    printf("%s\n", buf); //클라한테 받은내용 출력
                   
                    //echo 모드가 켜져있다면
                    if(option.echo != FALSE){
                        if(option.broad_cast == TRUE){
                            for (j=4; j<fd_max+1; j++){
                                if (j == i){
                                    continue;
                                }
                                if(write(j, buf, read_check) == -1){ //client로 보내기
                                    printf("write error");
                                    break;
                                }
                            }
                        }else{
                            if(write(i, buf, read_check) == -1){ //client로 보내기
                                printf("write error");
                                break;
                            }
                        }
                    }
                }
            }
        }

    }

    close(sock_client);
    close(sock_server);

    return 0;
   
}