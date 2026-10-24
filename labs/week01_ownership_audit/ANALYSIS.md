1. Dangling Reference
Sanitizer Report: The sanitizer can report an error related to accessing memory after a function has returned.
Actual Problem: The strongest_rune() function returned a reference to a local variable that no longer existed after the function ended.

2. Double Delete
Sanitizer Report: AddressSanitizer can report a double-free error when the program tries to free the same memory twice.
Actual Problem: The program deleted the Gale rune separately and then tried to delete it again when cleaning up the entire satchel.

3. Memory Leak
Sanitizer Report: LeakSanitizer can report a memory leak because allocated memory was never released.
Actual Problem: The Spare rune was created with new, but the program never deleted it, causing the memory to remain allocated.

Smart Pointer Changes
I replaced the owning raw pointers with unique_ptr because each rune only needs one owner. I also removed the manual delete operations because smart pointers automatically clean up memory when it is no longer needed.
I kept the non-owning pointers and references because they only access the runes without owning them.
