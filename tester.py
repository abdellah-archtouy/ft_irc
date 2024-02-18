import socket
import time

server = "localhost"
port = 8080
channel = "#channel_name"
nickname_template = "Nickname{}"
password = "1234"  

irc_clients = []

for i in range(1, 1025):
    nickname = nickname_template.format(i)
    irc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    irc.connect((server, port))
    time.sleep(0.01)
    irc.send("PASS {}\r\n".format(password).encode())
    time.sleep(0.01)
    irc.send("USER {} 0 * :Client Bot\r\n".format(nickname).encode())
    time.sleep(0.01)
    irc.send("NICK {}\r\n".format(nickname).encode())

    irc_clients.append(irc)

while True:
    for irc in irc_clients:
        message = irc.recv(2048).decode("UTF-8")
        print(message)