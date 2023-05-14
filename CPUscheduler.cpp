/*Peter Abah
Homework 2
Section 3
Tested on Mac OS X 10.9.5
*/
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

struct Process
{
    int num, arrival_time, burst_time, priority, start_time, remaining_time;

    // Compare burst time of two processes
    bool operator<(const Process &rhs) const
    {
        if (burst_time == rhs.burst_time)
            return num > rhs.num;
        return burst_time > rhs.burst_time;
    }
};
// Compare arival time of two processes
bool compareArrivalTime(Process a, Process b)
{
    return a.arrival_time < b.arrival_time;
}

int main()
{ // Read input.txt and write to output.txt
    std::ifstream infile("input.txt");
    std::ofstream outfile("output.txt");
    std::string line, scheduling_algorithm;
    int n_processes, time_quantum, current_time = 0, total_waiting_time = 0;

    std::getline(infile, line);
    std::istringstream iss(line);
    iss >> scheduling_algorithm >> time_quantum;
    outfile << scheduling_algorithm << " " << time_quantum << std::endl;

    std::getline(infile, line);
    std::istringstream iss2(line);
    iss2 >> n_processes;

    std::vector<Process> processes(n_processes);
    for (int i = 0; i < n_processes; ++i)
    {
        std::getline(infile, line);
        std::istringstream iss(line);
        Process process;
        process.start_time = -1;
        iss >> process.num >> process.arrival_time >> process.burst_time >> process.priority;
        process.remaining_time = process.burst_time;
        processes[i] = process;
    }

    // If Fiirst line of input.txt is RR
    if (scheduling_algorithm == "RR")
    {
        std::queue<Process> process_queue;
        for (int i = 0; i < n_processes; ++i)
        {
            process_queue.push(processes[i]);
        }

        // Sort processes based on arrival time
        while (!process_queue.empty())
        {
            Process process = process_queue.front();
            process_queue.pop();

            // If process has not started yet, set start time to current time
            if (process.start_time == -1)
            {
                process.start_time = current_time;
            }

            // If process has not finished, push it back to the queue
            if (process.remaining_time <= time_quantum)
            {
                current_time += process.remaining_time;
                total_waiting_time += current_time - process.arrival_time - process.burst_time;
                outfile << current_time << "\t" << process.num << std::endl;
            }
            else
            {
                process.remaining_time -= time_quantum;
                current_time += time_quantum;
                process_queue.push(process);
                outfile << current_time << "\t" << process.num << std::endl;
            }
        }
    }

    // If Fiirst line of input.txt is SJF
    else if (scheduling_algorithm == "SJF")
    {
        // Sort processes based on arrival time
        std::sort(processes.begin(), processes.end(), compareArrivalTime);

        std::priority_queue<Process> pq;
        std::vector<int> waiting_times(n_processes, 0);
        int index = 0;

        // Loop until all processes have been executed
        while (index < n_processes || !pq.empty())
        {
            while (index < n_processes && processes[index].arrival_time <= current_time)
            {
                pq.push(processes[index++]);
            }

            if (!pq.empty())
            {
                Process shortest = pq.top();
                pq.pop();
                waiting_times[shortest.num - 1] = current_time - shortest.arrival_time;
                outfile << current_time << "\t" << shortest.num << "\n";
                current_time += shortest.burst_time;
            }
            else
            {

                current_time = processes[index].arrival_time;
            }
        }

        for (int i = 0; i < n_processes; i++)
        {
            total_waiting_time += waiting_times[i];
        }
    }

    // If First line of input.txt is PR_noPREMP
    else if (scheduling_algorithm == "PR_noPREMP")
    {
        struct ComparePriority
        {
            bool operator()(const Process &p1, const Process &p2)
            {
                // Lower number means higher priority
                return p1.priority > p2.priority;
            }
        };

        std::priority_queue<Process, std::vector<Process>, ComparePriority> pq;

        std::sort(processes.begin(), processes.end(), compareArrivalTime);
        int index = 0;
        std::vector<int> waiting_times(n_processes, 0);

        // Loop until all processes have been executed
        while (index < n_processes || !pq.empty())
        {
            while (index < n_processes && processes[index].arrival_time <= current_time)
            {
                pq.push(processes[index++]);
            }

            if (!pq.empty())
            {
                Process top_priority = pq.top();
                pq.pop();
                waiting_times[top_priority.num - 1] = current_time - top_priority.arrival_time;
                outfile << current_time << "\t" << top_priority.num << "\n";
                current_time += top_priority.burst_time;
            }
            else
            {
                current_time = processes[index].arrival_time;
            }
        }

        for (int i = 0; i < n_processes; i++)
        {
            total_waiting_time += waiting_times[i];
        }
    }

    // If First line of input.txt is PR_withPREMP
    else if (scheduling_algorithm == "PR_withPREMP")
    {
        struct ComparePriority
        {
            bool operator()(const Process &p1, const Process &p2)
            {
                // Lower number means higher priority
                return p1.priority > p2.priority;
            }
        };

        std::priority_queue<Process, std::vector<Process>, ComparePriority> pq;

        std::sort(processes.begin(), processes.end(), compareArrivalTime);
        int index = 0;
        std::vector<int> waiting_times(n_processes, 0);

        while (index < n_processes || !pq.empty())
        {
            while (index < n_processes && processes[index].arrival_time <= current_time)
            {
                if (!pq.empty())
                {
                    Process top_priority = pq.top();
                    if (processes[index].priority < top_priority.priority)
                    {
                        top_priority.remaining_time -= (processes[index].arrival_time - current_time);
                        current_time = processes[index].arrival_time;
                        if (top_priority.remaining_time > 0)
                        {
                            pq.push(top_priority);
                        }
                    }
                }
                pq.push(processes[index++]);
            }

            if (!pq.empty())
            {
                Process top_priority = pq.top();
                pq.pop();
                waiting_times[top_priority.num - 1] += current_time - top_priority.arrival_time;
                top_priority.arrival_time = current_time;
                outfile << current_time << "\t" << top_priority.num << "\n";
                current_time += top_priority.remaining_time;
                top_priority.remaining_time = 0;
            }
            else
            {
                current_time = processes[index].arrival_time;
            }
        }

        for (int i = 0; i < n_processes; i++)
        {
            total_waiting_time += waiting_times[i];
        }
    }

    else
    {
        outfile << "Invalid scheduling algorithm" << std::endl;
        return 1;
    }

    outfile << "AVG Waiting Time: " << (double)total_waiting_time / n_processes << std::endl;

    return 0; // End of program
}
