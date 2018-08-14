#include <stdio.h>
#include <Server/MultiConnServer.h>

using namespace std;

int main(int nargs, char** args) {
  if(nargs != 2) {
    printf("Usage <my port>");
    return 1;
  }
  MultiConnServer serv;
  serv.server_init(args[1]);
  printf("FIN SERVER INIT\n");
  serv.server_side();
  printf("FIN SERVER SIDE\n");
  serv.server_close();
  printf("FIN SERVER CLOSE\n");
  return 0;
}
