/*
 * IPK.2023L
 *
 * Demonstration of trivial TCP server.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>

typedef struct
{
	int port_number;
} Config;

typedef enum
{
	ERROR,
	SUCCESS,
} Status;

typedef struct
{
	int capacity;
	int size;
	char *buffer;
} DynamicBuffer;

#define CHECK_ERROR(val)     \
	if (val == ERROR)        \
	{                        \
		return EXIT_FAILURE; \
	}

#define CHECK_PERROR(condition, message) \
	if (condition)                       \
	{                                    \
		perror(message);                 \
		return EXIT_FAILURE;             \
	}

Status GetConfigFromCLArguments(int argc, const char *argv[], Config *config);
Status AddToBuffer(DynamicBuffer *dynBuffer, char *contentToAdd, int length);
void InitBuffer(DynamicBuffer *dynBuffer);
void DestroyBuffer(DynamicBuffer *dynBuffer);

DynamicBuffer dynBuffer;
int welcome_socket = -1;
int comm_socket = -1;

void sigintHandler(int sig_num)
{
	fprintf(stderr, "\nOk, I'm out.\n");
	if (dynBuffer.buffer != NULL)
	{
		DestroyBuffer(&dynBuffer);
	}
	if (welcome_socket != -1)
	{
		close(welcome_socket);
	}
	if (comm_socket != -1)
	{
		close(comm_socket);
	}
	exit(EXIT_SUCCESS);
}

// Receive buffer size (>= 1)
#define RECEIVE_BUFSIZE 8

int main(int argc, const char *argv[])
{
	// Ctrl+C handler
	signal(SIGINT, sigintHandler);

	Config config;

	// Check and parse arguments
	CHECK_ERROR(GetConfigFromCLArguments(argc, argv, &config));

	// Create socket
	welcome_socket = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP, Protocol 0
	CHECK_PERROR(welcome_socket == -1, "Socket");

	// Optionally set socket options
	// SO_REUSEADDR is great for debugging as the same port can be reused in multiple program executions in quick succession
	int optval = 1;
	if (setsockopt(welcome_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("Setsockopt");
	}

	// Prepare server address
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;				 // IPv4
	server_address.sin_addr.s_addr = INADDR_ANY;		 // Listen on all interfaces, i.e. 0.0.0.0
	server_address.sin_port = htons(config.port_number); // Port

	// Assing server address to the socket
	CHECK_PERROR(bind(welcome_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1, "Bind");

	// Listen for connections (with client queue of size 1)
	CHECK_PERROR(listen(welcome_socket, 1) == -1, "Listen");

	printf("INFO: Server listening on port %d\n", config.port_number);
	printf("INFO: Receive buffer size: %d\n", RECEIVE_BUFSIZE);

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	while (true)
	{
		// Accept connection + serve client
		comm_socket = accept(welcome_socket, (struct sockaddr *)&client_addr, &client_len);
		if (comm_socket == -1)
		{
			perror("Accept");
			continue;
		}

		// Prepare buffer for the whole incoming message
		InitBuffer(&dynBuffer);

		// Add header to the message for later transmit back to the client
		char *message_header = "--Your message--\n";
		int message_header_length = strlen(message_header);
		CHECK_ERROR(AddToBuffer(&dynBuffer, message_header, message_header_length))

		// Client IP and port can be printed out
		printf("INFO: Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		// Start receiving message
		char receive_buffer[RECEIVE_BUFSIZE] = {0};
		bool terminate_char_found = false;

		while (!terminate_char_found)
		{
			// Receive bytes into receive buffer
			int received_bytes = recv(comm_socket, receive_buffer, RECEIVE_BUFSIZE, 0);
			if (received_bytes <= 0)
			{
				fprintf(stderr, "No data received from the client!\n");
				break;
			}

			// Client closed his side of the connection
			if (received_bytes == 0)
			{
				receive_buffer[0] == '\0';
			}

			// Check if '\0' is present
			for (int i = 0; i < received_bytes; i++)
			{
				if (receive_buffer[i] == '\0')
				{
					received_bytes = i + 1; // Everything after '\0' can be ignored
					terminate_char_found = true;
					break;
				}
			}

			printf("Received %d bytes\n", received_bytes);
			CHECK_ERROR(AddToBuffer(&dynBuffer, receive_buffer, received_bytes)); // Add received chunk to the rest
		}

		// Complete message can be printed out (without header)
		printf("--Message is complete--\n%s", dynBuffer.buffer + message_header_length);

		// Send message back
		int total_bytes_sent = 0;
		int total_bytes_to_send = dynBuffer.size;
		while (total_bytes_sent != total_bytes_to_send)
		{
			int bytes_sent = send(comm_socket, dynBuffer.buffer + total_bytes_sent, dynBuffer.size - total_bytes_sent, 0);
			if (bytes_sent <= 0)
			{
				fprintf(stderr, "No data has been sent to the client!\n");
				break;
			}
			total_bytes_sent += bytes_sent;
		}

		// Cleanup resources associated with the client
		close(comm_socket);		   // Close the communication socket
		DestroyBuffer(&dynBuffer); // Free memory
		printf("INFO: Client disconnected\n");
	}

	return EXIT_SUCCESS;
}

// Parse CL Arguments
Status GetConfigFromCLArguments(int argc, const char *argv[], Config *config)
{
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		return ERROR;
	}

	// Port
	int port_for_validation = atoi(argv[1]);
	if (port_for_validation < 0 || port_for_validation > USHRT_MAX)
	{
		fprintf(stderr, "Port number out of range\n");
		return ERROR;
	}
	config->port_number = port_for_validation;
}

void InitBuffer(DynamicBuffer *dynBuffer)
{
	dynBuffer->buffer = NULL;
	dynBuffer->capacity = 0;
	dynBuffer->size = 0;
}

void DestroyBuffer(DynamicBuffer *dynBuffer)
{
	free(dynBuffer->buffer);
	dynBuffer->buffer = NULL;
	dynBuffer->capacity = 0;
	dynBuffer->size = 0;
}

Status AddToBuffer(DynamicBuffer *dynBuffer, char *contentToAdd, int length)
{
	if ((dynBuffer->size + length) > dynBuffer->capacity)
	{
		dynBuffer->buffer = realloc(dynBuffer->buffer, dynBuffer->capacity + (length * 3));
		if (dynBuffer->buffer == NULL)
		{
			fprintf(stderr, "Realloc failed!\n");
			return ERROR;
		}
		dynBuffer->capacity = dynBuffer->capacity + (length * 3);
	}

	memcpy(dynBuffer->buffer + dynBuffer->size, contentToAdd, length);
	dynBuffer->size = dynBuffer->size + length;

	return SUCCESS;
}