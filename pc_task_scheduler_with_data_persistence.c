#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_TASKS 32
#define MAX_NAME_LENGTH 50
#define SAVE_FILE "tasks.dat"
#define FILE_MAGIC 0x54534B53  // "TSKS" in hex as a magic number

typedef struct {
    char name[MAX_NAME_LENGTH];
    int priority;
    time_t created_at;
    time_t last_run;
    int runs_completed;
    bool is_active;
} Task;

typedef struct {
    uint32_t magic;           // Magic number for file validation
    uint32_t version;         // Version number for future compatibility
    Task tasks[MAX_TASKS];
    int task_count;
} Scheduler;

// Global scheduler instance
Scheduler scheduler;

// Function prototypes
void save_scheduler_state(void);
void load_scheduler_state(void);
void add_task(const char* name, int priority);
void list_tasks(void);
void run_tasks(void);
void init_scheduler(void);

// Initialize a new scheduler
void init_scheduler(void) {
    memset(&scheduler, 0, sizeof(Scheduler));
    scheduler.magic = FILE_MAGIC;
    scheduler.version = 1;
    scheduler.task_count = 0;
}

// Save scheduler state to file with error checking
void save_scheduler_state(void) {
    FILE* file = fopen(SAVE_FILE, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Write the entire scheduler struct
    size_t written = fwrite(&scheduler, sizeof(Scheduler), 1, file);

    if (written != 1) {
        perror("Error writing to file");
        fclose(file);
        return;
    }

    if (fclose(file) != 0) {
        perror("Error closing file");
        return;
    }

    printf("Scheduler state saved successfully to %s\n", SAVE_FILE);
}

// Load scheduler state from file with validation
void load_scheduler_state(void) {
    FILE* file = fopen(SAVE_FILE, "rb");
    if (file == NULL) {
        printf("No existing save file found. Starting with new scheduler state.\n");
        init_scheduler();
        return;
    }

    // Read the entire file content
    Scheduler temp_scheduler;
    size_t read = fread(&temp_scheduler, sizeof(Scheduler), 1, file);
    fclose(file);

    if (read != 1) {
        printf("Error reading save file. Starting with new scheduler state.\n");
        init_scheduler();
        return;
    }

    // Validate the magic number
    if (temp_scheduler.magic != FILE_MAGIC) {
        printf("Invalid save file format. Starting with new scheduler state.\n");
        init_scheduler();
        return;
    }

    // Copy the valid data to our scheduler
    memcpy(&scheduler, &temp_scheduler, sizeof(Scheduler));
    printf("Scheduler state loaded successfully from %s\n", SAVE_FILE);
}

// Add a new task to the scheduler
void add_task(const char* name, int priority) {
    if (scheduler.task_count >= MAX_TASKS) {
        printf("Error: Maximum task limit reached\n");
        return;
    }

    if (priority < 0 || priority > 9) {
        printf("Error: Priority must be between 0 and 9\n");
        return;
    }

    Task* new_task = &scheduler.tasks[scheduler.task_count];
    strncpy(new_task->name, name, MAX_NAME_LENGTH - 1);
    new_task->name[MAX_NAME_LENGTH - 1] = '\0';
    new_task->priority = priority;
    new_task->created_at = time(NULL);
    new_task->last_run = 0;
    new_task->runs_completed = 0;
    new_task->is_active = true;

    scheduler.task_count++;
    printf("Task '%s' added with priority %d\n", name, priority);
    save_scheduler_state();
}

// List all tasks
void list_tasks(void) {
    if (scheduler.task_count == 0) {
        printf("\nNo tasks found.\n");
        return;
    }

    printf("\nTask List:\n");
    printf("%-20s %-10s %-20s %-20s %-15s %-10s\n",
           "Name", "Priority", "Created", "Last Run", "Runs", "Status");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < scheduler.task_count; i++) {
        Task* task = &scheduler.tasks[i];
        char created[26];
        char last_run[26];

        strftime(created, sizeof(created), "%Y-%m-%d %H:%M:%S", localtime(&task->created_at));
        if (task->last_run == 0) {
            strcpy(last_run, "Never");
        } else {
            strftime(last_run, sizeof(last_run), "%Y-%m-%d %H:%M:%S", localtime(&task->last_run));
        }

        printf("%-20s %-10d %-20s %-20s %-15d %-10s\n",
               task->name,
               task->priority,
               created,
               last_run,
               task->runs_completed,
               task->is_active ? "Active" : "Inactive");
    }
    printf("\n");
}

// Simulate running tasks
void run_tasks(void) {
    if (scheduler.task_count == 0) {
        printf("No tasks to run.\n");
        return;
    }

    printf("Running tasks...\n");

    // Sort tasks by priority (bubble sort for simplicity)
    for (int i = 0; i < scheduler.task_count - 1; i++) {
        for (int j = 0; j < scheduler.task_count - i - 1; j++) {
            if (scheduler.tasks[j].priority > scheduler.tasks[j + 1].priority) {
                Task temp = scheduler.tasks[j];
                scheduler.tasks[j] = scheduler.tasks[j + 1];
                scheduler.tasks[j + 1] = temp;
            }
        }
    }

    // Run active tasks in priority order
    for (int i = 0; i < scheduler.task_count; i++) {
        Task* task = &scheduler.tasks[i];
        if (task->is_active) {
            printf("Executing task: %s (priority: %d)\n", task->name, task->priority);
            task->last_run = time(NULL);
            task->runs_completed++;
            sleep(1); // Simulate task execution
        }
    }

    save_scheduler_state();
}

int main() {
    load_scheduler_state();

    char choice;
    int priority;

    while (1) {
        char task_name[MAX_NAME_LENGTH];
        printf("\nTask Scheduler Menu:\n");
        printf("1. Add Task\n");
        printf("2. List Tasks\n");
        printf("3. Run Tasks\n");
        printf("4. Delete tasks.dat and Exit\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                printf("Enter task name: ");
                scanf(" %49[^\n]", task_name);
                printf("Enter priority (0-9, 0 is highest): ");
                scanf("%d", &priority);
                add_task(task_name, priority);
                break;

            case '2':
                list_tasks();
                break;

            case '3':
                run_tasks();
                break;

            case '4':
                printf("Deleting tasks.dat and exiting...\n");
                remove(SAVE_FILE);
                return 0;

            case '5':
                printf("Exiting...\n");
                save_scheduler_state();
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }
}
