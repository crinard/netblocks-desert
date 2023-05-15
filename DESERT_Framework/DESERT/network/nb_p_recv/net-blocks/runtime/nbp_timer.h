#ifndef NB_TIMER_IMPL_H
#define NB_TIMER_IMPL_H


// Can define timers upto a second away
#define MAX_TIMER_SLOTS (1000)
#define MAX_TIMER_ALLOCS (1024)

struct nbp__timer_obj;

typedef void (*nbp__timer_callback_t)(struct nbp__timer_obj*, void*, unsigned long long);

struct nbp__timer_obj {
	nbp__timer_callback_t callback;
	void* argument;
	
	// For chaining
	struct nbp__timer_obj* next;
	struct nbp__timer_obj* prev;
	unsigned long long timeout;
};

typedef struct nbp__timer_obj nbp__timer;

extern nbp__timer* nbp__timer_slots[MAX_TIMER_SLOTS];
extern nbp__timer nbp__allocated_timers[MAX_TIMER_ALLOCS];
extern nbp__timer* nbp__timer_free_list;

extern nbp__timer* nbp__alloc_timer(void);
extern void nbp__return_timer(nbp__timer*);
void nbp__insert_timer(nbp__timer* t, unsigned long long to, nbp__timer_callback_t cb, void* argument);
extern void nbp__remove_timer(nbp__timer*);
extern void nbp__init_timers(void);

extern void nbp__check_timers(void);

#endif
