#include "cachesim.h"
#include <math.h>
/**
 * Subroutint for initializing your cache with the paramters.
 * You may add and initialize any global variables that you might need
 *
 * @param C The total size of your cache is 2^size bytes
 * @param S The total number of sets in your cache are 2^S
 * @param B The size of your block is 2^B bytes
 */
void cache_init(uint64_t C, uint64_t S, uint64_t B) {
    

}

/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats) {

}

/**
 * Subroutine for cleaning up memory operations and doing any calculations
 *
 */
void cache_cleanup (struct cache_stats_t *stats) {

}

int addrTag(int address){
    int blockBitNum = C-B;
    int rowBitNum = blockNum - S;
    return addr >>  (B + rowBitNum);
}
int addrIndex(int address){
    int blockBitNum = C-B;
    int rowBitNum = blockBitNum - S;
    return (address >> B) & (pow(2, rowNum) - 1)
}
int addrOffset(int address) {
    return addr & (pow(2, B) - 1);
}