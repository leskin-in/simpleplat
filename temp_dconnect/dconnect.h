#ifndef DCONNECT_INCLUDED
#define DCONNECT_INCLUDED

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#define TICK_TYPE uint64_t
#define ID_TYPE int

#define NET_SERVER_PORT 57575
#define NET_USUAL_REPEAT 3




/// Session net data
struct All_Net_data_t {
	int socket;
	struct sockaddr_in binder;
} D_NET_DATA;


/// Client connection data
struct Client_Net_data_t {
	struct sockaddr_in s_addr;
} D_CLIENT_NET_DATA;


/// Server connection database
struct Server_Net_data_t {
	struct sockaddr_in s_addr;
	socklen_t s_addr_len;

} *D_SERVER_NET_DATA;


struct Client_Net_packet_t {
	int8_t type;
	TICK_TYPE tick;
};





/**
 * Creates an UDP socket and binds it.
 *
 * @param type: Type of connection required
 * = 0: server
 * = 1: client
 * = 2: statistics
 * @param connections: [SERVER-only] Maximum number of connections allowed
 * 	@def: 0
 *
 * @return 0 if successful
 * @return -1 for errors
 *
 * @note Makes D_NET_DATA valid if successful and prepares D_SERVER_NET_DATA
 */
int d_all_connect(int type, int connections);


/**
 * @param type: Type of connection to be closed
 * = 0: server
 * = 1: client
 * = 2: statistics
 */
void d_all_disconnect(int type);


/**
 * Send raw data using UDP protocol
 * Wrapper for <socket.h>::sendto()
 *
 * @param data
 * @param data_length: 'sizeof(data)'
 * @param address
 * @param address_length: 'sizeof(address)'
 * @param repeats: Send UDP packet this number of times
 * 	@def: NET_USUAL_REPEAT
 *
 * @return 0 if successful
 * @return -1 for errors
 */
int d_all_sendraw(const void* data, size_t data_length,
		const struct sockaddr_in* address, size_t address_length,
		size_t repeats);


/**
 * Recieve raw data using UDP protocol
 * Wrapper for <socket.h>::recvfrom()
 *
 * @param data
 * @param data_length: 'sizeof(data)'
 * @param address
 * @param address_length: 'sizeof(address)'
 *
 * @return sizeof(data) actually recieved
 * @return -1 for errors
 */
int d_all_recvraw(void* data, size_t data_length,
		const struct sockaddr_in* address, socklen_t* address_length);






/**
 * CLIENT: Send "hello" datagram to server
 *
 * @param ip: Server ip (must be a valid C string with \0 ending)
 *
 * @return 0 if successful
 * @return -1 for errors
 *
 * @note Makes D_CLIENT_NET_DATA valid if call successful
 */
int d_client_connect(const char* ip);


/**
 * CLIENT: Send (any) datagram to server
 *
 * @param data: Data to send
 * @param data_length
 *
 * @return 0 if successful
 * @return -1 for errors
 *
 * @warning valid D_CLIENT_NET_DATA required
 * @warning data must begin with a 'size8_t' protocol code
 */
int d_client_send(const void* data, size_t data_length);


/**
 * CLIENT: Recieve [time-signed] datagram
 *
 * @param data: Place for data to be put to
 * @param data_length
 * @param tick: Minimum required tick to get data
 * 				If = 0, ignore time signature (not present in data)
 * 				If > 0, ignore all unsual datagrams (safe)
 *
 * @return 0 if successful
 * @return -1 for errors
 *
 * @warning valid D_CLIENT_NET_DATA required
 */
int d_client_get(void* data, size_t data_length, TICK_TYPE tick);






/**
 * SERVER: Send [time-signed] datagram
 *
 * @param data: Data to send
 * @param data_length
 * @param tick: Signing tick
 * 				If = 0, ignore time signature
 *
 * @return 0 if successful
 * @return -1 for errors
 *
 * @warning valid D_SERVER_NET_DATA required
 */
int d_server_send(ID_TYPE id,
		const void* data, size_t data_length,
		TICK_TYPE tick);


/**
 * SERVER: Recieve datagram from client
 *
 * @param data: Place for data to be put to
 * @param data_length
 * @param id_ptr: ID of client sending datagram
 *
 * @return 0 if usual datagram recieved
 * @return 1 if special datagram recieved
 * @return 2 if new client added (and special datagram recieved)
 * @return -1 for errors
 *
 * @note New clients are added automatically
 */
int d_server_get(void* data, size_t data_length, ID_TYPE* id_ptr);


/**
 * SERVER: Manage preferences for selected client id
 *
 * @param id: Client id
 * @param op: Operation to be performed
 * 		0: Test existence & exit
 * 		-1: Remove client
 *
 * @return 0 if successful
 * @return 1 if no such client exists
 * @return -1 for errors
 */
int d_server_manageid(ID_TYPE id, int op);





#endif /* DCONNECT_INCLUDED */
