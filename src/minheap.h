#pragma once

#include <utility>
#include <vector>

// Binary min-heap of integer keys (actor indices 0..capacity-1), ordered by priority.
// Built from scratch for Dijkstra; std::priority_queue is not used.
//
// The heap is "indexed": pos[key] remembers where each key sits in the array, so a key's
// priority can be lowered in place (decrease-key) instead of pushing a duplicate. Each key
// is in the heap at most once, so the heap never holds more than V entries.
class IndexedMinHeap {
public:
    explicit IndexedMinHeap(int capacity) : priority(capacity, 0), pos(capacity, -1) {}

    bool empty() const { return heap.empty(); }
    int size() const { return static_cast<int>(heap.size()); }
    bool contains(int key) const { return pos[key] != -1; }

    // Inserts key with this priority, or lowers its priority if it is already in the heap.
    // A higher priority for a key already in the heap is ignored.
    // O(log n)
    void pushOrDecrease(int key, long long newPriority) {
        if (contains(key)) {
            if (newPriority >= priority[key]) {
                return;
            }
            priority[key] = newPriority;
            siftUp(pos[key]);
            return;
        }
        priority[key] = newPriority;
        heap.push_back(key);
        pos[key] = size() - 1;
        siftUp(size() - 1);
    }

    // Removes and returns the key with the smallest priority. The heap must not be empty.
    // O(log n)
    int popMin() {
        int top = heap[0];
        swapNodes(0, size() - 1);
        heap.pop_back();
        pos[top] = -1;
        if (!heap.empty()) {
            siftDown(0);
        }
        return top;
    }

private:
    std::vector<int> heap;             // keys, arranged as a binary heap
    std::vector<long long> priority;   // priority[key]
    std::vector<int> pos;              // pos[key] = index in heap, or -1 if not in the heap

    bool less(int i, int j) const { return priority[heap[i]] < priority[heap[j]]; }

    void swapNodes(int i, int j) {
        std::swap(heap[i], heap[j]);
        pos[heap[i]] = i;
        pos[heap[j]] = j;
    }

    // Moves the node at i up while it is smaller than its parent.
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (!less(i, parent)) {
                break;
            }
            swapNodes(i, parent);
            i = parent;
        }
    }

    // Moves the node at i down while a child is smaller than it.
    void siftDown(int i) {
        int n = size();
        while (true) {
            int left = 2 * i + 1;
            int right = left + 1;
            int smallest = i;
            if (left < n && less(left, smallest)) {
                smallest = left;
            }
            if (right < n && less(right, smallest)) {
                smallest = right;
            }
            if (smallest == i) {
                break;
            }
            swapNodes(i, smallest);
            i = smallest;
        }
    }
};
