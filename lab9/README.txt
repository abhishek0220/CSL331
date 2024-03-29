Lab 9:  Workflows: Parallel searching

Input: MAX- Arrays of different sets of integers (either user supplied from a file arraydata.txt or randomly generated),   M random query integers (querydata.txt) ---arrays are new-line (empty line) separated. Array values are space separated.

Output: Locations of each integer in each array  (if found).

Task: 
1. Searching is always preceded by sorting.
2. Create a single thread to sort each array. Multiple searching threads, one per array.
3. The sorting thread signals a searching thread that will begin searching for the corresponding query integer.
4. The sorting thread can go back to sorting other arrays after the signaling.
5. The output of searching thread can be immediately shown after the search is complete.


Specifications:

1. Initially, is only one array. 
2. As time goes by, more arrays are added to the search space.
3. Maximum number of arrays is MAX.
4. Once all the M integers are complete then the search stops.
5. Each integer is searched in all available arrays.