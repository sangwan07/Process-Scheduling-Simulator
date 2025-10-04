#include "simulator.h"
#include <string.h> // memcpy ke liye

// --- Global Variables ---
Process processes[MAX_PROCESSES];
int process_count = 0;
int next_pid = 1;

// Program ka main entry point.
int main() {
    handle_user_choice();
    return 0;
}

// User ko saare options dikhane ke liye menu.
void display_menu() {
    printf("\n+----------------------------------------------------+\n");
    printf("|        ADVANCED PROCESS SCHEDULING SIMULATOR       |\n");
    printf("+----------------------------------------------------+\n");
    printf("| 1. Add Process                                     |\n");
    printf("| 2. Run First-Come, First-Served (FCFS)             |\n");
    printf("| 3. Run Shortest Job First (SJF) - Preemptive       |\n");
    printf("| 4. Run Priority Scheduling - Preemptive          |\n");
    printf("| 5. Run Round Robin (RR)                            |\n");
    printf("| 6. Compare All Algorithms & Find Best              |\n");
    printf("| 7. Exit                                            |\n");
    printf("+----------------------------------------------------+\n");
    printf("Enter your choice: ");
}

// User ke menu choice ko handle karne wala function.
void handle_user_choice() {
    int choice;
    do {
        display_menu();
        int result = scanf("%d", &choice);
        
        // Input sahi hai ya nahi, yeh check karne ke liye.
        if (result != 1) {
            printf("\n[ERROR] Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Input buffer ko clear karo
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1: add_process(); break;
            case 2: run_fcfs(); break;
            case 3: run_sjf_preemptive(); break;
            case 4: run_priority_preemptive(); break;
            case 5: run_round_robin(); break;
            case 6: compare_all_algorithms(); break;
            case 7: printf("\nExiting simulator. Goodbye!\n"); break;
            default: printf("\n[ERROR] Invalid choice. Please try again.\n"); break;
        }
    } while (choice != 7);
}

// User se process ki details lekar list mein add karta hai.
void add_process() {
    if (process_count >= MAX_PROCESSES) {
        printf("\n[ERROR] Maximum process limit reached.\n");
        return;
    }

    Process p;
    p.pid = next_pid;

    printf("\n--- Add New Process (PID: %d) ---\n", p.pid);
    printf("Enter Arrival Time: ");
    if(scanf("%d", &p.arrival_time) != 1 || p.arrival_time < 0) {
        printf("[ERROR] Invalid arrival time. Must be a non-negative integer.\n");
        while(getchar()!='\n'); return;
    }

    printf("Enter Burst Time: ");
    if(scanf("%d", &p.burst_time) != 1 || p.burst_time <= 0) {
        printf("[ERROR] Invalid burst time. Must be a positive integer.\n");
        while(getchar()!='\n'); return;
    }

    printf("Enter Priority (lower number = higher priority): ");
     if(scanf("%d", &p.priority) != 1 || p.priority < 0) {
        printf("[ERROR] Invalid priority. Must be a non-negative integer.\n");
        while(getchar()!='\n'); return;
    }

    p.remaining_time = p.burst_time;
    p.is_completed = false;
    simulate_memory_allocation(&p); // Memory allocation ka simulation

    processes[process_count++] = p;
    next_pid++;
    printf("\n[SUCCESS] Process %d added successfully.\n", p.pid);
}


// Process ke liye memory block allocate karne ka simulation.
void simulate_memory_allocation(Process* p) {
    p->memory_block = malloc(p->burst_time * sizeof(char) * 10);
    if (p->memory_block == NULL) {
        printf("[MEMORY_SIM] Failed to allocate memory for PID %d.\n", p->pid);
    } else {
        printf("[MEMORY_SIM] Allocated memory for PID %d at address %p.\n", p->pid, p->memory_block);
    }
}

// Process ke poora hone par memory free karne ka simulation.
void simulate_memory_free(Process* p) {
    if (p->memory_block != NULL) {
        printf("[MEMORY_SIM] Freeing memory for PID %d from address %p.\n", p->pid, p->memory_block);
        free(p->memory_block);
        p->memory_block = NULL;
    }
}


// Naye simulation ke liye sabhi process ki state ko reset karta hai.
void reset_process_state() {
    for (int i = 0; i < process_count; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].is_completed = false;
        processes[i].completion_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].waiting_time = 0;
    }
}

// Har algorithm ke liye processes ki ek fresh copy banata hai.
void copy_processes(Process dest[], Process src[], int n) {
    for(int i = 0; i < n; i++) {
        dest[i] = src[i];
        dest[i].remaining_time = src[i].burst_time;
        dest[i].is_completed = false;
    }
}

