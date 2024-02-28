/*
 * IPK.2023L
 *
 * Demonstration of trivial TCP client.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

typedef struct
{
  const char *server_hostname;
  const char *port_string;
} Config;

typedef enum
{
  ERROR,
  SUCCESS,
} Status;

#define CHECK_ERROR(val) \
  if (val == ERROR)      \
  {                      \
    return EXIT_FAILURE; \
  }

#define CHECK_PERROR(condition, message, server_info) \
  if (condition)                                      \
  {                                                   \
    perror(message);                                  \
    freeaddrinfo(server_info);                        \
    return EXIT_FAILURE;                              \
  }

#define CHECK_ERROR_MESSAGE(condition, message, server_info) \
  if (condition)                                             \
  {                                                          \
    fprintf(stderr, message);                                \
    freeaddrinfo(server_info);                               \
    return EXIT_FAILURE;                                     \
  }

Status GetConfigFromCLArguments(int argc, const char *argv[], Config *config);
Status GetAddrForDomain(Config *config, struct addrinfo **server_info);

// Sending buffer size
#define SEND_BUFSIZE 1024

// Receive buffer size (<= 1) - does not really matter with TCP
#define RECEIVE_BUFSIZE 4

// Terminate connection character
#define TERMINATE_CHAR '!'

int main(int argc, const char *argv[])
{

  char send_buffer[SEND_BUFSIZE] = {0};
  Config config;

  // Check and parse arguments
  CHECK_ERROR(GetConfigFromCLArguments(argc, argv, &config));

  // Resolve domain to IP address
  struct addrinfo *server_info;
  CHECK_ERROR(GetAddrForDomain(&config, &server_info));

  // Create socket
  int client_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
  CHECK_PERROR(client_socket < 0, "Socket", server_info);

  // Optionally set socket options with setsockopt

  // Set receive timeout to 2 seconds
  struct timeval timeval = {
      .tv_sec = 2};
  CHECK_PERROR(setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeval, sizeof(timeval)) < 0, "Setsockopt", server_info);

  // Connect to the remote server
  CHECK_PERROR(connect(client_socket, server_info->ai_addr, server_info->ai_addrlen) < 0, "Connect", server_info);

  // Get messages from stdin and send them to the server
  printf("INFO: Connected to the server!\n");
  printf("Type your messages, terminate with '%c' or Control+D\n", TERMINATE_CHAR);

  bool terminate_char_found = false;
  while (!terminate_char_found)
  {
    // Read bufsize or a single line
    if (fgets(send_buffer, SEND_BUFSIZE, stdin) == NULL) // fgets reads at most BUFSIZE - 1
    {
      send_buffer[0] = TERMINATE_CHAR; // In case of EOF, just insert TERMINATE_CHAR
    }

    // Check terminating character
    char *terminate_character = strchr(send_buffer, TERMINATE_CHAR);
    if (terminate_character != NULL)
    {
      *terminate_character = '\0';
      terminate_char_found = true;
    }

    // Send message
    int total_bytes_sent = 0;
    int total_bytes_to_send = terminate_char_found ? strlen(send_buffer) + 1 : strlen(send_buffer);

    while (total_bytes_sent != total_bytes_to_send)
    {

      // Include \0 if user terminated input
      int size_to_send = terminate_char_found ? strlen(send_buffer + total_bytes_sent) + 1 : strlen(send_buffer + total_bytes_sent);
      int bytes_sent = send(client_socket, send_buffer + total_bytes_sent, size_to_send, 0);
      if (bytes_sent <= 0)
      {
        fprintf(stderr, "No data has been sent to the server!\n");
      } 
      CHECK_PERROR(bytes_sent < 0, "Send failed!", server_info);
      total_bytes_sent += bytes_sent;
    }
  }

  // Receive message back from the server, message is terminated with \0
  char receive_buffer[RECEIVE_BUFSIZE] = {0};

  terminate_char_found = false;
  while (!terminate_char_found)
  {
    int received_bytes = recv(client_socket, receive_buffer, RECEIVE_BUFSIZE, 0);
    if (received_bytes <= 0)
    {
      fprintf(stderr, "No data received from the server!\n");
      break;
    }

    // Print message, check if \0 was present in the message
    for (int i = 0; i < received_bytes; i++)
    {
      printf("%c", receive_buffer[i]);
      if (receive_buffer[i] == '\0')
      {
        terminate_char_found = true;
        break;
      }
    }
  }
  printf("Terminating ...\n");

  // Close the socket
  close(client_socket);

  // Free addrInfo
  freeaddrinfo(server_info);

  return EXIT_SUCCESS;
}

// Parse CL Arguments
Status GetConfigFromCLArguments(int argc, const char *argv[], Config *config)
{
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
    return ERROR;
  }

  // Port
  int port_for_validation = atoi(argv[2]);
  if (port_for_validation < 0 || port_for_validation > USHRT_MAX)
  {
    fprintf(stderr, "Port number out of range\n");
    return ERROR;
  }
  config->port_string = argv[2];

  // Hostname
  config->server_hostname = argv[1];
}

// Get IPv4 address for domain and port in config
Status GetAddrForDomain(Config *config, struct addrinfo **server_info)
{
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_protocol = 0;           // Protocol

  int status = getaddrinfo(config->server_hostname, config->port_string, &hints, server_info);

  if (status != 0 || (*server_info)->ai_addr == NULL)
  {
    fprintf(stderr, "getaddrinfo: failed to resolve hostname!\n");
    return ERROR;
  }

  return SUCCESS;
}