#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <errno.h>
#include <set>

#define STDIN 0

#define MAX_LISTEN_QUEUE 10

using namespace std;


class MultiConnServer {
  public:
    MultiConnServer();
    ~MultiConnServer();
    void server_init(char *listening_port);
    void accept_new_conns(fd_set *read_fdset);
    void process_incoming_data(fd_set *readyfd_set);
    void process_single_stream(int socket);
    void server_side();
    void server_close();
  private:

    fd_set convertfds(set<int> &a);
    int max_socket();
    void close_read_socket(int socket);
    int num_readfds;
    int l_sock;
    set<int> readfds;
};
