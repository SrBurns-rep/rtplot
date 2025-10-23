#ifndef _RING_H
#define _RING_H

// Use this to make a live updating graph with raylib

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Sample {
    uint16_t is_valid : 1;
    uint16_t meta     : 5;
    uint16_t raw      : 10;
} Sample;

typedef struct SampleRing {
    size_t head;
    size_t size;
    Sample samples[];
} SampleRing;

SampleRing *ring_init(size_t size) {
    SampleRing *new_ring = malloc(sizeof(SampleRing) + size * sizeof(Sample));

    if(new_ring) {
        new_ring->head = 0;
        new_ring->size = size;
    }

    return new_ring;
}

void ring_push_sample(SampleRing *ring, Sample sample) {
    ring->samples[ring->head] = sample;
    ring->head++;
    ring->head %= ring->size;
}

void ring_print_samples(SampleRing *ring) {
    for(size_t i = ring->head; i < ring->size; i++) {
        if(ring->samples[i].is_valid) {
            printf("[%02lu] = %u\n", i, ring->samples[i].raw);
        } else {
            printf("[%02lu] = NULL\n", i);
        }
    }

    for(size_t i = 0; i < ring->head; i++) {
        if(ring->samples[i].is_valid) {
            printf("[%02lu] = %u\n", i, ring->samples[i].raw);
        } else {
            printf("[%02lu] = NULL\n", i);
        }
    }
}

#endif /* _RING_H */
