import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('127.0.0.1', 5553))
server_socket.listen(1)

print("Server is listening on port 5553...")

try:
    client_socket, client_address = server_socket.accept()
    print(f"Accepted connection from {client_address}")
    while True:
        data = client_socket.recv(1024)
        if not data:
            break
        print(f"Received data: {data.decode()}")
        
except KeyboardInterrupt:
    print("\nClosing the server...")
    client_socket.close()
    server_socket.close()