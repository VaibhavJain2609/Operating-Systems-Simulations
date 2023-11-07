#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int completed;
    int waitingTime;

    Process(int pid, int arrival_time, int burst_time, int priority)
            : pid(pid), arrival_time(arrival_time), burst_time(burst_time), priority(priority), completed(burst_time), waitingTime(0) {}

    void reset() {
        waitingTime = 0;
        completed = burst_time;
    }

    int work(int time_quanta, int time) {
        int retTime = 0;
        if (time_quanta > 0) {
            if (time_quanta <= completed) {
                completed -= time_quanta;
                retTime = time_quanta;
            } else {
                retTime = completed;
                completed = 0;
            }
            return retTime;
        } else {
            retTime = completed;
            completed = 0;
        }
        return retTime;
    }
};

struct CompareProcesses {
    int Scheduling_order;

    CompareProcesses(int scheduling_order) : Scheduling_order(scheduling_order) {}

    bool operator()(const Process& p1, const Process& p2) {
        switch (Scheduling_order) {
            case 1:
                return p1.arrival_time > p2.arrival_time;
            case 2:
                return p1.burst_time > p2.burst_time;
            case 3:
                return p1.priority > p2.priority;
            default:
                return false;
        }
    }
};

class Cpuscheduling {
private:
    int Scheduling_order;
    static const int order_by_priority = 3, order_by_burst_time = 2, order_by_arrival_time = 1;
    int no_of_processes;
    vector<Process> processes;

public:
    Cpuscheduling() : Scheduling_order(0), no_of_processes(0) {}

    void Read_Input(const string& filename) {
        ifstream inputFile(filename);
        string scheduler;
        int time_quanta = 0;
        if (inputFile.is_open()) {
            inputFile >> scheduler;
            if (scheduler == "RR") {
                inputFile >> time_quanta;
            }
            inputFile >> no_of_processes;
            processes.reserve(no_of_processes);
            for (int i = 0; i < no_of_processes; ++i) {
                int pid, arrival_time, burst_time, priority;
                inputFile >> pid >> arrival_time >> burst_time >> priority;
                processes.emplace_back(pid, arrival_time, burst_time, priority);
            }
            inputFile.close();
            if (scheduler == "RR") {
                cout << "RR " << time_quanta << endl;
                implementscheduler_RR(time_quanta);
            } else if (scheduler == "SJF") {
                cout << "SJF" << endl;
                implementscheduler_SJF();
            } else if (scheduler == "PR_noPREMP") {
                cout << "PR_noPREMP" << endl;
                implementPR_noPREMP();
            } else if (scheduler == "PR_withPREMP") {
                cout << "PR_withPREMP" << endl;
                implementPR_withPREMP();
            }
        } else {
            cerr << "File not found." << endl;
        }
    }

    void average_waiting_time() {
        int total_waiting_time = 0;
        for (int i = 0; i < no_of_processes; ++i) {
            total_waiting_time += processes[i].waitingTime;
            processes[i].reset();
        }
        cout << fixed << setprecision(2) << "AVG Waiting Time: " << static_cast<double>(total_waiting_time) / static_cast<double>(no_of_processes) << endl;
    }

    void implementscheduler_RR(int time_quanta) {
        queue<Process> ready_queue;
        Scheduling_order = order_by_arrival_time;
        sort(processes.begin(), processes.end(), CompareProcesses(Scheduling_order));
        int last = -1, time = 0, process_done = 0;
        Process* running_in_cpu = nullptr;
        while (process_done < no_of_processes) {
            for (int i = 0; i < no_of_processes; ++i) {
                if (last < processes[i].arrival_time && processes[i].arrival_time <= time) {
                    ready_queue.push(processes[i]);
                }
            }
            if (running_in_cpu != nullptr && running_in_cpu->completed != 0) {
                ready_queue.push(*running_in_cpu);
            }
            *running_in_cpu = ready_queue.front();
            ready_queue.pop();
            last = time;
            cout << time << " " << running_in_cpu->pid << endl;
            time += running_in_cpu->work(time_quanta, time);
            if (running_in_cpu->completed == 0) {
                ++process_done;
                running_in_cpu->waitingTime = time - running_in_cpu->burst_time - running_in_cpu->arrival_time;
            }
        }
        average_waiting_time();
    }

