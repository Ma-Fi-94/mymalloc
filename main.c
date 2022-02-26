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


// Head of our global linked list of metadata records
void* HEAD = NULL;


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
        block = request_space(NULL, size);
        
        // Return NULL if request has failed.
        if (!block) {
            return NULL;
        }
        
        // Otherwise, the requested block becomes our new list head.
        HEAD = block;
    
        
    // Not the first call -- there are already blocks on the global list
    } else {
        // We will search through our list soon, and while doing so,
        // store the last block of the list here
        // TODO: This seems kinda optimisable... can't we store a global TAIL?
        struct metadata* last = NULL;
        
        // Try to find a free block on the list
        block = find_free_block(&last, size);
        
        // If we found a suitable free block on the list,
        // mark it as used now.
        if (block) {
            block->free = 0;
        
        // Else, we need to request more memory from the OS
        } else {
            // here, we need 'last' to know where to continue our list
            block = request_space(last, size);
            
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


int main() {
    find_free_block(100);
    return 0;
}
