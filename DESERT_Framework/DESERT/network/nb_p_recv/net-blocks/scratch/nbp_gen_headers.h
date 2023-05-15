#pragma once
#include "nbp_data_queue.h"
struct dynamic_struct_0;
typedef struct dynamic_struct_0 nbp__connection_t;
struct dynamic_struct_0 {
  nbp__accept_queue_t* accept_queue;
  void (*callback_f)(int, nbp__connection_t*);
  unsigned int first_unacked_seq;
  nbp__data_queue_t* input_queue;
  unsigned int last_recv_sequence;
  unsigned int last_sent_sequence;
  unsigned int local_app_id;
  void* redelivery_buffer[32];
  unsigned int remote_app_id;
  char remote_host_id[6];
};
struct dynamic_struct_1;
typedef struct dynamic_struct_1 nbp__net_state_t;
struct dynamic_struct_1 {
  nbp__connection_t* active_connections[512];
  unsigned int active_local_app_ids[512];
  unsigned int active_remote_app_ids[512];
  char active_remote_host_ids[512][6];
  int num_conn;
};
