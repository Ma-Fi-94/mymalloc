/* WIP, based on: https://danluu.com/malloc-tutorial/
 * Added comments and modified a bit, maybe for the better (?)
 * 
 * Implemented to the program logic:
 *  -   Introduced pointer TAIL to last element of global linked list,
 *      so that we don't have to search for it anew every time.
 *  -   Removed sbrk(0) before sbrk(META_SIZE + size) in request_space().
 *  -   Switched from a single-linked to a double-linked global list of memory
 *      blocks to make searching for previous blocks O(1) instead of O(n)
 *  -   Added block splitting when a free block is recycled that's too large,
 *      and block merging upon freeing.
 * 
 * Planned changes to the program logic:
 *  -   TBD: Add best-fit as alternative to current first-fit.
 * 
*/ 

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

// For every allocated block, we store some metadata
struct metadata {
  size_t size;
  int free;
  struct metadata* next;
  struct metadata* prev;
};


// The amount of bytes we need for one block's metadata
#define META_SIZE sizeof(struct metadata)


// Head and tail of our global linked list of metadata records
struct metadata* HEAD = NULL;
struct metadata* TAIL = NULL;

// Trying to find a free block of suitable size in the list.
// Return the first that fits.
struct metadata* find_first_free_block(size_t size) {
    // Starting at the HEAD of the list
    struct metadata* current = HEAD;
    
    // Iterate through list until suitable block is found
    while (current && !(current->free && current->size >= size)) {
        current = current->next;
    }
    
    return current;
}

// TODO
// Trying to find a free block of suitable size in the list.
// Return the best-fitting block
//struct metadata* find_best_free_block(size_t size) {
    // TODO
//    return NULL;
//}


// Request a new block of memory from the OS
struct metadata* request_space(size_t size) {    
    // New block starts at current end of heap
    struct metadata* block = sbrk(META_SIZE + size);
    
    // If there is already at least one entry in the list
    if (TAIL) {
        
        // Set pointer *next of current TAIL block to new block
        TAIL->next = block;
        
        // Set pointer *prev of the new block to current tail
        block->prev = TAIL;
        
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
        block = find_first_free_block(size);
        
        // If we found a suitable free block on the list,
        // mark it as used now.
        if (block) {
            block->free = 0;
            
            // If the block is sufficiently large, split it
            if (block->size > size + 2*META_SIZE) {
                
                // The new block that contains the surplus memory
                struct metadata* surplus = (void*) block + META_SIZE + size;
                
                // If the current block was TAIL, the surplus block
                // becomes the new TAIL
                if (TAIL == block) {
                    TAIL = surplus;
                }
                
                // Write metadata for the surplus block
                surplus->size = block->size - size - 2*META_SIZE;
                surplus->next = block->next;
                surplus->prev = block;
                surplus->free = 1;
                
                // Write metadata for the allocated block
                block->size = size;
                block->next = surplus;
            }
       
        // If we didn't find a suitable free block, request memory from the OS
        // to get a new block, which will become the new TAIL
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



// Convenience function to plot the complete global linked list
void print_list() {
    printf("------------------------------------------------------------------------\n");
    printf("%-20s %-20s %-7s %-6s %-20s\n", "Adress", "Previous", "Size", "Free", "Next");
    printf("------------------------------------------------------------------------\n");
    printf("HEAD is %li\n", (long int) HEAD);
    if (!HEAD) {
        printf("List is empty.\n");
    } else {
    struct metadata* current = HEAD;
    while (current) {
        printf("%-20li %-20li %-7i %-6i %-20li\n",
               (long int) current,
               (long int) current->prev,
               (int) current->size,
               current->free,
               (long int) current->next);
        current = current->next;
        }
    }
    printf("TAIL is %li\n", (long int) TAIL);
    printf("------------------------------------------------------------------------\n\n");
}

// Convenience function to get the metadata for a block of memory
struct metadata *get_block_ptr(void *ptr) {
  return ((struct metadata*) ptr) - 1;
}


void myfree(void *ptr) {
  // Calling free(NULL) is supported
  if (!ptr) { return; }
 
  // Get pointer to metadata of the block of memory that shall be freed
  struct metadata* block = get_block_ptr(ptr);
  
  // Freeing a freed block is supported
  if (block->free) { return; }
  
  // Free it
  block->free = 1;
  
  // If the block "to the right" exists and is free, we merge the two
  struct metadata* next_block = block->next;
  if (next_block && next_block->free) {
      printf("Merge to right.\n");
      
      block->next = next_block->next;
      block->size = block->size + META_SIZE + next_block->size;
      
      // If the block to the right has a successor, we set the successor's
      // *prev pointer to the block we just merged together
      struct metadata* next_next_block = next_block->next;
      if (next_next_block) {
        next_next_block->prev = block;
      }
      
    // If the block to the right was the TAIL, then
    // the merged block becomes the new TAIL.
    if (TAIL == next_block) {
        TAIL = block;
    }
      
  }
    
  // Same for the block "to the left". If it exists and is free:
  struct metadata* prev_block = block->prev;
  if (prev_block && prev_block->free) {
    // Successor of previous block is set to current block's successor
    prev_block->next = block->next;
    
    // Size of previous block is set to its size + size of one meta block
    // + size of the current block
    prev_block->size = prev_block->size + META_SIZE + block->size;

    
    // If the block to left left has a predecessor, we set the predecessor's
    // *next ptr to the block we just merged together
    struct metadata* prev_prev_block = prev_block->prev;
    if (prev_prev_block) {
        prev_prev_block->next = block;
    }
    
    // If the current block was the TAIL,
    // the newly merged block becomes the new TAIL
    if (TAIL == block) {
        TAIL = prev_block;
    }
  } 
}


void* mycalloc(size_t nelem, size_t elsize) {
  // Check for overflow
  if ( nelem > SIZE_MAX / elsize ) {
      return NULL;
  }
  
  // Allocate block, initialise with zeros and return.
  size_t size = nelem * elsize;
  void *ptr = mymalloc(size); 
  memset(ptr, 0, size);
  return ptr;
}


void *myrealloc(void *ptr, size_t size) {
  // NULL ptr. realloc should act like malloc.
  if (!ptr) { return mymalloc(size); }

  // Get metadata associated with the block of memory ptr points to
  struct metadata* block_ptr = get_block_ptr(ptr);
  
  // If we already have enough space, we don't do anything
  // TODO: Split block?
  if (block_ptr->size >= size) { return ptr; }

  // Need to really realloc.
  // Malloc new space first. Return NULL if failure
  void *new_ptr = mymalloc(size);
  if (!new_ptr) { return NULL; }

  // Copy data to new memory
  memcpy(new_ptr, ptr, block_ptr->size);
  
  // Free old block of memory
  myfree(ptr);
  
  // Return new block
  return new_ptr;
}

int main() {
    print_list();
    
    void* x;
    
    printf("Allocate 100 bytes.\n");
    x = mymalloc(100);
    print_list();    
    
    printf("Realloc to 200 bytes.\n");
    x = myrealloc(x, 200);
    print_list();
    
    printf("Free block\n");
    myfree(x);
    print_list();
    
    return 0;
}
