import socket
import sys

args = sys.argv[1:]
if len(args) != 3:
    print("Usage: <their_host> <their_port> <my_port>")
    sys.exit(1)
serv_host = args[0]
serv_port = args[1]
my_port = args[2]

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((serv_host,int(serv_port)))
s.send("HELLO THERE SERVER")
