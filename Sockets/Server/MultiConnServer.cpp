#include <Server/MultiConnServer.h>

int socket_on_port(char *port);

int socket_on_port(char *port) {
  addrinfo hints, *res;
  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(NULL,port,&hints,&res);
  int s = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  if(s < 0) {
    printf("%s\n",strerror(errno));
    exit(1);
  }
  int bind_code = bind(s,res->ai_addr,res->ai_addrlen);
  freeaddrinfo(res);
  if(bind_code) {
    printf("%s\n",strerror(errno));
    exit(1);
  }
  fcntl(s,F_SETFL,O_NONBLOCK);
  return s;
}


MultiConnServer::MultiConnServer() {
  num_readfds = 0;
  l_sock = 0;
  readfds.clear();
}

MultiConnServer::~MultiConnServer() {
  server_close();
}

fd_set MultiConnServer::convertfds(set<int> &a) {
  fd_set res;
  FD_ZERO(&res);
  for(set<int>::iterator it = readfds.begin(); it != readfds.end(); it++) {
    FD_SET(*it,&res);
  }
  return res;
}

int MultiConnServer::max_socket() {
  int res = max(0,l_sock);
  for(set<int>::iterator it = readfds.begin(); it != readfds.end(); it++) {
    res = max(*it,res);
  }
  return res;
}

void MultiConnServer::accept_new_conns(fd_set *read_fdset) {
  //while have incoming conn, maybe accept with tiemout of close to zero
  //refactor this. cant have this dependency
  if(!FD_ISSET(l_sock,read_fdset)) {
    return;
  }
  int new_fd = 0;
  sockaddr their_addr;
  socklen_t addr_size = sizeof their_addr;
  printf("BEFORE FIRST ACCEPT CALL\n");
  new_fd = accept(l_sock,&their_addr,&addr_size);
  printf("AFTER FIRST ACCEPT CALL\n");
  while(new_fd > 0) {
    if(readfds.find(new_fd) == readfds.end()) {
      num_readfds++;
      readfds.insert(new_fd);
    }
    printf("GOT A NEW CONNECTION %d\n",new_fd);
    new_fd = accept(l_sock,&their_addr,&addr_size);
  }

}

void MultiConnServer::process_single_stream(int socket) {
  char buf[128] = {0};
  int bytes_recvd = recv(socket,buf,128,0);
  if(bytes_recvd == 0) {
    close_read_socket(socket);
  }
  printf("%s\n",buf);
}

void MultiConnServer::close_read_socket(int socket) {
  readfds.erase(socket);
  printf("READ SOCKET %d CLOSED\n",socket);
}

void MultiConnServer::process_incoming_data(fd_set *readyfd_set) {
  for(set<int>::iterator it = readfds.begin(); it != readfds.end(); it++) {
    if(FD_ISSET(*it,readyfd_set)) {
      printf("THIS SOCKET READY %d\n",*it);
      process_single_stream(*it);
    }
  }
}

void MultiConnServer::server_init(char *listening_port) {
  l_sock = socket_on_port(listening_port);
  if(l_sock < 0) {
    printf("FAILED TO GET PORT SOCKET\n");
  }
  listen(l_sock,MAX_LISTEN_QUEUE);
}

void MultiConnServer::server_close() {
  for(set<int>::iterator it = readfds.begin(); it != readfds.end(); it++) {
    close(*it);
  }
  if(l_sock >= 0) {
    close(l_sock);
  }
  readfds.clear();
  l_sock = 0;
  num_readfds = 0;
}

void MultiConnServer::server_side() {
  /*set up socket. listen for new incoming connections. */

  timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  while(true) {
    fd_set readfd_set = convertfds(readfds);
    FD_SET(STDIN,&readfd_set);
    FD_SET(l_sock,&readfd_set);
    //printf("AFTER CONVERT FDS\n");
    int ret_code = select(max_socket() + 1,&readfd_set,NULL,NULL,&tv);
    if(ret_code < 0) {
      printf("ERROR with select\n");
      printf("%s\n",strerror(errno));
      break;
    }
    if(FD_ISSET(STDIN,&readfd_set)) {
      printf("HEHEHE\n");
    }
    if(FD_ISSET(l_sock,&readfd_set)) {
      printf("A NEW CONN\n");
    }
    //printf("FIN SELECT CALL\n");
    accept_new_conns(&readfd_set);
    /* process all incoming data */
    process_incoming_data(&readfd_set);
    if(FD_ISSET(STDIN,&readfd_set)) {
      break;
    }
  }

}
