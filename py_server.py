import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("127.0.0.1", 4567))
server_socket.listen(1)

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

            client_socket.sendall(b"REPLY OK IS Join success\n")
            try:
                client_socket.sendall(f"MSG {data[1]} has joined the chat\n".encode())
            except:
                pass

        elif data.decode().startswith("MSG"):
            continue
        elif data.decode().startswith("BYE"):
            print("Client is closing the connection...")
            server_socket.close()
            client_socket.close()
            break

except KeyboardInterrupt:
    print("\nClosing the server...")
    server_socket.close()