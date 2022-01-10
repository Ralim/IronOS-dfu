
// Based on libopencm3 project.
#include <stdint.h>
void null_handler(void);
int main(void);
void __attribute__((naked)) reset_handler(void);

extern unsigned _data_loadaddr, _data, _edata, _ebss, _stack;

typedef void (*vector_table_entry_t)(void);

typedef struct {
  unsigned int *initial_sp_value; /**< Initial stack pointer value. */
  vector_table_entry_t reset;
  vector_table_entry_t nmi;
  vector_table_entry_t hard_fault;
  vector_table_entry_t memory_manage_fault; /* not in CM0 */
  vector_table_entry_t bus_fault;           /* not in CM0 */
  vector_table_entry_t usage_fault;         /* not in CM0 */
  vector_table_entry_t reserved_x001c[4];
  vector_table_entry_t sv_call;
  vector_table_entry_t debug_monitor; /* not in CM0 */
  vector_table_entry_t reserved_x0034;
  vector_table_entry_t pend_sv;
  vector_table_entry_t systick;
} vector_table_t;

// A handler that does nothing, we use no interrupts
void null_handler(void) {
  while (1)
    ;
}

/* Less common symbols exported by the linker script(s): */
typedef void (*funcp_t)(void);

void __attribute__((naked)) nmi_handler(void);
void __attribute__((naked)) hard_fault_handler(void);
void __attribute__((naked)) memory_manage_fault_handler(void);
void __attribute__((naked)) bus_fault_handler(void);
void __attribute__((naked)) usage_fault_handler(void);
void __attribute__((naked)) debug_monitor_handler(void);
void __attribute__((naked)) sv_call_handler(void);
void __attribute__((naked)) pend_sv_handler(void);
void __attribute__((naked)) systick_handler(void);

void __attribute__((naked)) reset_handler(void) {
  volatile unsigned *src, *dest;

  for (src = &_data_loadaddr, dest = &_data; dest < &_edata; src++, dest++) {
    *dest = *src;
  }

  while (dest < &_ebss)
    *dest++ = 0;

  /* Ensure 8-byte alignment of stack pointer on interrupts */
  /* Enabled by default on most Cortex-M parts, but not M3 r1 */
  volatile uint32_t *_scb_ccr = (uint32_t *)0xE000ED14U;
  *_scb_ccr |= (1 << 9);

  /* Call the application's entry point. */
  main();
}

// Vector table (bare minimal one)
__attribute__((section(".vectors"))) vector_table_t vector_table = {
    .initial_sp_value = &_stack,
    .reset = reset_handler,
    .nmi = nmi_handler,
    .hard_fault = hard_fault_handler,
    .memory_manage_fault = memory_manage_fault_handler,
    .bus_fault = bus_fault_handler,
    .usage_fault = usage_fault_handler,
    .debug_monitor = debug_monitor_handler,
    .sv_call = sv_call_handler,
    .pend_sv = pend_sv_handler,
    .systick = systick_handler,
};

void __attribute__((naked)) nmi_handler(void) { asm("bkpt"); }
void __attribute__((naked)) hard_fault_handler(void) { asm("bkpt"); }
void __attribute__((naked)) memory_manage_fault_handler(void) { asm("bkpt"); }
void __attribute__((naked)) bus_fault_handler(void) { asm("bkpt"); }
void __attribute__((naked)) usage_fault_handler(void) { asm("bkpt"); }
void __attribute__((naked)) debug_monitor_handler(void) { asm("bkpt"); }
void __attribute__((naked)) sv_call_handler(void) { asm("bkpt"); }
void __attribute__((naked)) pend_sv_handler(void) { asm("bkpt"); }
void __attribute__((naked)) systick_handler(void) { asm("bkpt"); }