    void implementscheduler_SJF() {
        vector<Process> ready_queue;
        Scheduling_order = order_by_burst_time;
        sort(processes.begin(), processes.end(), CompareProcesses(Scheduling_order));
        Process* running_in_cpu = nullptr;
        int process_done = 0, time = 0, last = -1;
        while (process_done < no_of_processes) {
            for (int i = 0; i < no_of_processes; ++i) {
                if (last < processes[i].arrival_time && processes[i].arrival_time <= time) {
                    ready_queue.push_back(processes[i]);
                }
            }
            sort(ready_queue.begin(), ready_queue.end(), CompareProcesses(Scheduling_order));
            *running_in_cpu = ready_queue[0];
            ready_queue.erase(ready_queue.begin());
            last = time;
            cout << time << " " << running_in_cpu->pid << endl;
            time += running_in_cpu->work(0, time);
            if (running_in_cpu->completed == 0) {
                ++process_done;
                running_in_cpu->waitingTime = time - running_in_cpu->burst_time - running_in_cpu->arrival_time;
            }
        }
        average_waiting_time();
    }

    void implementPR_noPREMP() {
        vector<Process> ready_queue;
        Scheduling_order = order_by_arrival_time;
        sort(processes.begin(), processes.end(), CompareProcesses(Scheduling_order));
        Process* running_in_cpu = nullptr;
        int process_done = 0, time = 0, last = -1;
        while (process_done < no_of_processes) {
            for (int i = 0; i < no_of_processes; ++i) {
                if (last < processes[i].arrival_time && processes[i].arrival_time <= time) {
                    ready_queue.push_back(processes[i]);
                }
            }
            Scheduling_order = order_by_priority;
            sort(ready_queue.begin(), ready_queue.end(), CompareProcesses(Scheduling_order));
            *running_in_cpu = ready_queue[0];
            ready_queue.erase(ready_queue.begin());
            last = time;
            cout << time << " " << running_in_cpu->pid << endl;
            time += running_in_cpu->work(0, time);
            if (running_in_cpu->completed == 0) {
                ++process_done;
                running_in_cpu->waitingTime = time - running_in_cpu->burst_time - running_in_cpu->arrival_time;
            }
        }
        average_waiting_time();
    }

    void implementPR_withPREMP() {
        vector<Process> ready_queue;
        Scheduling_order = order_by_arrival_time;
        sort(processes.begin(), processes.end(), CompareProcesses(Scheduling_order));
        Process* running_in_cpu = nullptr;
        int process_done = 0, time = 0, last = -1;
        while (process_done < no_of_processes) {
            for (int i = 0; i < no_of_processes; ++i) {
                if (last < processes[i].arrival_time && processes[i].arrival_time <= time) {
                    ready_queue.push_back(processes[i]);
                }
            }
            if (running_in_cpu != nullptr && running_in_cpu->completed != 0) {
                ready_queue.push_back(*running_in_cpu);
            }
            Scheduling_order = order_by_priority;
            sort(ready_queue.begin(), ready_queue.end(), CompareProcesses(Scheduling_order));
            *running_in_cpu = ready_queue[0];
            ready_queue.erase(ready_queue.begin());
            int timeTaken = time + running_in_cpu->completed;
            int interruptTime = 0;
            for (int i = 0; i < no_of_processes; ++i) {
                if (last < processes[i].arrival_time && processes[i].arrival_time < timeTaken && processes[i].priority < running_in_cpu->priority) {
                    interruptTime = processes[i].arrival_time - time;
                    break;
                }
            }
            last = time;
            cout << time << " " << running_in_cpu->pid << endl;
            time += running_in_cpu->work(interruptTime, time);
            if (running_in_cpu->completed == 0) {
                ++process_done;
                running_in_cpu->waitingTime = time - running_in_cpu->burst_time - running_in_cpu->arrival_time;
            }
        }
        average_waiting_time();
    }
};

int main(int argc, char** argv) {
    if (argc == 1) {
        cerr << "Missing mandatory file name in argument list" << endl;
        return 1;
    }
    string file_name = argv[1];
    Cpuscheduling cpu;
    cpu.Read_Input(file_name);
    return 0;
}
