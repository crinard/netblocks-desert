#include "nbp_runtime.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define DESERT 1
struct data_queue_t* nbp__new_data_queue(void) {
  struct data_queue_t* q = malloc(sizeof(struct data_queue_t));
  q->current_elems = 0;
  return q;
}
void nbp__free_data_queue(struct data_queue_t* q) { free(q); }
void nbp__insert_data_queue(struct data_queue_t* q, char* buff, int len) {
  int index = q->current_elems;
  q->current_elems++;
  q->data_queue_elems[index] = buff;
  q->data_queue_elems_size[index] = len;
}

nbp__accept_queue_t* nbp__new_accept_queue(void) {
  nbp__accept_queue_t* q = malloc(sizeof(*q));
  q->current_elems = 0;
  return q;
}
void nbp__free_accept_queue(nbp__accept_queue_t* q) { free(q); }
void nbp__insert_accept_queue(nbp__accept_queue_t* q, unsigned src_app_id,
                              char* src_host_id, void* packet) {
  int index = q->current_elems;
  q->current_elems++;
  q->src_app_id[index] = src_app_id;
  memcpy(q->src_host_id[index], src_host_id, HOST_IDENTIFIER_LEN);
  q->packet[index] = packet;
}

int nbp__read(nbp__connection_t* c, char* buff, int max_len) {
  if (c->input_queue->current_elems == 0) return 0;
  char* payload = c->input_queue->data_queue_elems[0];
  int payload_size = c->input_queue->data_queue_elems_size[0];
  int i;
  // TODO: Replace this with a circular queue
  for (i = 1; i < c->input_queue->current_elems; i++) {
    c->input_queue->data_queue_elems[i - 1] =
        c->input_queue->data_queue_elems[i];
    c->input_queue->data_queue_elems_size[i - 1] =
        c->input_queue->data_queue_elems_size[i];
  }
  c->input_queue->current_elems--;
  int copy_size = max_len < payload_size ? max_len : payload_size;
  memcpy(buff, payload, copy_size);
  return copy_size;
}

nbp__connection_t* nbp__accept(nbp__connection_t* c,
                               void (*callback)(int, nbp__connection_t*)) {
  if (c->accept_queue->current_elems == 0) return NULL;

  // We are popping from back, because order doesn't matter
  // TODO: This needs to be fixed!!
  // We should bump from the front otherwise we will get out of order packet
  // and reordering of packets will cause drop
  int index = c->accept_queue->current_elems - 1;
  c->accept_queue->current_elems--;
  unsigned int src_app_id = c->accept_queue->src_app_id[index];
  char* src_host_id = c->accept_queue->src_host_id[index];
  unsigned int dst_app_id = c->local_app_id;
  void* packet = c->accept_queue->packet[index];
  nbp__connection_t* s =
      nbp__establish(src_host_id, src_app_id, dst_app_id, callback);

  // If is okay to pass 0 for now, because we know we aren't using the size
  // TODO: Fix this later by storing size
  if (packet) nbp__run_ingress_step(packet, 0);

  int i;
  for (i = 0; i < c->accept_queue->current_elems; i++) {
    if (c->accept_queue->src_app_id[i] == src_app_id &&
        memcmp(c->accept_queue->src_host_id[i], src_host_id,
               HOST_IDENTIFIER_LEN) == 0) {
      void* packet = c->accept_queue->packet[i];
      if (packet) nbp__run_ingress_step(packet, 0);
      // Swap this element with the last one
      int last = c->accept_queue->current_elems - 1;
      c->accept_queue->src_app_id[i] = c->accept_queue->src_app_id[last];
      memcpy(c->accept_queue->src_host_id[i],
             c->accept_queue->src_host_id[last], HOST_IDENTIFIER_LEN);
      c->accept_queue->packet[i] = c->accept_queue->packet[last];
      // decrement i so we don't miss a packet
      i--;
      // Bump the last elemnt
      c->accept_queue->current_elems--;
    }
  }

  return s;
}
#define REDELIVERY_BUFFER_SIZE (32)
static void nbp__cycle_connections(void) {
  int i;
  unsigned long long time_now = nbp__get_time_ms_now();
  for (i = 0; i < nbp__net_state->num_conn; i++) {
    nbp__connection_t* c = nbp__net_state->active_connections[i];
    // First check for accepts
    if (c->accept_queue->current_elems) {
      c->callback_f(QUEUE_EVENT_ACCEPT_READY, c);
    }
    // Now check for read ready
    if (c->input_queue->current_elems) {
      c->callback_f(QUEUE_EVENT_READ_READY, c);
    }
  }
}

unsigned long long nbp__time_now = -1;
void nbp__main_loop_step(void) {
  // struct timespec tv;
  // clock_gettime(CLOCK_MONOTONIC, &tv);
  // nbp__time_now = tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
  // #ifdef DESERT
  nbp__time_now = (int)(nbp__desert_get_time() * 10);
  // #endif
  int len = 0;

  // Something that is not NULL;
  char* p = (char*)&len;

  // Currently just processing one packet
  // TODO: Get the headroom value from the generated system
  p = nbp__poll_packet(&len, 20);
  if (p != NULL) nbp__run_ingress_step(p, len);
  nbp__cycle_connections();

  nbp__check_timers();
}

// Set this at init
char nbp__my_host_id[6] = {0};

nbp__net_state_t* nbp__net_state;

void nbp__debug_packet(char* p) {
  int i = 0;
  for (i = 0; i < 64; i++) {
    printf("%x ", (unsigned char)p[i]);
  }
  printf("\n");
}

char nbp__wildcard_host_identifier[HOST_IDENTIFIER_LEN] = {0};

unsigned long long nbp__get_time_ms_now(void) {
  if (nbp__time_now == -1) {
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    nbp__time_now = tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
#ifdef DESERT
    nbp__time_now = (int)(nbp__desert_get_time() * 10);
#endif
    // Seconds * 1000 + nanoseconds / 1000000 (10^-3)
  }
  return nbp__time_now;
}
