/* WIP, based on: https://danluu.com/malloc-tutorial/
 * Added comments and modified a bit, maybe for the better (?)
 * 
 * Implemented to the program logic:
 *  -   Introduced pointer TAIL to last element of global linked list,
 *      so that we don't have to search for it anew every time.
 *  -   Removed sbrk(0) before sbrk(META_SIZE + size) in request_space().
 * 
 * Planned changes to the program logic:
 *  -   TBD: Add block splitting (WIP) and block merging.
 *  -   TBD: Add best-fit as alternative to current first-fit.
 * 
*/ 

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

// For every allocated block, we store some metadata
struct metadata {
  size_t size;
  int free;
  struct metadata* next;
};


// The amount of bytes we need for one block's metadata
// On my machine, it's 24 bytes.
#define META_SIZE sizeof(struct metadata)


// Head and tail of our global linked list of metadata records
struct metadata* HEAD = NULL;
struct metadata* TAIL = NULL;

// Trying to find a free block of suitable size in the list
struct metadata* find_free_block(size_t size) {
    // Starting at the HEAD of the list
    struct metadata* current = HEAD;
    
    // Iterate through list until suitable block is found
    while (current && !(current->free && current->size >= size)) {
        current = current->next;
    }
    
    return current;
}


// Request a new block of memory from the OS
struct metadata* request_space(size_t size) {
    // New block starts at current end of heap
    struct metadata* block = sbrk(META_SIZE + size);
    
    // If there is already at least one entry in the list
    if (TAIL) {
        // Set pointer *next of current TAIL block to new block
        TAIL->next = block;
        
        // New block becomes the new TAIL
        TAIL = block;
    }
    
    // Write entry, and return it
    block->size = size;
    block->next = NULL;
    block->free = 0;
    return block;
}


void *mymalloc(size_t size) {
    // Evidently nonsense
    if (size <= 0) { return NULL; }

    // New block to be stored in here
    struct metadata *block;

    // First call -- our global list is still empty,
    if (!HEAD) {
        // and thus we need to request space from the OS.
        block = request_space(size);
        
        // Return NULL if request has failed.
        if (!block) { return NULL; }
        
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
            
            // TODO: Consider splitting block to save memory
            if (block->size > size + META_SIZE) {
                printf("Splitting oversized block. \n");
                
                struct metadata* remaining = (void*) block + META_SIZE + size;
                printf("Remaining block will start at %ld\n", (long) remaining); // checked, ok.
                
                // TODO: write metadata for "remaining", and include it into our linked list.
            }
            
            
            
        
        // Else, we need to request more memory from the OS
        } else {
            block = request_space(size);
            
            // if request failed, we return NULL
            if (!block) { return NULL; }
        }
    }
        
    // Return pointer to the actual block of free memory
    // (right after the metadata)
    return (block+1);
}


// Convenience function to plot a single block of metadata
void print_block(struct metadata* block) {
    if (block) {
        printf("Adress: %li, Size: %i, Free: %i, Next: %li\n",
               (long) block,
               (int) block->size,
               block->free,
               (long) block->next);
    } else {
        printf("NULL\n");
    }
}


// Convenience function to plot the complete global linked list
void print_list() {
    printf("-------------------------------------------------\n");
    if (!HEAD) {
        printf("List is empty.\n");
        printf("-------------------------------------------------\n\n");
        return;
    }
    
    struct metadata* current = HEAD;
    while (current) {
        print_block(current);
        current = current->next;
    }
    printf("-------------------------------------------------\n\n");
}

// Convenience function to get the metadata for a block of memory
struct metadata *get_block_ptr(void *ptr) {
  return ((struct metadata*) ptr) - 1;
}


void myfree(void *ptr) {
  // Calling free(NULL) is supported
  if (!ptr) { return; }

  // TODO: consider merging blocks once splitting blocks is implemented.
  
  // Get pointer to metadata of the block of memory that shall be freed
  struct metadata* block_ptr = get_block_ptr(ptr);
  
  // Make sure it is even freeable
  assert(block_ptr->free == 0);
  
  // Free it
  block_ptr->free = 1;
}


int main() {
    print_list();
    
    printf("Allocating 500 bytes.\n");
    void* x = mymalloc(500);
    print_list();
    
    printf("Allocating another 500 bytes.\n");
    void* y = mymalloc(500);
    print_list();
    
    printf("Freeing the first block.\n");
    myfree(x);
    print_list();
    
    printf("Allocating 10 bytes.\n");
    x = mymalloc(10);
    print_list();
    
    return 0;
}
