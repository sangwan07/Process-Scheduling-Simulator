// Header guard, taaki is file ko galti se ek se zyada baar include na kiya jaye.
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h> // INT_MAX ke liye

// --- Constants ---
#define MAX_PROCESSES 100 // Simulator maximum kitne process handle kar sakta hai.

// --- Data Structures ---

// Yeh structure ek process ki saari zaroori jaankari store karta hai.
typedef struct {
    int pid;              // Process ID
    int arrival_time;     // Process kab system mein aaya
    int burst_time;       // Process ko chalne ke liye total kitna time chahiye
    int priority;         // Process ki priority (chhota number matlab high priority)
    
    // --- Simulation ke liye zaroori variables ---
    int remaining_time;   // Process ka kitna kaam bacha hai (preemptive ke liye)
    int completion_time;  // Process kab poora hua
    int waiting_time;     // Process ne ready queue mein kitna intezaar kiya
    int turnaround_time;  // Process ke aane se लेकर poora hone tak ka time
    
    // --- Memory Simulation Feature ---
    void* memory_block;   // Simulated memory block ka pointer
    bool is_completed;    // Flag yeh batane ke liye ki process poora ho gaya hai
} Process;


// Yeh structure Gantt chart ke ek block ko represent karta hai.
typedef struct {
    int pid;              // Kaun sa process chala
    int start_time;       // Kab chalna shuru hua
    int end_time;         // Kab chalna band hua
} GanttEntry;


// --- Function Prototypes (Function declarations) ---

// Menu aur user interaction ke functions
void display_menu();
void add_process();
void handle_user_choice();

// Memory simulation ke functions
void simulate_memory_allocation(Process* p);
void simulate_memory_free(Process* p);

// Scheduling algorithms ke functions
void run_fcfs();
void run_sjf_preemptive();
void run_priority_preemptive();
void run_round_robin();

// Results aur Gantt chart dikhane wale functions
void calculate_metrics(Process procs[], int n);
void print_gantt_chart(GanttEntry chart[], int n);
void print_results_table(Process procs[], int n, const char* algorithm_name);

// Sabhi algorithms ko compare karne wala function
void compare_all_algorithms();

// Helper functions
void reset_process_state();
void copy_processes(Process dest[], Process src[], int n);

#endif // SIMULATOR_H

/*   gcc scheduling_simulator.c -o simulator

./simulator    */