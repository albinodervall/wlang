// #define SUBARRAY_SIZE 4096
// #define SUBARRAY_SIZE_MASK (SUBARRAY_SIZE - 1)
// #define SUBARRAY_SIZE_SHIFT 12
#define SUBARRAY_SIZE 32
#define SUBARRAY_SIZE_MASK (SUBARRAY_SIZE - 1)
#define SUBARRAY_SIZE_SHIFT 5

template<class T>
struct Array {
    u64 number_of_elements = 0;

    u64 number_of_subarrays_allocated = 0;
    T ** pointer = NULL;

    T & operator[](u64 index) {
        ASSERT(index < number_of_elements);

        u64 outer_index = ((index & ~SUBARRAY_SIZE_MASK) >> SUBARRAY_SIZE_SHIFT);
        ASSERT(outer_index < number_of_subarrays_allocated);

        u64 inner_index = (index & SUBARRAY_SIZE_MASK);
        ASSERT(inner_index < SUBARRAY_SIZE);

        return pointer[outer_index][inner_index];
    }

    T const & operator[](u64 index) const {
        ASSERT(index < number_of_elements);

        u64 outer_index = ((index & ~SUBARRAY_SIZE_MASK) >> SUBARRAY_SIZE_SHIFT);
        ASSERT(outer_index < number_of_subarrays_allocated);

        u64 inner_index = (index & SUBARRAY_SIZE_MASK);
        ASSERT(inner_index < SUBARRAY_SIZE);

        return pointer[outer_index][inner_index];
    }
};

template<typename T>
struct Array_Iterator {
    Array<T> & array;
    u64 index;

    T & operator*() {
        return array[index];
    }
};

template<typename T>
bool operator==(Array_Iterator<T> const & a, Array_Iterator<T> const & b) {
    return a.index == b.index;
}

template<typename T>
Array_Iterator<T> & operator++(Array_Iterator<T> & a) {
    a.index += 1;
    return a;
}

template<typename T>
struct Array_Constant_Iterator {
    Array<T> const & array;
    u64 index;

    T const & operator*() {
        return array[index];
    }
};

template<typename T>
bool operator==(Array_Constant_Iterator<T> const & a, Array_Constant_Iterator<T> const & b) {
    return a.index == b.index;
}

template<typename T>
Array_Constant_Iterator<T> & operator++(Array_Constant_Iterator<T> & a) {
    a.index += 1;
    return a;
}

template<class T>
static Array_Iterator<T> begin(Array<T> & array) {
    return Array_Iterator<T>{ .array = array, .index = 0 };
}

template<class T>
static Array_Constant_Iterator<T> begin(Array<T> const & array) {
    return Array_Constant_Iterator<T>{ .array = array, .index = 0 };
}

template<class T>
static Array_Iterator<T> end(Array<T> & array) {
    return Array_Iterator<T>{ .array = array, .index = array.number_of_elements };
}

template<class T>
static Array_Constant_Iterator<T> end(Array<T> const & array) {
    return Array_Constant_Iterator<T>{ .array = array, .index = array.number_of_elements };
}

template<class T>
static T & push(Array<T> & array, T const & value) {
    if (array.number_of_elements + 1 > SUBARRAY_SIZE * array.number_of_subarrays_allocated) {
        array.number_of_subarrays_allocated += 1;
        array.pointer = (T **)reallocate(array.pointer, array.number_of_subarrays_allocated * sizeof(T *));

        array.pointer[array.number_of_subarrays_allocated - 1] = (T *)allocate(SUBARRAY_SIZE * sizeof(T));
    }

    array.number_of_elements += 1;

    u64 index = (array.number_of_elements - 1);
    array[index] = value;

    return array[index];
}

template<class T>
static void push(Array<T> & array, Array<T> const & values) {
    for (T const & value : values) {
        push(array, value);
    }
}

template<class T>
static T pop(Array<T> & array) {
    array.number_of_elements -= 1;

    return array.pointer[array.number_of_elements];
}

template<class T>
static void format(Formatter & formatter, Format const & specifier, Array<T> const & array) {
    for (T const & value : array) {
        if (&value == &array[array.number_of_elements - 1]) {
            format(formatter, "{}", value);
        } else {
            format(formatter, "{}{!}", value, specifier.delimiter);
        }
    }
}
