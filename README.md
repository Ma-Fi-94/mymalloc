# mymalloc
Very much WIP right now!

I'm following an online tutorial (https://danluu.com/malloc-tutorial/) for writing a simple memory manager in plain C. I'm slightly reformating the code, annotating it extensively, and slightly imporve upon it (I think...).

## Changes to the original alreday implemented:
  - Introduced pointer TAIL to last element of global linked list, so that we don't have to search for it anew every time. In my opinion, this makes the code slightly easier to understand, as we avoid the double pointer in find_free_block().
  - Removed sbrk(0) before sbrk(META_SIZE + size) in request_space(), because as far as I can tell this is not required.
  - Added block splitting when a free block is recycled that's too large (has been suggested as additional exercise in the tutorial).

## Additionally planned changes:
  - Add block merging upon freeing of blocks (has been suggested as additional exercise in the tutorial).
  - Add a best-fit search as an alternative to the current first-fit search.
