template<class ... T> void print(utf8 const * string, T ... arguments);
template<class ... T> utf8 * format(utf8 const * string, T ... arguments);

#define TRACE 1

#if TRACE
    #if (__APPLE__)
        extern "C" int _NSGetExecutablePath(char*, u32*);
        extern "C" u32 _dyld_image_count(void);
        extern "C" const char * _dyld_get_image_name(u32);

        struct segment_command_64 { /* for 64-bit architectures */
            u32    cmd;        /* LC_SEGMENT_64 */
            u32    cmdsize;    /* includes sizeof section_64 structs */
            char        segname[16];    /* segment name */
            u64    vmaddr;     /* memory address of this segment */
            u64    vmsize;     /* memory size of this segment */
            u64    fileoff;    /* file offset of this segment */
            u64    filesize;   /* amount to map from the file */
            int   maxprot;    /* maximum VM protection */
            int   initprot;   /* initial VM protection */
            u32    nsects;     /* number of sections in segment */
            u32    flags;      /* flags */
        };

        extern "C" const struct segment_command_64 * getsegbyname(char const *);
        extern "C" unsigned long _dyld_get_image_vmaddr_slide(u32);
        extern "C" int backtrace(void** array, int size);

        // #include <execinfo.h>
        // #include <mach-o/dyld.h>
        // #include <mach-o/getsect.h>
        // #include <mach/mach_init.h>
        // #include <mach/mach_vm.h>

        static b32 utf8_compare(utf8 const * a, utf8 const * b);
        static utf8 const * utf8_last(utf8 const * a);
        static u64 utf8_number_of_bytes(utf8 const * string);

        struct FILE;
        extern "C" FILE * popen(const char *, const char *);
        extern "C" int pclose(FILE *);
        extern "C" char * fgets(char *, int, FILE *);

        static void debug_stack() {
            utf8 path[1024];
            u32 size = sizeof(path);
            _NSGetExecutablePath(path, &size);

            u64 load_address = 0;
            {
                for (u32 index = 0; index < _dyld_image_count(); index += 1) {
                    if (utf8_compare(_dyld_get_image_name(index), path)) {
                        segment_command_64 const * command = getsegbyname("__TEXT");
                        load_address = (u64)_dyld_get_image_vmaddr_slide(index) + command->vmaddr;
                    }
                }
            }

            void * frames[64];
            i32 number_of_frames = backtrace(frames, 64);
            for (i32 index = 0; index < number_of_frames; index += 1) {
                utf8 * command = format("atos -l {x} -o {} {x}", load_address, path, frames[index]);

                if (FILE * pipe = popen(command, "r")) {
                    utf8 output[4096];
                    while (fgets(output, sizeof(output), pipe) != NULL) {
                        if (utf8_compare(utf8_last(output), "\n")) {
                            print("\u001b[31m" "{3}   {016x}   {*}" "\u001b[0m" "\n", index, frames[index], utf8_number_of_bytes(output) - 1, output);
                        } else {
                            print("\u001b[31m" "{3}   {016x}   {}" "\u001b[0m" "\n", index, frames[index], output);
                        }
                    }

                    pclose(pipe);
                }
            }
        }
    #else
        // TODO
        #define debug_stack()
    #endif
#else
    #define debug_stack()
#endif

#define ASSERT(assertion) do {\
    if (!(assertion)) {\
        print("\u001b[31m" "{}:{} ASSERT: {}" "\u001b[0m" "\n", __FILE__, __LINE__, #assertion);\
        \
        debug_stack();\
        \
        __builtin_debugtrap();\
        __builtin_unreachable();\
    }\
} while (0)

#define ASSERT_NOT_REACHED() do {\
        print("\u001b[31m" "{}:{} ASSERT_NOT_REACHED" "\u001b[0m" "\n", __FILE__, __LINE__);\
        \
        debug_stack();\
        \
        __builtin_debugtrap();\
        __builtin_unreachable();\
    } while (0)

#define ASSERT_NOT_IMPLEMENTED() do {\
        print("\u001b[31m" "{}:{} ASSERT_NOT_IMPLEMENTED" "\u001b[0m" "\n", __FILE__, __LINE__);\
        \
        debug_stack();\
        \
        __builtin_debugtrap();\
        __builtin_unreachable();\
    } while (0)
