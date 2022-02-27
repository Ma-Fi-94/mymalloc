// WIP, based on: https://danluu.com/malloc-tutorial/
// Commented, and modified a bit, maybe for the better (?)

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

// For every allocated block, we store some metadata
struct metadata {
  size_t size;
  int free;
  struct metadata* next;
};


// The amount of bytes we need for one block's metadata
#define META_SIZE sizeof(struct metadata)


// Head and tail of our global linked list of metadata records
struct metadata* HEAD = NULL;
struct metadata* TAIL = NULL;

// Find a free block of suitable size in the list, starting from head
struct metadata* find_free_block(size_t size) {
    struct metadata* current = HEAD;
    while (current && !(current->free && current->size >= size)) {
        current = current->next;
    }
    return current;
}


struct metadata* request_space(size_t size) {
    // New block starts at current end of heap
    struct metadata* block = sbrk(0);
    
    // Extend heap by amount of space required
    void* request = sbrk(META_SIZE + size);
    
    // Return NULL if sbrk failed
    if (request == (void*) -1) {
        return NULL;
    }
    
    // Consistency check (race conditions / thread safety)
    assert ((void*) block == request);
    
    // If there is already at least one entry in the list
    if (TAIL) {
        //...
        TAIL->next = block;
        //...
        TAIL = block;
    }
    
    // Write entry, and return it
    block->size = size;
    block->next = NULL;
    block->free = 0;
    return block;
}


void *malloc(size_t size) {
    // Evidently nonsense
    if (size <= 0) {
        return NULL;
    }

    // New block to be stored in here
    struct metadata *block;

    // First call -- our global list is still empty,
    if (!HEAD) {
        // and thus we need to request space from the OS.
        block = request_space(size);
        
        // Return NULL if request has failed.
        if (!block) {
            return NULL;
        }
        
        // Otherwise, the requested block becomes our new list head and tail.
        HEAD = block;
        TAIL = block;
    
        
    // Not the first call -- there are already blocks on the global list
    } else {

        // Try to find a free block on the list
        block = find_free_block(size);
        
        // If we found a suitable free block on the list,
        // mark it as used now.
        if (block) {
            block->free = 0;
        
        // Else, we need to request more memory from the OS
        } else {
            block = request_space(size);
            
            // if request failed, we return NULL
            if (!block) {
                return NULL;
            }
        }
    }
        
    // Return pointer to the actual block of free memory
    // (right after the metadata)
    return (block+1);
}

void print_list() {
    // TBD
    return NULL;
}

int main() {
    int* x = (int*) malloc(10*sizeof(int));
    return 0;
}
