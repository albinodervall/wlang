// TODO: QueryPerformanceCounter is limited to 10 MHz on Windows, If we start doing very
//       fine grained profiling, we need to change these to higher resolution timers (e.g.
//       rdtsc on x86_64).
#if defined(__APPLE__)
    // TODO: I do not like this...
     #include <time.h>
#endif

f64 get_time() {
    #if defined(_WIN64)
        LARGE_INTEGER frequency;
        ASSERT(QueryPerformanceFrequency(&frequency));

        LARGE_INTEGER counter;
        ASSERT(QueryPerformanceCounter(&counter));

        counter.QuadPart *= 1000000000;
        counter.QuadPart /= frequency.QuadPart;

        return (f64)counter.QuadPart / 1E9;
    #elif defined(__APPLE__)
        return (f64)clock_gettime_nsec_np(CLOCK_UPTIME_RAW) / 1E9;
    #endif
}
