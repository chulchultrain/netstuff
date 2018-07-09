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

int socket_on_port(char *port);

void print_addr_info(char *host_name) {
  addrinfo hints,*res;

  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(host_name,NULL,&hints,&res);
  printf("%s\n",host_name);

  for(addrinfo *p = res; p != NULL; p = p->ai_next) {
    char ipstr[INET6_ADDRSTRLEN];
    void *void_addr;
    if(p->ai_family == AF_INET) { //ipv4
      struct sockaddr_in *addr4 = (sockaddr_in *) p->ai_addr;
      void_addr = &(addr4->sin_addr);
    } else if(p->ai_family == AF_INET6) { //ipv6
      struct sockaddr_in6 *addr6 = (sockaddr_in6 *) p->ai_addr;
      void_addr = &(addr6->sin6_addr);
    }

    inet_ntop(p->ai_family,void_addr,ipstr,sizeof ipstr);

    printf("%s\n",ipstr);
  }
  freeaddrinfo(res);
}



int socket_on_port(char *host,char *port) {
  addrinfo hints, *res;
  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(NULL,port,&hints,&res);
  int s_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  bind(s_fd,res->ai_addr,res->ai_addrlen);
  return s_fd;
}

void set_up_server(char *home_port) {
  //bind home.
  //getaddrinfo for the host away port.
  printf("PORT IS %s\n",home_port);
  int home_socket = socket_on_port(NULL,home_port);
  printf("socket fd is %d\n",home_socket);
  listen(home_socket,10);
  sockaddr their_addr;
  socklen_t addr_size = sizeof their_addr;
  int new_fd = 0;
  int i = 1;
  //this_thread::sleep_for(chrono::seconds(20));
  while(i < 4) {
    printf("BEFORE ACCEPT %d\n",i);
    new_fd = accept(home_socket,&their_addr,&addr_size);
    printf("ACCEPTED %d\n",i);
    char *msg = new char[4096];

    msg[0] = char(i + '0');
    msg[1] = 'x';
    msg[2] = 'y';
    msg[3] = 'z';
    msg[4] = 0;

    i++;
    send(new_fd,msg,4096,0);
    delete[] msg;
    close(new_fd);
  }
  /*
  printf("%d\n",new_fd);
  printf("HERP\n");

  string str_msg = "HEHEHEHHE";
  char *msg = new char[4096];
  strcpy(msg,str_msg.c_str());
  send(new_fd,msg,4096,0);
  */
  close(home_socket);
}

void set_up_connection(char *home_port,char *away_host, char *away_port) {
  int home_socket = socket_on_port(NULL,home_port);
  /*int flags = fcntl(home_socket,F_GETFL);
  if(flags && O_NONBLOCK) {
    printf("NON BLOCK FLAG IS SET\n");
  }
  //fcntl(home_socket,F_SETFL,O_NONBLOCK);
  */
  printf("HOMESOCKET IS %d\n",home_socket);
  printf("REACHES HERE\n");
  addrinfo hints, *res;
  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  int ac = getaddrinfo(NULL,away_port,&hints,&res);
  printf("AWAY PORT IS %s\n",away_port);
  if(ac != 0) {
    printf("FAIL ADDRINFO\n");
  }
  int ret_code = connect(home_socket,res->ai_addr,res->ai_addrlen);
  if(ret_code == 0) {
      char ca[4096] = {0};
      recv(home_socket,ca,4096,0);
      printf("%s\n",ca);
  } else {
    printf("NO CONNECT\n");
    printf("RETCODE is %d\n",ret_code);
    printf("%s\n",strerror(errno));
  }
  close(home_socket);
}

/*
  be accepting connections in a loop. after waiting a while so that the queue can build up.
  and in this connection return the counter of which connection it is in a datastream.


*/


/*
  client side. open up N sockets all trying to connect to that same port.and once they all
*/

int main(int nargs, char** args) {
  /*
  for(int i = 1; i < nargs; i++) {
    print_addr_info(args[i]);
  }
  */
  if(nargs != 4) {
    printf("Usage <myport> <their_port> <whichone>");
    return 1;
  }
  if(args[3][0] == '1') {
    set_up_server(args[1]);

  } else {
    set_up_connection(args[1],NULL,args[2]);
  }
  return 0;
}
