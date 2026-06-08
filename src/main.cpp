#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

// -----------------------------------------
// 1. DATA STRUCTURE: Task Struct
// -----------------------------------------
struct Task {
    int id;
    string name;
    int deadline;
    int profit;

    // Default constructor
    Task() : id(0), name(""), deadline(0), profit(0) {}
    Task(int i, string n, int d, int p) : id(i), name(n), deadline(d), profit(p) {}
};

// -----------------------------------------
// 2. DATA STRUCTURE: Custom Max-Heap
// -----------------------------------------
class MaxHeap {
private:
    vector<Task> heap;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return (2 * i) + 1; }
    int rightChild(int i) { return (2 * i) + 2; }

    void heapify_up(int i) {
        while (i != 0 && heap[parent(i)].profit < heap[i].profit) {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    void heapify_down(int i) {
        int largest = i;
        int left = leftChild(i);
        int right = rightChild(i);
        int n = heap.size();

        if (left < n && heap[left].profit > heap[largest].profit)
            largest = left;
        if (right < n && heap[right].profit > heap[largest].profit)
            largest = right;

        if (largest != i) {
            swap(heap[i], heap[largest]);
            heapify_down(largest);
        }
    }

public:
    void insert(Task t) {
        heap.push_back(t);
        heapify_up(heap.size() - 1);
    }

    Task extractMax() {
        if (heap.empty()) return Task(-1, "", -1, -1);
        if (heap.size() == 1) {
            Task root = heap[0];
            heap.pop_back();
            return root;
        }

        Task root = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapify_down(0);

        return root;
    }

    bool isEmpty() { return heap.empty(); }
    int size() { return heap.size(); }
};

// -----------------------------------------
// 3. DATA STRUCTURE: Disjoint Set Union (DSU)
// -----------------------------------------
class DSU {
private:
    vector<int> parent;
public:
    DSU(int n) {
        parent.resize(n + 1);
        for (int i = 0; i <= n; i++) {
            parent[i] = i; // Initially, every slot is its own parent
        }
    }

    // Find with Path Compression
    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]); 
    }

    void unite(int u, int v) {
        parent[u] = v; // u merges into v
    }
};

// -----------------------------------------
// 4. CORE ENGINE: Task Scheduler
// -----------------------------------------
class TaskScheduler {
private:
    MaxHeap taskHeap;
    vector<Task> allTasks;
    vector<Task> missedTasks;
    int maxDeadline;

public:
    TaskScheduler() : maxDeadline(0) {}

    void loadTasks(const string& filepath) {
        ifstream file(filepath);
        if (!file.is_open()) {
            cerr << "[!] Error: Cannot open " << filepath << ". Make sure data/tasks.csv exists.\n";
            return;
        }

        string line, word;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            string id_str, name, deadline_str, profit_str;

            getline(ss, id_str, ',');
            getline(ss, name, ',');
            getline(ss, deadline_str, ',');
            getline(ss, profit_str, ',');

            Task t(stoi(id_str), name, stoi(deadline_str), stoi(profit_str));
            allTasks.push_back(t);
            taskHeap.insert(t);
            
            if (t.deadline > maxDeadline) {
                maxDeadline = t.deadline;
            }
        }
        cout << "[*] Successfully loaded " << allTasks.size() << " tasks into the Max-Heap.\n";
    }

    void optimizeSchedule() {
        // schedule[i] will store the task assigned to time slot i
        vector<Task> schedule(maxDeadline + 1); 
        DSU dsu(maxDeadline);
        int totalProfit = 0;
        int scheduledCount = 0;

        cout << "\n========================================\n";
        cout << "🏆 OPTIMIZED TASK SCHEDULE (C++ DSU) 🏆\n";
        cout << "========================================\n";
        printf("%-10s | %-20s | %-6s\n", "Time Slot", "Task Name", "Profit");
        cout << "----------------------------------------\n";

        // Process tasks from highest profit to lowest
        while (!taskHeap.isEmpty()) {
            Task currentTask = taskHeap.extractMax();

            // Find maximum available time slot <= currentTask.deadline
            int availableSlot = dsu.find(currentTask.deadline);

            if (availableSlot > 0) {
                // Assign task to this slot
                schedule[availableSlot] = currentTask;
                totalProfit += currentTask.profit;
                scheduledCount++;
                
                // Union the filled slot with the previous slot
                dsu.unite(availableSlot, dsu.find(availableSlot - 1));
            } else {
                missedTasks.push_back(currentTask);
            }
        }

        // Display Schedule in chronological order
        for (int i = 1; i <= maxDeadline; i++) {
            if (schedule[i].id != 0) {
                printf("Slot %-5d | %-20s | $%d\n", i, schedule[i].name.c_str(), schedule[i].profit);
            }
        }

        cout << "\n========================================\n";
        cout << "❌ MISSED TASKS (Insufficient Time) ❌\n";
        cout << "========================================\n";
        for (const auto& t : missedTasks) {
            cout << "- " << t.name << " (Deadline: " << t.deadline << ", Profit: $" << t.profit << ")\n";
        }

        cout << "\n========================================\n";
        cout << "💰 TOTAL OPTIMIZED PROFIT: $" << totalProfit << "\n";
        cout << "========================================\n\n";
        
        generateReport(schedule);
    }

    void generateReport(const vector<Task>& schedule) {
        ofstream file("outputs/schedule_report.csv");
        file << "Time Slot,TaskID,Name,Deadline,Profit\n";
        for (int i = 1; i <= maxDeadline; i++) {
            if (schedule[i].id != 0) {
                file << i << "," << schedule[i].id << "," << schedule[i].name 
                     << "," << schedule[i].deadline << "," << schedule[i].profit << "\n";
            }
        }
        cout << "[*] System report successfully saved to outputs/schedule_report.csv\n";
    }
};

int main() {
    TaskScheduler system;
    system.loadTasks("data/tasks.csv");
    system.optimizeSchedule();
    return 0;
}