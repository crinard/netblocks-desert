#ifndef NBP_RUNTIME_H
#define NBP_RUNTIME_H
#include <stdlib.h>
#include <string.h>
#include "nbp_gen_headers.h"
#include "nbp_timer.h"

#ifdef __cplusplus 
extern "C" {
#endif

#define QUEUE_EVENT_ESTABLISHED (0)
#define QUEUE_EVENT_READ_READY (1)
#define QUEUE_EVENT_ACCEPT_READY (2)

static int nbp__connection_t_size(void) {
	return sizeof(nbp__connection_t);
}

// Data queue methods
struct data_queue_t* nbp__new_data_queue(void);
void nbp__free_data_queue(struct data_queue_t*);
void nbp__insert_data_queue(struct data_queue_t*, char*, int);

// Accept queue methods
nbp__accept_queue_t* nbp__new_accept_queue(void);
void nbp__free_accept_queue(nbp__accept_queue_t*);
void nbp__insert_accept_queue(nbp__accept_queue_t*, unsigned, char*, void*);

void nbp__add_connection(nbp__connection_t*, unsigned sa);
void nbp__delete_connection(unsigned sa);
nbp__connection_t* nbp__retrieve_connection(unsigned sa);


void nbp__debug_packet(char* p);


// Network interface API TODO: implement these -- they have to interface with the 
// bottom of the stack. 
// The top (application layer is written using the code (like follows)), and the 
// application will have the netblocks implementations.
/**
 * nbp__ipc_init("/tmp/ipc_socket", 0);
	printf("IPC initialized\n");
	nbp__net_init();
	memcpy(nbp__my_host_id, client_id, 6);

	nbp__connection_t * conn = nbp__establish(server_id, 8080, 8081, callback);
	
	nbp__send(conn, CLIENT_MSG, sizeof(CLIENT_MSG));

	while (running) {
		nbp__main_loop_step();
		usleep(100 * 1000);
	}
	nbp__destablish(conn);
 */
char* nbp__poll_packet(int*, int);
int nbp__send_packet(char*, int);
void nbp__ipc_init(const char* sock_path, int mode);
void nbp__ipc_deinit();
void nbp__mlx5_init(void);
char* nbp__request_send_buffer(void);
void nbp__return_send_buffer(char*);
void nbp__desert_init(void* _m);
void nbp__desert_deinit(void);
double nbp__desert_get_time(void);

// Generated protocol API
void nbp__run_ingress_step (void*, int);
int nbp__send (nbp__connection_t* arg0, char* arg1, int arg2);
void nbp__destablish (nbp__connection_t* arg0);
nbp__connection_t* nbp__establish (char* arg0, unsigned int arg1, unsigned int arg2, void (*arg3)(int, nbp__connection_t*));
void nbp__net_init (void);
void nbp__reliable_redelivery_timer_cb(nbp__timer*, void* param, unsigned long long to);

// Runtime API
int nbp__read(nbp__connection_t*, char*, int);
nbp__connection_t* nbp__accept(nbp__connection_t*, void (*)(int, nbp__connection_t*));
void nbp__main_loop_step(void);

extern char nbp__reuse_mtu_buffer[];
extern char nbp__my_host_id[];
extern nbp__net_state_t* nbp__net_state;

extern char nbp__wildcard_host_identifier[];


extern unsigned long long nbp__get_time_ms_now(void);
#ifdef __cplusplus 
}
#endif

#endif
