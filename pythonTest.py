import socket

UDP_IP = "12.0.0.3"
UDP_PORT = 5007
MESSAGE = "Hello, World!"

# print "UDP target IP:", UDP_IP
# print "UDP target port:", UDP_PORT
# print "message:", MESSAGE

sock = socket.socket(socket.AF_INET, # Internet
             socket.SOCK_DGRAM) # UDP
sock.sendto(b"Hello, World!", (UDP_IP, UDP_PORT))