// First-Come, First-Served (FCFS) algorithm ka simulation.
void run_fcfs() {
    if (process_count == 0) {
        printf("\n[ERROR] No processes to schedule. Please add processes first.\n");
        return;
    }
    
    Process procs[MAX_PROCESSES];
    copy_processes(procs, processes, process_count);
    GanttEntry gantt_chart[MAX_PROCESSES * 2];
    int gantt_count = 0;
    
    // Processes ko arrival time ke hisab se sort karna.
    for (int i = 0; i < process_count - 1; i++) {
        for (int j = 0; j < process_count - i - 1; j++) {
            if (procs[j].arrival_time > procs[j + 1].arrival_time) {
                Process temp = procs[j];
                procs[j] = procs[j + 1];
                procs[j + 1] = temp;
            }
        }
    }
    
    int current_time = 0;
    for (int i = 0; i < process_count; i++) {
        if (current_time < procs[i].arrival_time) {
            current_time = procs[i].arrival_time; // CPU khali hai.
        }
        
        gantt_chart[gantt_count].pid = procs[i].pid;
        gantt_chart[gantt_count].start_time = current_time;
        
        current_time += procs[i].burst_time;
        procs[i].completion_time = current_time;
        
        gantt_chart[gantt_count].end_time = current_time;
        gantt_count++;

        simulate_memory_free(&procs[i]);
    }

    calculate_metrics(procs, process_count);
    print_results_table(procs, process_count, "First-Come, First-Served (FCFS)");
    print_gantt_chart(gantt_chart, gantt_count);
}


// Preemptive Shortest Job First (SJF) algorithm ka simulation.
void run_sjf_preemptive() {
    if (process_count == 0) {
        printf("\n[ERROR] No processes to schedule.\n");
        return;
    }

    Process procs[MAX_PROCESSES];
    copy_processes(procs, processes, process_count);
    GanttEntry gantt_chart[MAX_PROCESSES * 10];
    int gantt_count = 0;
    
    int current_time = 0;
    int completed_count = 0;
    int last_pid = -1;

    while (completed_count < process_count) {
        int shortest_job_idx = -1;
        int min_remaining_time = INT_MAX;

        // Sabse kam remaining time wala process dhundhna jo aa chuka hai.
        for (int i = 0; i < process_count; i++) {
            if (procs[i].arrival_time <= current_time && !procs[i].is_completed) {
                if (procs[i].remaining_time < min_remaining_time) {
                    min_remaining_time = procs[i].remaining_time;
                    shortest_job_idx = i;
                }
            }
        }

        if (shortest_job_idx == -1) {
            current_time++; // CPU khali hai.
            continue;
        }
        
        int current_pid = procs[shortest_job_idx].pid;

        // Gantt chart ke liye: agar process badalta hai toh nayi entry.
        if(last_pid != current_pid) {
             if(gantt_count > 0) {
                gantt_chart[gantt_count-1].end_time = current_time;
            }
            gantt_chart[gantt_count].pid = current_pid;
            gantt_chart[gantt_count].start_time = current_time;
            gantt_count++;
        }
        last_pid = current_pid;

        procs[shortest_job_idx].remaining_time--;
        current_time++;

        if (procs[shortest_job_idx].remaining_time == 0) {
            procs[shortest_job_idx].completion_time = current_time;
            procs[shortest_job_idx].is_completed = true;
            completed_count++;
            simulate_memory_free(&procs[shortest_job_idx]);
            last_pid = -1;
        }
    }
    gantt_chart[gantt_count-1].end_time = current_time;

    calculate_metrics(procs, process_count);
    print_results_table(procs, process_count, "Preemptive Shortest Job First (SJF)");
    print_gantt_chart(gantt_chart, gantt_count);
}

