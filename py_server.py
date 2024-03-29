import socket
import time

# Create a UDP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind(("127.0.0.1", 4567))

print("Server is listening on port 4567...")

try:
    while True:

        data, client_address = server_socket.recvfrom(1024)
        server_socket.sendto(b"\x00\x00\x01", client_address)
        server_socket.sendto(b"\x01\x00\x01\x01\x00\x01yes\x00", client_address)
        data, client_address = server_socket.recvfrom(1024)

        server_socket.sendto(b"\x04\x00\x01smrt\x00ahojky\x00", client_address)

        # print(f"Accepted connection from {client_address}")
        # print("potvrdzujem");
        # server_socket.sendto(b"\x01\x00\x00\x00\x00\x00nene\x00", client_address)
        # data, client_address = server_socket.recvfrom(1024)

        # time.sleep(2)
        # data, client_address = server_socket.recvfrom(1024)
        # print("prislo")
        # server_socket.sendto(b"\x00\x00\x01", client_address)
        # server_socket.sendto(b"\x01\x00\x01\x01\x00\x01yes\x00", client_address)
        # data, client_address = server_socket.recvfrom(1024)

        print("fajront")
# continue in while loop

except KeyboardInterrupt:
    print("\nClosing the server...")
    server_socket.close()