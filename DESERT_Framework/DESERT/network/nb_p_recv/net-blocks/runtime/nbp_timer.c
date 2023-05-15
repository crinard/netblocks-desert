#include "nbp_timer.h"

#include <stdio.h>
#include <stdlib.h>

nbp__timer nbp__allocated_timers[MAX_TIMER_ALLOCS];
nbp__timer* nbp__timer_free_list = NULL;

// Actual slots to hold the timers
nbp__timer* nbp__timer_slots[MAX_TIMER_SLOTS];

nbp__timer* nbp__alloc_timer(void) {
  if (nbp__timer_free_list == NULL) return NULL;
  nbp__timer* to_ret = nbp__timer_free_list;
  nbp__timer_free_list = to_ret->next;
  return to_ret;
}
void nbp__return_timer(nbp__timer* t) {
  t->next = nbp__timer_free_list;
  nbp__timer_free_list = t;
}
void nbp__insert_timer(nbp__timer* t, unsigned long long to,
                       nbp__timer_callback_t cb, void* argument) {
  t->callback = cb;
  t->argument = argument;
  t->timeout = to;

  // TODO: Ensure that we do not insert timers too ahead
  int slot = to % MAX_TIMER_SLOTS;
  t->next = nbp__timer_slots[slot];
  t->prev = NULL;
  if (t->next) t->next->prev = t;
  nbp__timer_slots[slot] = t;
}
void nbp__remove_timer(nbp__timer* t) {
  if (t->next) {
    t->next->prev = t->prev;
  }
  if (t->prev) {
    t->prev->next = t->next;
  } else {
    int slot = t->timeout % MAX_TIMER_SLOTS;
    nbp__timer_slots[slot] = t->next;
  }
  t->next = t->prev = NULL;
  t->timeout = -1;
}

static unsigned long long nbp__last_timer_checked;
extern unsigned long long nbp__get_time_ms_now(void);
void nbp__init_timers(void) {
  nbp__timer_free_list = &nbp__allocated_timers[0];
  for (int i = 0; i < MAX_TIMER_ALLOCS - 1; i++) {
    nbp__allocated_timers[i].next = &nbp__allocated_timers[i + 1];
  }
  nbp__allocated_timers[MAX_TIMER_ALLOCS - 1].next = NULL;
  for (int i = 0; i < MAX_TIMER_SLOTS; i++) {
    nbp__timer_slots[i] = NULL;
  }
  nbp__last_timer_checked = nbp__get_time_ms_now();
}

void nbp__check_timers(void) {
  unsigned long long t_now = nbp__get_time_ms_now();
  nbp__last_timer_checked = t_now;
  // Check all timers till now
  // This is because sometimes timers might not be checked if the stack is stuck
  for (unsigned long long i = nbp__last_timer_checked + 1; i <= t_now; i++) {
    int slot = i % MAX_TIMER_SLOTS;
    while (nbp__timer_slots[slot]) {
      nbp__timer* t = nbp__timer_slots[slot];
      // This is so that we keep the current slot consistent
      // It is possible that insertions and deletions might happen in callbacks
      if (t->next) t->next->prev = NULL;
      nbp__timer_slots[slot] = t->next;
      // It is the callback's job to free up this object
      // callbacks are allowed to reinsert timers if they wish
      t->callback(t, t->argument, i);
    }
  }
  nbp__last_timer_checked = t_now;
}