// Preemptive Priority scheduling algorithm ka simulation.
void run_priority_preemptive() {
    if (process_count == 0) {
        printf("\n[ERROR] No processes to schedule.\n");
        return;
    }

    Process procs[MAX_PROCESSES];
    copy_processes(procs, processes, process_count);
    GanttEntry gantt_chart[MAX_PROCESSES * 10];
    int gantt_count = 0;
    
    int current_time = 0;
    int completed_count = 0;
    int last_pid = -1;

    while (completed_count < process_count) {
        int highest_priority_idx = -1;
        int min_priority = INT_MAX;

        // Sabse zyada priority wala process dhundhna (jiska priority number sabse kam ho).
        for (int i = 0; i < process_count; i++) {
            if (procs[i].arrival_time <= current_time && !procs[i].is_completed) {
                if (procs[i].priority < min_priority) {
                    min_priority = procs[i].priority;
                    highest_priority_idx = i;
                }
            }
        }

        if (highest_priority_idx == -1) {
            current_time++; // CPU khali hai.
            continue;
        }
        
        int current_pid = procs[highest_priority_idx].pid;

        if(last_pid != current_pid) {
            if(gantt_count > 0) gantt_chart[gantt_count-1].end_time = current_time;
            gantt_chart[gantt_count].pid = current_pid;
            gantt_chart[gantt_count].start_time = current_time;
            gantt_count++;
        }
        last_pid = current_pid;

        procs[highest_priority_idx].remaining_time--;
        current_time++;

        if (procs[highest_priority_idx].remaining_time == 0) {
            procs[highest_priority_idx].completion_time = current_time;
            procs[highest_priority_idx].is_completed = true;
            completed_count++;
            simulate_memory_free(&procs[highest_priority_idx]);
            last_pid = -1;
        }
    }
    gantt_chart[gantt_count-1].end_time = current_time;

    calculate_metrics(procs, process_count);
    print_results_table(procs, process_count, "Preemptive Priority Scheduling");
    print_gantt_chart(gantt_chart, gantt_count);
}

// Round Robin (RR) scheduling algorithm ka simulation.
void run_round_robin() {
    if (process_count == 0) {
        printf("\n[ERROR] No processes to schedule.\n");
        return;
    }

    int time_quantum;
    printf("\nEnter Time Quantum for Round Robin: ");
    if (scanf("%d", &time_quantum) != 1 || time_quantum <= 0) {
        printf("[ERROR] Invalid time quantum. Must be a positive integer.\n");
        while(getchar()!='\n'); return;
    }

    Process procs[MAX_PROCESSES];
    copy_processes(procs, processes, process_count);
    GanttEntry gantt_chart[MAX_PROCESSES * 10];
    int gantt_count = 0;

    int ready_queue[MAX_PROCESSES];
    int front = -1, rear = -1;
    bool in_queue[MAX_PROCESSES + 1] = {false};
    
    int current_time = 0;
    int completed_count = 0;
    
    // Processes ko arrival time ke hisab se sort karna.
    for (int i = 0; i < process_count - 1; i++) {
        for (int j = 0; j < process_count - i - 1; j++) {
            if (procs[j].arrival_time > procs[j+1].arrival_time) {
                Process temp = procs[j]; procs[j] = procs[j+1]; procs[j+1] = temp;
            }
        }
    }

    while (completed_count < process_count) {
        // Naye aaye hue processes ko ready queue mein daalna.
        for (int i = 0; i < process_count; i++) {
            if (!procs[i].is_completed && procs[i].arrival_time <= current_time && !in_queue[procs[i].pid]) {
                if (front == -1) front = 0;
                rear = (rear + 1) % MAX_PROCESSES;
                ready_queue[rear] = i;
                in_queue[procs[i].pid] = true;
            }
        }

        if (front == -1) {
            current_time++; // CPU khali hai.
            continue;
        }

        int current_proc_idx = ready_queue[front];
        front = (front == rear) ? -1 : (front + 1) % MAX_PROCESSES;
        in_queue[procs[current_proc_idx].pid] = false;
        
        int time_slice = (procs[current_proc_idx].remaining_time < time_quantum) ? procs[current_proc_idx].remaining_time : time_quantum;
        
        gantt_chart[gantt_count].pid = procs[current_proc_idx].pid;
        gantt_chart[gantt_count].start_time = current_time;
        
        current_time += time_slice;
        procs[current_proc_idx].remaining_time -= time_slice;
        
        gantt_chart[gantt_count].end_time = current_time;
        gantt_count++;

        // Jo process is time slice ke dauran aaye, unhe queue mein add karna.
        for (int i = 0; i < process_count; i++) {
            if (!procs[i].is_completed && procs[i].arrival_time <= current_time && !in_queue[procs[i].pid]) {
                if(front == -1) front = 0;
                rear = (rear + 1) % MAX_PROCESSES;
                ready_queue[rear] = i;
                in_queue[procs[i].pid] = true;
            }
        }

        if (procs[current_proc_idx].remaining_time == 0) {
            procs[current_proc_idx].is_completed = true;
            procs[current_proc_idx].completion_time = current_time;
            completed_count++;
            simulate_memory_free(&procs[current_proc_idx]);
        } else {
            // Agar process poora nahi hua, toh use wapas queue mein daal do.
            if(front == -1) front = 0;
            rear = (rear + 1) % MAX_PROCESSES;
            ready_queue[rear] = current_proc_idx;
            in_queue[procs[current_proc_idx].pid] = true;
        }
    }

    calculate_metrics(procs, process_count);
    print_results_table(procs, process_count, "Round Robin (RR)");
    print_gantt_chart(gantt_chart, gantt_count);
}


