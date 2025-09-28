/**
 * @file simulator.h
 * @brief Header file for the Advanced Process Scheduling Simulator.
 *
 * This file contains the definitions for data structures (Process, GanttEntry)
 * and the function prototypes used across the simulator. This structure
 * promotes modularity and clarity, which are key in system software development.
 * Relevant for companies like American Megatrends (AMI).
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h> // For INT_MAX

// --- Constants ---
#define MAX_PROCESSES 100 // Maximum number of processes the simulator can handle

// --- Data Structures ---

/**
 * @struct Process
 * @brief Represents a single process in the operating system simulation.
 *
 * This structure holds all essential attributes of a process needed for scheduling.
 */
typedef struct {
    int pid;                 // Process ID
    int arrival_time;        // Time at which the process arrives in the ready queue
    int burst_time;          // Total CPU time required by the process
    int priority;            // Priority of the process (lower number means higher priority)
    
    // --- Simulation-specific fields ---
    int remaining_time;      // Time remaining for execution (used in preemptive algorithms)
    int completion_time;     // Time at which the process finishes execution
    int waiting_time;        // Time spent waiting in the ready queue (Turnaround Time - Burst Time)
    int turnaround_time;     // Time from arrival to completion (Completion Time - Arrival Time)
    
    // --- Advanced Feature: Memory Simulation ---
    void* memory_block;      // Pointer to a simulated allocated memory block on the heap
    bool is_completed;       // Flag to check if the process has completed
} Process;

/**
 * @struct GanttEntry
 * @brief Represents a single block in the Gantt chart visualization.
 */
typedef struct {
    int pid;                 // Process ID that was running
    int start_time;          // The time this process started its execution slice
    int end_time;            // The time this process ended its execution slice
} GanttEntry;


// --- Function Prototypes ---

// Main menu and user interaction
void display_menu();
void add_process();
void handle_user_choice();

// Memory simulation
void simulate_memory_allocation(Process* p);
void simulate_memory_free(Process* p);

// Scheduling algorithms
void run_fcfs();
void run_sjf_preemptive();
void run_priority_preemptive();
void run_round_robin();

// Results and visualization
void calculate_metrics(Process procs[], int n);
void print_gantt_chart(GanttEntry chart[], int n);
void print_results_table(Process procs[], int n, const char* algorithm_name);

// Comparative analysis
void compare_all_algorithms();

// Utility functions
void reset_process_state();
void copy_processes(Process dest[], Process src[], int n);

#endif // SIMULATOR_H

/*   gcc scheduling_simulator.c -o simulator

./simulator    */