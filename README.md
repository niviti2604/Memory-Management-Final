
# Memory Management Simulator

**Author:** Niviti Sharma  
**Student ID:** 23117093  
**Language:** C++17  

---

## Overview
This project is a comprehensive simulation of an Operating System's Memory Management Unit (MMU). It models the interaction between the CPU, Cache, and Main Memory, offering a detailed analysis of memory fragmentation, allocation efficiency, and cache hierarchy performance.

The system is designed to visualize how different allocation strategies impact system resources and how multi-level caching improves data access times.

<img width="541" alt="System Architecture Diagram" src="https://github.com/user-attachments/assets/0bae3b3c-a049-451c-a603-ec0013e699f4" />

---

##  Core Modules

### 1. Memory Management
* **Files:** `memory.hpp`, `memory.cpp`
* **Data Structure:** Contiguous memory region managed via a **Linked List** of blocks.
* **Key Features:**
    * Dynamic allocation & deallocation.
    * **Coalescing:** Automatically merges adjacent free blocks to reduce external fragmentation.
    * **Fragmentation Tracking:** Monitors both internal and external fragmentation in real-time.
    * **Memory Dump:** Visualizes the memory map for debugging.

### 2. Allocators (Strategy Pattern)
* **Files:** `allocator.hpp`, `first_fit.cpp`, `best_fit.cpp`, `worst_fit.cpp`
* **Description:** Implements specific algorithms to determine where data is stored in the heap.

| Strategy | Description | Pros |
| :--- | :--- | :--- |
| **First Fit** | Allocates the *first* block capable of holding the data. | Fast execution. |
| **Best Fit** | Scans for the *smallest* block that fits the data. | Minimizes internal fragmentation. |
| **Worst Fit** | Allocates the *largest* available block. | Leaves large chunks for future large requests. |

### 3. Cache Simulator
* **Files:** `cache.hpp`, `cache.cpp`
* **Features:**
    * Configurable hierarchy (L1, L2, L3).
    * Set-associative arrays with per-set replacement lists.
    * **Replacement Policies:** FIFO (First In First Out), LRU (Least Recently Used), LFU (Least Frequently Used).
    * Tracks hit/miss ratios per level.


### 4. Statistics
* **Files:** `stats.hpp`, `stats.cpp`
* **Function:** Acts as an observer to report memory utilization, allocation success rates, and effective memory access time.

---

##  Technical Implementation

The project utilizes advanced C++ features and Design Patterns to ensure modularity and extensibility.

### Data Structures
* **Memory:** Linked List of Block nodes.
* **Cache:** Set-associative arrays.

### Design Patterns
1.  **Strategy Pattern:** Used for swapping Allocation algorithms (First/Best/Worst fit) at runtime without changing the core memory logic.
2.  **Factory Pattern:** Used for the clean creation of Allocator instances.
3.  **Observer Pattern:** Used by the Statistics module to gather metrics non-intrusively.

---

##  Build & Run Instructions

This project uses a `Makefile` for compilation. Ensure you have `g++` and `make` installed.

### 1. Compilation
To clean previous builds and compile the fresh executable:

```bash
make clean && make
