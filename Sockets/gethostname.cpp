#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <errno.h>

using namespace std;

int socket_on_port(char *port) {
  addrinfo hints, *res;
  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(NULL,port,&hints,&res);
  int s = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  if(s < 0) {
    printf("%s\n",strerror(errno));
    exit(1);
  }
  freeaddrinfo(res);
  int ret_code = bind(s,res->ai_addr,res->ai_addrlen);
  if(ret_code != 0) {
    printf("%s\n",strerror(errno));
    exit(1);
  }
  return s;
}

void set_up_server(char *port) {
  int home_socket = socket_on_port(port);
  sockaddr their_addr;
  socklen_t their_addr_size = sizeof their_addr;
  char buf[4096] = {0};
  recvfrom(home_socket,buf,4096,0,&their_addr,&their_addr_size);
  printf("%s\n",buf);
}

void send_dgram(char *home_port, char *away_host, char *away_port) {
  int home_socket = socket_on_port(home_port);
  addrinfo hints,*res;
  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  int ac = getaddrinfo(away_host,away_port,&hints,&res);
  if(ac != 0) {
    printf("FAIL ADDRINFO\n");
    printf("%s\n",strerror(errno));
  }
  char msg[4096] = {'a','b','x','y','z'};
  sockaddr their_addr;
  sendto(home_socket,msg,4096,0,res->ai_addr,res->ai_addrlen);

}


int main(int nargs, char** args) {
  /*
  for(int i = 1; i < nargs; i++) {
    print_addr_info(args[i]);
  }
  */
  if(nargs != 5) {
    printf("Usage <myport> <their_ip> <their_port> <whichone>");
    return 1;
  }
  if(args[4][0] == '1') {
    set_up_server(args[1]);

  } else {
    send_dgram(args[1],args[2],args[3]);
  }
  return 0;
}
