#include "CircularBuffer.hpp"

template<typename T, size_t Size>
int circ_bbuf_push(circ_bbuf_t<T, Size> *cbuf, T value) {
    if (cbuf->isFull) {
        return -1; // Buffer is full
    }
    cbuf->buffer[cbuf->head] = value;
    cbuf->head = (cbuf->head + 1) % cbuf->maxlen;
    cbuf->items++;
    if (cbuf->head == cbuf->tail) {
        cbuf->isFull = true;
    }
    return 0; // Success
}

template<typename T, size_t Size>
int circ_bbuf_pop(circ_bbuf_t<T, Size> *cbuf, T *value) {
    if (cbuf->items == 0) {
        return -1; // Buffer is empty
    }
    *value = cbuf->buffer[cbuf->tail];
    cbuf->tail = (cbuf->tail + 1) % cbuf->maxlen;
    cbuf->items--;
    cbuf->isFull = false;
    return 0; // Success
}

template<typename T, size_t Size, typename Func>
void circ_bbuf_iterate(circ_bbuf_t<T, Size> *cbuf, Func func) {
    int index = cbuf->tail;
    for (uint32_t i = 0; i < cbuf->items; ++i) {
        func(cbuf->buffer[index]);
        index = (index + 1) % cbuf->maxlen;
    }
}

