// This is the top of the stack, as provided to us by the linker.
extern unsigned int _estack;


// This is a partial definition of the vector table. It only defines the
// first two entries which, as far as I can tell, are the minimum needed
// for a program to work at all.
// Space for the other interrupt handlers is reserved. I'm not sure if this
// is necessary, but I can imagine that the vector table not having the
// right length could cause all kinds of problems (imagine if it was too
// short, and the linker would place something else directly after it).
typedef struct {
	void *initial_stack_pointer_value;
	void *reset_handler;

	char other_interrupt_vectors[44 * 4]; // space for 44 32-bit pointers
} VectorTable;


void start();


// The vector table. We're using GCC-specific functionality to place this
// into the .vectors section, not where it would normally go (I suppose
// .rodata). The linker script makes sure that the .vectors section is at
// the right place.
__attribute__ ((section(".vectors")))
const VectorTable vector_table = {
	(void *)(&_estack),
	(void *)start,
};

// Addresses of several registers used to control the real-time timer.
static volatile int * const timer_mode_register  = (int *)0x400E1A30;
static volatile int * const timer_value_register = (int *)0x400E1A38;


// As the name suggests, this function sleeps for a given number of
// milliseconds. Our replacement for Arduino's delay function.
void sleep_ms(int milliseconds) {
	int sleep_until = *timer_value_register + milliseconds;
	while (*timer_value_register < sleep_until) {}
}

#include <sam3xa/include/sam3x8e.h>

// This function is the entry point for our application and the handler
// function for the reset interrupt.
void start() {
  REG_PMC_PCER0 |= PMC_PCER0_PID12;
  REG_PIOB_PER |= PIO_PB27;
  REG_PIOB_OER |= PIO_PB27;
  REG_PIOB_OWER |= PIO_PB27;

  // Set the timer to a resolution of a millisecond.
  *timer_mode_register = 0x00000020;

  for(;;){
    REG_PIOB_CODR |= PIO_PB27;
    sleep_ms(500);
    REG_PIOB_SODR |= PIO_PB27;
    sleep_ms(500);
  }
}
