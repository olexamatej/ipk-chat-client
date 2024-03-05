import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('127.0.0.1', 5553))
server_socket.listen(1)

print("Server is listening on port 5553...")

try:
    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Accepted connection from {client_address}")
        client_socket.sendall(b"Rytmus baro frajeris\n")
        client_socket.close()

except KeyboardInterrupt:
    print("\nClosing the server...")
    server_socket.close()