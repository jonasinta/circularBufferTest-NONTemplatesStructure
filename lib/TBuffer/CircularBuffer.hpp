#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <Arduino.h>

// Define the circular buffer structure as a template
template<typename T, size_t Size>
struct circ_bbuf_t {
    T buffer[Size];
    int head;
    int tail;
    const int maxlen;
    uint32_t items;
    boolean isFull;

    circ_bbuf_t() : head(0), tail(0), maxlen(Size), items(0), isFull(false) {}
};

// Function declarations
template<typename T, size_t Size>
int circ_bbuf_push(circ_bbuf_t<T, Size> *, T);

template<typename T, size_t Size>
int circ_bbuf_pop(circ_bbuf_t<T, Size> *, T *);

template<typename T, size_t Size, typename Func>
void circ_bbuf_iterate(circ_bbuf_t<T, Size> *, Func);



#include "CircularBuffer.tpp"

#endif // CIRCULARBUFFER_HPP