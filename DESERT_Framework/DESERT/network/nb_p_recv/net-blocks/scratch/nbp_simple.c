#include <stdio.h>

#include "nbp_runtime.h"

void nbp__net_init(void) {
  nbp__init_timers();
  nbp__net_state = malloc(sizeof(nbp__net_state[0]));
  nbp__net_state->num_conn = 0;
}

nbp__connection_t* nbp__establish(char* arg0, unsigned int arg1,
                                  unsigned int arg2,
                                  void (*arg3)(int, nbp__connection_t*)) {
  nbp__connection_t* var8;
  var8 = malloc(sizeof(var8[0]));
  var8->callback_f = arg3;
  var8->last_sent_sequence = 1010;
  var8->last_recv_sequence = 0;
  nbp__connection_t* var28 = var8;
  var28->callback_f(0, var8);
  for (int var33 = 0; var33 < 32; var33 = var33 + 1) {
    var8->redelivery_buffer[var33] = 0;
  }
  memcpy(var8->remote_host_id, arg0, 6);
  var8->remote_app_id = arg1;
  var8->local_app_id = arg2;
  var8->input_queue = nbp__new_data_queue();
  var8->accept_queue = nbp__new_accept_queue();
  int var49 = nbp__net_state->num_conn;
  nbp__net_state->num_conn = var49 + 1;
  nbp__net_state->active_local_app_ids[var49] = arg2;
  nbp__net_state->active_connections[var49] = var8;
  nbp__net_state->active_remote_app_ids[var49] = arg1;
  memcpy(nbp__net_state->active_remote_host_ids[var49], arg0, 6);
  return var8;
}

void nbp__destablish(nbp__connection_t* arg0) {
  int var15 = nbp__net_state->num_conn;
  int var16 = 0;
  if (var16 < var15) {
    while (1) {
      if (((nbp__net_state->active_local_app_ids[var16] ==
            arg0->local_app_id) &&
           (nbp__net_state->active_remote_app_ids[var16] ==
            arg0->remote_app_id)) &&
          (memcmp(nbp__net_state->active_remote_host_ids[var16],
                  arg0->remote_host_id, 6) == 0)) {
        var15 = var15 - 1;
        nbp__net_state->num_conn = var15;
        nbp__net_state->active_local_app_ids[var16] =
            nbp__net_state->active_local_app_ids[var15];
        nbp__net_state->active_connections[var16] =
            nbp__net_state->active_connections[var15];
        nbp__net_state->active_remote_app_ids[var16] =
            nbp__net_state->active_remote_app_ids[var15];
        memcpy(nbp__net_state->active_remote_host_ids[var15],
               nbp__net_state->active_remote_host_ids[var16], 6);
        break;
      } else {
        var16 = var16 + 1;
        if (!(var16 < var15)) {
          break;
        }
      }
    }
  }
  nbp__free_data_queue(arg0->input_queue);
  nbp__free_accept_queue(arg0->accept_queue);
  free(arg0);
}

