/********************************
 * this is specifically embedded ARM Cortex-M code that requires the appropriate SDK and
 * won't run on a regular PC
********************************/

#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 32
#define TICK_RATE_HZ 1000
#define STACK_SIZE 1024

typedef void (*task_function_t)(void*);

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
} task_state_t;

typedef struct {
    uint32_t* stack_ptr;            // Current stack pointer
    uint32_t stack[STACK_SIZE];     // Task stack
    task_function_t function;       // Task function pointer
    void* args;                     // Task arguments
    uint8_t priority;               // 0 is highest priority
    uint32_t time_slice;            // Time slice in ticks
    uint32_t ticks_remaining;       // Ticks until preemption
    task_state_t state;             // Current task state
    const char* name;               // Task name for debugging
} task_control_block_t;

// Scheduler context
typedef struct {
    task_control_block_t tasks[MAX_TASKS];
    uint8_t task_count;
    uint8_t current_task;
    uint32_t system_ticks;
    bool scheduler_running;
} scheduler_context_t;

static scheduler_context_t scheduler;

// Architecture-specific context switch (example for ARM Cortex-M)
__attribute__((naked)) void PendSV_Handler(void) {
    __asm volatile (
        "PUSH    {R4-R11}           \n" // Save remaining registers
        "LDR     R0, =scheduler     \n" // Load scheduler address
        "LDR     R1, [R0, #4]       \n" // Load current task index
        "LDR     R2, =tasks         \n" // Load tasks array
        "LSL     R1, R1, #2         \n" // Multiply by 4 for pointer arithmetic
        "ADD     R2, R2, R1         \n" // Get current TCB
        "STR     SP, [R2]           \n" // Save current SP to TCB

        // Load new task context
        "LDR     R1, [R0, #8]       \n" // Load new task index
        "LSL     R1, R1, #2         \n"
        "ADD     R2, R2, R1         \n"
        "LDR     SP, [R2]           \n" // Load new SP from TCB
        "POP     {R4-R11}           \n"
        "BX      LR                 \n"
    );
}

// Initialize the scheduler
void scheduler_init(void) {
    scheduler.task_count = 0;
    scheduler.current_task = 0;
    scheduler.system_ticks = 0;
    scheduler.scheduler_running = false;
}

// Create a new task
bool create_task(task_function_t function, void* args, uint8_t priority,
                uint32_t time_slice, const char* name) {
    if (scheduler.task_count >= MAX_TASKS) {
        return false;
    }

    task_control_block_t* tcb = &scheduler.tasks[scheduler.task_count];

    // Initialize stack (assuming stack grows downward)
    tcb->stack_ptr = &tcb->stack[STACK_SIZE - 16]; // Space for context

    // Set up initial stack frame for context switching
    tcb->stack[STACK_SIZE - 1] = 0x01000000;       // xPSR
    tcb->stack[STACK_SIZE - 2] = (uint32_t)function; // PC
    tcb->stack[STACK_SIZE - 3] = 0xFFFFFFFD;       // LR
    tcb->stack[STACK_SIZE - 4] = 0;                // R12
    tcb->stack[STACK_SIZE - 5] = 0;                // R3
    tcb->stack[STACK_SIZE - 6] = 0;                // R2
    tcb->stack[STACK_SIZE - 7] = 0;                // R1
    tcb->stack[STACK_SIZE - 8] = (uint32_t)args;   // R0

    // Initialize TCB
    tcb->function = function;
    tcb->args = args;
    tcb->priority = priority;
    tcb->time_slice = time_slice;
    tcb->ticks_remaining = time_slice;
    tcb->state = TASK_READY;
    tcb->name = name;

    scheduler.task_count++;
    return true;
}

// Find the highest priority ready task
static uint8_t find_next_task(void) {
    uint8_t highest_priority = 255;
    uint8_t next_task = scheduler.current_task;

    for (uint8_t i = 0; i < scheduler.task_count; i++) {
        if (scheduler.tasks[i].state == TASK_READY &&
            scheduler.tasks[i].priority < highest_priority) {
            highest_priority = scheduler.tasks[i].priority;
            next_task = i;
        }
    }

    return next_task;
}

// Timer tick handler - called at TICK_RATE_HZ
void scheduler_tick(void) {
    scheduler.system_ticks++;

    if (!scheduler.scheduler_running) {
        return;
    }

    task_control_block_t* current = &scheduler.tasks[scheduler.current_task];
    current->ticks_remaining--;

    if (current->ticks_remaining == 0) {
        current->ticks_remaining = current->time_slice;
        current->state = TASK_READY;

        // Trigger context switch
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}

// Start the scheduler
void start_scheduler(void) {
    if (scheduler.task_count == 0) {
        return;
    }

    // Set up SysTick for scheduler tick
    SysTick_Config(SystemCoreClock / TICK_RATE_HZ);

    // Set up PendSV for context switching
    NVIC_SetPriority(PendSV_IRQn, 0xFF); // Lowest priority

    scheduler.scheduler_running = true;
    scheduler.current_task = find_next_task();

    // Start first task
    task_control_block_t* first_task = &scheduler.tasks[scheduler.current_task];
    __set_PSP((uint32_t)first_task->stack_ptr);
    __set_CONTROL(0x02); // Switch to PSP
    __ISB();

    first_task->function(first_task->args);
}

