# mymalloc
Very much WIP right now!

I'm following an online tutorial (https://danluu.com/malloc-tutorial/) for writing a simple memory manager in plain C. I'm slightly refactoring the code, annotating it extensively, and also slightly improve upon it (at least, I think I do...).

## Changes to the original alraedy implemented:
  - Introduced pointer TAIL to last element of the global linked list of memory blocks, so that we don't have to search for it anew every time. In my opinion, this makes the code slightly easier to understand, as we avoid the double pointer in the parameters of find_free_block() that gets changed during search.
  - Removed sbrk(0) before sbrk(META_SIZE + size) in request_space(), because as far as I can tell this is not required.
  - Added a convenience function to print the current state of the global linked list of memory blocks.
  - Added block splitting when a free block is recycled which is too large (has been suggested as additional exercise in the tutorial).
  - Added block merging upon freeing of blocks (has been suggested as additional exercise in the tutorial).

## Additionally planned changes:
  - Switch to a double-linked list instead of a single-linked one to make finding the left neighbour happen in O(1) instead of O(n).
  - Add a best-fit search as an alternative to the current first-fit search in find_free_block().
