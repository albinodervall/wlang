template<class T>
T & push(T *& pointer, u64 & capacity, u64 & count, T const & value) {
    if (count + 1 > capacity) {
        capacity = 2 * capacity + 1;
        pointer = (T *)reallocate(pointer, capacity * sizeof(T));
    }

    pointer[count] = value;
    count += 1;

    return pointer[count - 1];
}