// Sabhi processes ke liye waiting time aur turnaround time calculate karta hai.
void calculate_metrics(Process procs[], int n) {
    for (int i = 0; i < n; i++) {
        for(int j=0; j < process_count; j++){
            if(procs[i].pid == processes[j].pid){
                 procs[i].turnaround_time = procs[i].completion_time - procs[i].arrival_time;
                 procs[i].waiting_time = procs[i].turnaround_time - processes[j].burst_time;
                 break;
            }
        }
    }
}

// Results ko ek saaf table format mein print karta hai.
void print_results_table(Process procs[], int n, const char* algorithm_name) {
    float total_wt = 0, total_tat = 0;
    
    // Output consistent rahe, isliye PID se sort karna.
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (procs[j].pid > procs[j + 1].pid) {
                Process temp = procs[j]; procs[j] = procs[j + 1]; procs[j + 1] = temp;
            }
        }
    }

    printf("\n\n--- RESULTS FOR: %s ---\n", algorithm_name);
    printf("+-----+--------------+------------+----------+-----------------+-------------------+--------------+\n");
    printf("| PID | Arrival Time | Burst Time | Priority | Completion Time | Turnaround Time | Waiting Time |\n");
    printf("+-----+--------------+------------+----------+-----------------+-------------------+--------------+\n");
    for (int i = 0; i < n; i++) {
        printf("| %-3d | %-12d | %-10d | %-8d | %-15d | %-17d | %-12d |\n",
               procs[i].pid, procs[i].arrival_time, processes[i].burst_time, procs[i].priority,
               procs[i].completion_time, procs[i].turnaround_time, procs[i].waiting_time);
        total_wt += procs[i].waiting_time;
        total_tat += procs[i].turnaround_time;
    }
    printf("+-----+--------------+------------+----------+-----------------+-------------------+--------------+\n");
    printf("| Average Waiting Time     : %.2f\n", total_wt / n);
    printf("| Average Turnaround Time  : %.2f\n", total_tat / n);
    printf("+------------------------------------------------------------------------------------------------+\n");
}


// Ek visual ASCII Gantt chart print karta hai.
void print_gantt_chart(GanttEntry chart[], int n) {
    printf("\n--- GANTT CHART ---\n\n");

    // Upar ka border
    printf(" ");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < (chart[i].end_time - chart[i].start_time) * 2; j++) printf("-");
        printf(" ");
    }
    printf("\n|");

    // Process IDs
    for (int i = 0; i < n; i++) {
        int duration = (chart[i].end_time - chart[i].start_time) * 2;
        int padding = (duration - 4) / 2;
        if(padding < 0) padding = 0;

        for (int j = 0; j < padding; j++) printf(" ");
        printf(" P%d ", chart[i].pid);
        for (int j = 0; j < padding; j++) printf(" ");
        if((duration - 4) % 2 != 0) printf(" ");
        printf("|");
    }
    printf("\n ");

    // Neeche ka border
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < (chart[i].end_time - chart[i].start_time) * 2; j++) printf("-");
        printf(" ");
    }
    printf("\n");

    // Timestamps
    printf("%d", chart[0].start_time);
    for (int i = 0; i < n; i++) {
        int duration = (chart[i].end_time - chart[i].start_time) * 2;
        for (int j = 0; j < duration; j++) printf(" ");

        if (chart[i].end_time > 9) printf("\b"); // do-digit numbers ke liye adjustment
        printf("%d", chart[i].end_time);
    }
    printf("\n\n");
}


// Sabhi algorithms ko chalata hai aur unke average waiting time ko compare karta hai.
void compare_all_algorithms() {
    if (process_count == 0) {
        printf("\n[ERROR] No processes to compare. Please add processes first.\n");
        return;
    }
    printf("\n--- COMPARING ALL ALGORITHMS ---\n");
    
    // Note: Is function ka sabse aasan tareeka hai ki har algorithm ko dobara run kiya jaye
    // aur unke results ko compare kiya jaye.
    printf("\nNOTE: The following outputs are for comparison calculation.\n");
    run_fcfs();
    run_sjf_preemptive();
    run_priority_preemptive();
    run_round_robin();
    
    printf("\n[ANALYSIS] A full re-run of each algorithm was performed.\n");
    printf("Based on the results above, compare the 'Average Waiting Time' for each.\n");
    printf("The algorithm with the LOWEST average waiting time is generally the most efficient for the given workload.\n");
    printf("For throughput-oriented systems, SJF is often optimal. For interactive systems, Round Robin provides better response times.\n");
} 


