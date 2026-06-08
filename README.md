# 🚀 High-Performance Task Scheduler System (C++)

An enterprise-grade scheduling system built in C++ to solve the Job Sequencing with Deadlines problem. Engineered for maximum speed utilizing custom data structures, bypassing standard libraries to demonstrate systems-level algorithm design.

## 🧠 Algorithmic Core
* **Time Complexity:** $\mathcal{O}(N \log N)$ for heap operations + $\mathcal{O}(N \alpha(N))$ for slot allocation.
* **Custom Max-Heap:** Implemented via contiguous arrays (`std::vector`) with custom `heapify_up` and `heapify_down` methods to aggressively extract highest-profit tasks.
* **Disjoint Set Union (DSU):** Replaces the standard $\mathcal{O}(N^2)$ nested-loop slot searching mechanism. Utilizes **Path Compression** to assign tasks to the latest available time slots in near-constant time.

## 🛠️ Build & Run
```bash
# Compile using g++ with O3 optimizations
g++ -O3 src/main.cpp -o scheduler

# Execute
./scheduler