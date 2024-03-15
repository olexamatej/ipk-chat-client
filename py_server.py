import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("127.0.0.1", 5553))
server_socket.listen(1)

print("Server is listening on port 5553...")

try:
    client_socket, client_address = server_socket.accept()
    print(f"Accepted connection from {client_address}")
    while True:
        data = client_socket.recv(1024)
        if not data:
            continue
        print(f"Received data: {data.decode()}")
        if data.decode().startswith("AUTH"):
            client_socket.sendall(b"REPLY OK IS Auth success")
            auth = True
        elif data.decode().startswith("JOIN"):
            data = data.decode().split()

            client_socket.sendall(b"REPLY OK IS Join success")
            try:
                client_socket.sendall(f"MSG {data[1]} has joined the chat".encode())
            except:
                pass

        elif data.decode().startswith("MSG"):
            continue
        elif data.decode().startswith("BYE"):
            print("Client is closing the connection...")
            server_socket.close()
            client_socket.close()
            break

# continue in while loop

except KeyboardInterrupt:
    print("\nClosing the server...")
    client_socket.close()
    server_socket.close()