int nbp__send(nbp__connection_t* arg0, char* arg1, int arg2) {
  unsigned char* var9 = nbp__request_send_buffer();
  int var10 = 0;
  int* var11 = (&(var10));
  var11[0] = arg2;
  memcpy(var9 + 58, arg1, arg2);
  unsigned long long int var19 = 38 + arg2;
  int* var23 = (void*)(var9 + 36);
  var23[0] = var19;
  unsigned int var30 = arg0->last_sent_sequence + 1;
  arg0->last_sent_sequence = var30;
  unsigned int* var36 = (void*)(var9 + 50);
  var36[0] = var30;
  unsigned int* var50 = (void*)(var9 + 50);
  arg0->redelivery_buffer[var50[0] % 32] = var9;
  nbp__timer* var54 = nbp__alloc_timer();
  nbp__insert_timer(var54, nbp__get_time_ms_now() + 50000,
                    nbp__reliable_redelivery_timer_cb, var9);
  unsigned long long int var55 = (unsigned long long)(var54);
  unsigned long long int* var59 = (void*)(var9 + 4);
  var59[0] = var55;
  unsigned long long int var60 = 0;
  unsigned int* var64 = (void*)(var9 + 54);
  var64[0] = var60;
  memcpy(var9 + 20, arg0->remote_host_id, 6);
  unsigned long long int var74 = arg0->remote_app_id;
  int* var78 = (void*)(var9 + 42);
  var78[0] = var74;
  memcpy(var9 + 26, nbp__my_host_id, 6);
  unsigned long long int var82 = arg0->local_app_id;
  int* var86 = (void*)(var9 + 46);
  var86[0] = var82;
  unsigned long long int var87 = 8;
  unsigned int* var91 = (void*)(var9 + 32);
  var91[0] = var87;
  char* var98 = (void*)(var9);
  unsigned long long int var99 = 0;
  unsigned short int* var103 = (void*)(var98 + 40);
  var103[0] = var99;
  int var104 = 20;
  int* var108 = (void*)(var98 + 36);
  int var110 = var108[0] + 20;
  if ((var110 - var104) % 2) {
    var98[var110] = 0;
    var110 = var110 + 1;
  }
  unsigned short int var111 = 0;
  for (char* var112 = var98 + var104; var112 < (var98 + var110);
       var112 = var112 + 2) {
    unsigned short int* var113 = (void*)(var112);
    var111 = var111 + var113[0];
  }
  unsigned short int* var118 = (void*)(var9 + 40);
  var118[0] = var111;
  int* var127 = (void*)(var9 + 36);
  nbp__send_packet(var9 + 20, var127[0]);
  return var10;
}

