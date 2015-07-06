#include "cachesim.h"
#include <stdlib.h>
#include <stdio.h>


struct cache_info{ 
    uint64_t tagBits;
    uint64_t indexBits;
    uint64_t offsetBits;
    uint64_t num_block_set;
    uint64_t num_sets;

    uint64_t C;
    uint64_t S;
    uint64_t B;
} ;

typedef struct { 
    uint64_t tag;
    int  valid;
    int  dirty;
    int lru;

} cache_meta ;


typedef struct { 
    cache_meta* blocks;
} cache_set;

uint64_t  addrTag(uint64_t address);
uint64_t  addrIndex(uint64_t address);
uint64_t  addrOffset(uint64_t address);

struct cache_info info;
cache_set* cacheSet;
//int hit = 0;
//cache_total* cache;
/**
 * Subroutint for initializing your cache with the paramters.
 * You may add and initialize any global variables that you might need
 *
 * @param C The total size of your cache is 2^size bytes
 * @param S The total number of sets in your cache are 2^S
 * @param B The size of your block is 2^B bytes
 */
void cache_init(uint64_t C, uint64_t S, uint64_t B) {
    //printf("Good at 52");
    info.C = C;
    info.S = S;
    info.B = B;
    info.offsetBits = B;
    info.indexBits = C-B-S;
    info.tagBits = 64 - info.indexBits - info.offsetBits;
    info.num_block_set = (uint64_t) ((uint64_t)1 << (info.indexBits));//num blocks per set
    info.num_sets = (uint64_t) ((uint64_t)1 << S);

    cacheSet = (cache_set*) calloc (info.num_sets, sizeof(cache_set));
    int i,j;
    for(i = 0; i < info.num_sets; i++) {
        cacheSet[i].blocks = (cache_meta*)calloc(info.num_block_set, sizeof(cache_meta));

        for (j = 0; j < info.num_block_set; j++) {
            cacheSet[i].blocks[j].tag = 0;
            cacheSet[i].blocks[j].dirty = 0;
            cacheSet[i].blocks[j].valid = 0;
            cacheSet[i].blocks[j].lru = 0;
        }
    }
    //printf("Good at 77");
}

/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats) {
    //printf("Good at 100");
    stats->accesses += 1;
    if(rw == 'r') {
        stats-> reads++;
    } else {
        stats-> writes++;
    }
    uint64_t tag = addrTag(address);
    uint64_t index = addrIndex(address);
    uint64_t offset = addrOffset(address);
    // printf("address: %" PRIu64 "\n", address);
    // printf("tag: %" PRIu64 "\n", tag);
    // printf("index: %" PRIu64 "\n", index);


    int i;
    int isHit = 0;
    for (i = 0; i < info.num_sets; i++){
        //if hit
        if(cacheSet[i].blocks[index].tag == tag & cacheSet[i].blocks[index].valid == 1){
            if(rw =='w'){
                cacheSet[i].blocks[index].dirty = 1;
            }
            cacheSet[i].blocks[index].valid = 1;
            cacheSet[i].blocks[index].lru = 1;
            isHit = 1;
            //hit++;
        } else if(cacheSet[i].blocks[index].lru != 0 ){
            cacheSet[i].blocks[index].lru += 1;
        }
    }
    if(isHit == 1) {return;}


    int victim = 0;
    int passValid = 0;
    for (i = 0; i < info.num_sets; i++){
        if(cacheSet[i].blocks[index].valid == 0 & passValid != 1){
            victim = i;
            passValid = 1;
        } else if(cacheSet[i].blocks[index].lru != 0 ){
            cacheSet[i].blocks[index].lru += 1;
        }
    }

    if(passValid != 1){
        for (i = 0; i < info.num_sets; i++){
            if(cacheSet[i].blocks[index].lru > cacheSet[victim].blocks[index].lru){
                victim = i;
            }
        }
    }
    if(cacheSet[victim].blocks[index].dirty){
        stats -> write_backs += 1;
    }

    stats->misses += 1;
    cacheSet[victim].blocks[index].tag = tag;
    cacheSet[victim].blocks[index].valid = 1;
    cacheSet[victim].blocks[index].lru = 1;

    if(rw == 'r'){
        stats->read_misses += 1;
        cacheSet[victim].blocks[index].dirty = 0;
    } else {
        stats->write_misses += 1;
        cacheSet[victim].blocks[index].dirty = 1;       
    }
}

/**
 * Subroutine for cleaning up memory operations and doing any calculations
 *
 */
void cache_cleanup (struct cache_stats_t *stats) {
    int i,j;
    for(i = 0; i < info.num_sets; i++) {
            free(cacheSet[i].blocks);
    }
    free(cacheSet);
    stats -> miss_rate = (stats ->misses)/((double)stats ->accesses);
    stats -> avg_access_time = 2 + (stats -> miss_rate) * (stats -> miss_penalty);

    //printf("hit: %d" "\n", hit);
}

uint64_t  addrTag(uint64_t address) {
    return address >> (info.offsetBits + info.indexBits);
}
uint64_t  addrIndex(uint64_t address) {
    return (address >> (info.offsetBits)) & (info.num_block_set - 1);
}
uint64_t  addrOffset(uint64_t address) {
    return address & ((uint64_t)1 << info.offsetBits);
}