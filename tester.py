import socket
import threading
import time
# Informations de connexion au serveur IRC
server_ip = "localhost"
server_port = 8080
server_password = "1234"


# Liste de pseudos disponibles
available_nicks = []


for i in range(1, 50):
    nick = "client_bot{}".format(i)
    available_nicks.append(nick)

def connect_irc(nick):
    irc_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    irc_socket.connect((server_ip, server_port))

    irc_socket.send("PASS {}\r\n".format(server_password).encode())
    time.sleep(0.01)
    irc_socket.send("USER {} 0 * :Client Bot\r\n".format(nick).encode())
    time.sleep(0.01)
    irc_socket.send("NICK {}\r\n".format(nick).encode())
    time.sleep(0.01)
    irc_socket.send("JOIN #te\r\n".format(nick).encode())
    time.sleep(0.01)
    irc_socket.send("JOIN #tee\r\n".format(nick).encode())
    time.sleep(0.01)
    irc_socket.send("JOIN #teee\r\n".format(nick).encode())
    time.sleep(0.0001)

    while True:
        message = irc_socket.recv(2048).decode()
        print(message)

for nick in available_nicks:
    thread = threading.Thread(target=connect_irc, args=(nick,))
    thread.start()