void nbp__run_ingress_step(void* arg0, int arg1) {
  unsigned short int* var10 = (void*)(arg0 + 40);
  unsigned long long int var11 = var10[0];
  char* var14 = (void*)(arg0);
  unsigned long long int var15 = 0;
  unsigned short int* var19 = (void*)(var14 + 40);
  var19[0] = var15;
  int var20 = 20;
  int* var24 = (void*)(var14 + 36);
  int var26 = var24[0] + 20;
  if ((var26 - var20) % 2) {
    var14[var26] = 0;
    var26 = var26 + 1;
  }
  unsigned short int var27 = 0;
  for (char* var28 = var14 + var20; var28 < (var14 + var26);
       var28 = var28 + 2) {
    unsigned short int* var29 = (void*)(var28);
    var27 = var27 + var29[0];
  }
  unsigned short int* var34 = (void*)(arg0 + 40);
  var34[0] = var11;
  if (var27 == var11) {
    if (!(memcmp(arg0 + 20, nbp__my_host_id, 6) != 0)) {
      int* var42 = (void*)(arg0 + 42);
      unsigned long long int var43 = var42[0];
      int* var48 = (void*)(arg0 + 46);
      unsigned long long int var49 = var48[0];
      unsigned char* var52 = arg0 + 26;
      nbp__connection_t* var57 = 0;
      int var59 = nbp__net_state->num_conn;
      int var60 = 0;
      if (var60 < var59) {
        while (1) {
          if (((nbp__net_state->active_local_app_ids[var60] == var43) &&
               (nbp__net_state->active_remote_app_ids[var60] == var49)) &&
              (memcmp(nbp__net_state->active_remote_host_ids[var60], var52,
                      6) == 0)) {
            var57 = nbp__net_state->active_connections[var60];
            break;
          } else {
            var60 = var60 + 1;
            if (!(var60 < var59)) {
              break;
            }
          }
        }
      }
      if (var57 != 0) {
        unsigned long long int var68 = (unsigned long long)(var57);
        unsigned long long int* var72 = (void*)(arg0 + 12);
        var72[0] = var68;
        unsigned int* var77 = (void*)(arg0 + 54);
        unsigned long long int var78 = var77[0];
        unsigned long long int* var83 = (void*)(arg0 + 12);
        nbp__connection_t* var85 = (void*)(var83[0]);
        if (var78 != 0) {
          unsigned int var86 = var78 % 32;
          unsigned long long int* var92 =
              (void*)(var85->redelivery_buffer[var86] + 4);
          nbp__timer* var94 = (void*)(var92[0]);
          nbp__remove_timer(var94);
          nbp__return_timer(var94);
          var85->redelivery_buffer[var86] = 0;
        } else {
          unsigned int* var99 = (void*)(arg0 + 50);
          unsigned int var101 = var99[0];
          unsigned char* var102 = nbp__request_send_buffer();
          char var103[1] = {0};
          int var104;
          int* var105 = (&(var104));
          unsigned int* var110 = (void*)(var102 + 54);
          var110[0] = var101;
          unsigned long long int var111 = 39;
          int* var115 = (void*)(var102 + 36);
          var115[0] = var111;
          memcpy(var102 + 20, var85->remote_host_id, 6);
          unsigned long long int var125 = var85->remote_app_id;
          int* var129 = (void*)(var102 + 42);
          var129[0] = var125;
          memcpy(var102 + 26, nbp__my_host_id, 6);
          unsigned long long int var133 = var85->local_app_id;
          int* var137 = (void*)(var102 + 46);
          var137[0] = var133;
          unsigned long long int var138 = 8;
          unsigned int* var142 = (void*)(var102 + 32);
          var142[0] = var138;
          char* var149 = (void*)(var102);
          unsigned long long int var150 = 0;
          unsigned short int* var154 = (void*)(var149 + 40);
          var154[0] = var150;
          int var155 = 20;
          int* var159 = (void*)(var149 + 36);
          int var161 = var159[0] + 20;
          if ((var161 - var155) % 2) {
            var149[var161] = 0;
            var161 = var161 + 1;
          }
          unsigned short int var162 = 0;
          for (char* var163 = var149 + var155; var163 < (var149 + var161);
               var163 = var163 + 2) {
            unsigned short int* var164 = (void*)(var163);
            var162 = var162 + var164[0];
          }
          unsigned short int* var169 = (void*)(var102 + 40);
          var169[0] = var162;
          int* var178 = (void*)(var102 + 36);
          nbp__send_packet(var102 + 20, var178[0]);
          unsigned long long int* var186 = (void*)(arg0 + 12);
          nbp__connection_t* var188 = (void*)(var186[0]);
          unsigned int* var194 = (void*)(arg0 + 50);
          int* var200 = (void*)(arg0 + 36);
          nbp__insert_data_queue(var188->input_queue, arg0 + 58,
                                 var200[0] - 38);
        }
      } else {
        nbp__connection_t* var208 = 0;
        int var210 = nbp__net_state->num_conn;
        int var211 = 0;
        if (var211 < var210) {
          while (1) {
            if (((nbp__net_state->active_local_app_ids[var211] == var43) &&
                 (nbp__net_state->active_remote_app_ids[var211] == 0)) &&
                (memcmp(nbp__net_state->active_remote_host_ids[var211],
                        nbp__wildcard_host_identifier, 6) == 0)) {
              var208 = nbp__net_state->active_connections[var211];
              break;
            } else {
              var211 = var211 + 1;
              if (!(var211 < var210)) {
                break;
              }
            }
          }
        }
        var57 = var208;
        if (var57 != 0) {
          nbp__insert_accept_queue(var57->accept_queue, var49, var52, arg0);
        }
      }
    }
  }
}

void nbp__reliable_redelivery_timer_cb(nbp__timer* arg0, void* arg1,
                                       unsigned long long int arg2) {
  int* var7 = (void*)(arg1 + 36);
  nbp__send_packet(arg1 + 20, var7[0]);
  nbp__insert_timer(arg0, arg2 + 50000, nbp__reliable_redelivery_timer_cb,
                    arg1);
  // fprintf(stdout, "reliable redelivery timer cb\n");
}
