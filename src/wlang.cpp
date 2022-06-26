#include "lib/windows.hpp"
#include "lib/types.hpp"
#include "lib/assert.hpp"
#include "lib/memory.hpp"
#include "lib/print.hpp"
#include "lib/array.hpp"
#include "lib/push.hpp"
#include "lib/unicode.hpp"
#include "lib/file.hpp"
#include "lib/time.hpp"
#include "lib/buffer.hpp"

#include "compiler/target.hpp"
#include "compiler/types/address.hpp"
#include "compiler/types/type.hpp"
#include "compiler/types/register.hpp"
#include "compiler/types/location.hpp"
#include "compiler/types/token.hpp"
#include "compiler/types/expression.hpp"
#include "compiler/types/symbol.cpp"
#include "compiler/types/instruction.hpp"
#include "compiler/types/relocation.hpp"
#include "compiler/types/object_symbol.hpp"
#include "compiler/types/code.hpp"
#include "compiler/types/unit.hpp"
#include "compiler/types/context.hpp"
#include "compiler/lexer.cpp"
#include "compiler/parse.cpp"
#include "compiler/resolve.cpp"
#include "compiler/dependency.cpp"
#include "compiler/generator.cpp"
#include "compiler/disassembler.cpp"
#include "compiler/converter/arm64.cpp"
#include "compiler/converter/converter_arm64.cpp"
#include "compiler/converter/x64.cpp"
#include "compiler/converter/converter_x64.cpp"
#include "compiler/converter/converter.cpp"
#include "compiler/output/merge.cpp"
#include "compiler/output/coff.cpp"
#include "compiler/output/macho.cpp"
#include "linker/linker.cpp"
#include "compiler/compiler.cpp"

void print_help() {
    print("Usage: wlang [options...] [input]\n");
    print("\n");
    print("Options:\n");
    print("    " "\u001b[1m" "-h" "\u001b[0m" ", " "\u001b[1m" "--help" "\u001b[0m" "               Print this help message.\n");
    print("    " "\u001b[1m" "  " "\u001b[0m" "  " "\u001b[1m" "--version" "\u001b[0m" "            Print version message.\n");
    print("    " "\u001b[1m" "-v" "\u001b[0m" ", " "\u001b[1m" "--verbose" "\u001b[0m" "            Print verbose output.\n");
    print("    " "\u001b[1m" "-q" "\u001b[0m" ", " "\u001b[1m" "--quiet" "\u001b[0m" "              Print no output.\n");
    print("\n");
    print("    " "\u001b[1m" "-o [file]" "\u001b[0m" "                Write output to [file].\n");
    print("    " "\u001b[1m" "-O [directory]" "\u001b[0m" "           Write output to [directory].\n");
    print("\n");
    print("    " "\u001b[1m" "-t [target]" "\u001b[0m" "              Build for [target], where [target] is ");

    for (u64 targets_index = 0; targets_index < TARGETS_COUNT; targets_index += 1) {
        if (targets_index == TARGETS_COUNT - 1) {
            print(" or ");
        } else if (targets_index > 0) {
            print(", ");
        }

        print("'\u001b[1m{}\u001b[0m'", target_names[targets_index]);
    }

    print(".\n");
    print("    " "\u001b[1m" "-l [argument]" "\u001b[0m" "            Pass [argument] to the linker.\n");
}

void print_version() {
    print("wlang\n");
}

b32 wlang(utf8 ** arguments, u64 arguments_count) {
    struct {
        struct {
            b32 value = FALSE;
            utf8 * argument = NULL;
        } verbose;

        struct {
            b32 value = FALSE;
            utf8 * argument = NULL;
        } quiet;

        struct {
            b32 value = FALSE;
            utf8 * argument = NULL;
        } profile;

        struct {
            struct {
                utf8 * value = NULL;
                utf8 * argument = NULL;
            } file;
        } input;

        struct {
            struct {
                utf8 * value = NULL;
                utf8 * argument = NULL;
            } file;

            struct {
                utf8 * value = NULL;
                utf8 * argument = NULL;
            } directory;
        } output;

        struct {
            #if defined(__aarch64__)
                Target value = TARGET_ARM64;
            #elif defined(__x86_64__)
                Target value = TARGET_X64;
            #else
                #error "Unsupported architecture."
            #endif
            utf8 * argument = NULL;
        } target;

        struct {
            utf8 ** arguments = NULL;
            u64 arguments_capacity = 0;
            u64 arguments_count = 0;
        } linker;
    } flags;

    if (arguments_count > 0) {
        for (u32 arguments_index = 0; arguments_index < arguments_count; arguments_index += 1) {
            utf8 * argument = arguments[arguments_index];

            if (utf8_compare(argument, "-h") || utf8_compare(argument, "--help")) {
                print_help();
                return TRUE;
            } else if (utf8_compare(argument, "--version")) {
                print_version();
                return TRUE;
            } else if (utf8_compare(argument, "-v") || utf8_compare(argument, "--verbose")) {
                flags.verbose.value = TRUE;
                flags.verbose.argument = argument;
            } else if (utf8_compare(argument, "-q") || utf8_compare(argument, "--quiet")) {
                flags.quiet.value = TRUE;
                flags.quiet.argument = argument;
            } else if (utf8_compare(argument, "-p") || utf8_compare(argument, "--profile")) {
                flags.profile.value = TRUE;
                flags.profile.argument = argument;
            } else if (utf8_compare(argument, "-o")) {
                if (arguments_index + 1 < arguments_count) {
                    utf8 * value = arguments[arguments_index + 1];

                    if (flags.output.file.argument != NULL) {
                        report("duplicate argument '" "\u001b[1m" "{} {}" "\u001b[0m" "', previously specified with '" "\u001b[1m" "{}" "\u001b[0m" "'", argument, value, flags.output.file.argument);
                        return FALSE;
                    }

                    flags.output.file.value = value;
                    flags.output.file.argument = format("{} {}", argument, value);

                    arguments_index += 1;
                } else {
                    report("missing value for argument '" "\u001b[1m" "{}" "\u001b[0m" "'", argument);
                    return FALSE;
                }
            } else if (utf8_compare(argument, "-O")) {
                if (arguments_index + 1 < arguments_count) {
                    utf8 * value = arguments[arguments_index + 1];

                    if (flags.output.directory.argument != NULL) {
                        report("duplicate argument '" "\u001b[1m" "{} {}" "\u001b[0m" "', previously specified with '" "\u001b[1m" "{}" "\u001b[0m" "'", argument, value, flags.output.directory.argument);
                        return FALSE;
                    }

                    flags.output.directory.value = value;
                    flags.output.directory.argument = format("{} {}", argument, value);

                    arguments_index += 1;
                } else {
                    report("missing value for argument '" "\u001b[1m" "{}" "\u001b[0m" "'", argument);
                    return FALSE;
                }
            } else if (utf8_compare(argument, "-t")) {
                if (arguments_index + 1 < arguments_count) {
                    utf8 * value = arguments[arguments_index + 1];

                    if (flags.target.argument != NULL) {
                        report("duplicate argument '" "\u001b[1m" "{} {}" "\u001b[0m" "', previously specified with '" "\u001b[1m" "{}" "\u001b[0m" "'", argument, value, flags.target.argument);
                        return FALSE;
                    }

                    // TODO: This loop is wonky since we have to use int instead of Target as the iterator :(
                    for (int target = 0; target < TARGETS_COUNT; target += 1) {
                        if (utf8_compare(value, target_names[target])) {
                            flags.target.value = (Target)target;
                            flags.target.argument = format("{} {}", argument, value);

                            break;
                        }
                    }

                    if (flags.target.argument == NULL) {
                        // TODO: Print valid values
                        report("invalid value '" "\u001b[1m" "{}" "\u001b[0m" "' for argument '" "\u001b[1m" "{}" "\u001b[0m" "'", value, argument);
                        return FALSE;
                    }

                    arguments_index += 1;
                } else {
                    report("missing value for argument '" "\u001b[1m" "{}" "\u001b[0m" "'", argument);
                    return FALSE;
                }
            } else if (utf8_compare(argument, "-l")) {
                if (arguments_index + 1 < arguments_count) {
                    utf8 * value = arguments[arguments_index + 1];
                    push(flags.linker.arguments, flags.linker.arguments_capacity, flags.linker.arguments_count, value);

                    arguments_index += 1;
                } else {
                    report("missing value for argument '" "\u001b[1m" "{}" "\u001b[0m" "'", argument);
                    return FALSE;
                }
            } else {
                if (flags.input.file.value != NULL) {
                    report("duplicate input '" "\u001b[1m" "{}" "\u001b[0m" "', previously specified with '" "\u001b[1m" "{}" "\u001b[0m" "'", argument, flags.input.file.argument);
                    return FALSE;
                }

                flags.input.file.value = argument;
                flags.input.file.argument = argument;
            }
        }
    } else {
        print_help();
        return TRUE;
    }

    if (flags.input.file.value == NULL) {
        print_help();
        return FALSE;
    }

    Context context;
    context.verbose = flags.verbose.value;
    context.quiet = flags.quiet.value;
    context.profile = flags.profile.value;

    context.target = flags.target.value;

    context.input_file_name = flags.input.file.value;

    context.output_directory_name = flags.output.directory.value;

    if (flags.output.file.value != NULL) {
        context.output_file_name = flags.output.file.value;
    } else {
        // TODO: Do we need the format here?
        context.output_file_name = format("{}", get_file_name_without_file_extension(flags.input.file.value));
    }

    context.linker_arguments = flags.linker.arguments;
    context.linker_arguments_capacity = flags.linker.arguments_capacity;
    context.linker_arguments_count = flags.linker.arguments_count;

    f64 start_time = get_time();

    compile(context);

    if (context.errors.number_of_elements > 0) {
        print("{} error{} generated.\n", context.errors.number_of_elements, context.errors.number_of_elements > 1 ? "s" : "");
        return FALSE;
    }

    f64 end_time = get_time();
    f64 elapsed_time = end_time - start_time;

    if (flags.profile.value) {
        print("Compilation took {.4} s\n", elapsed_time);

        f64 time_parsing = 0.0;
        f64 time_finding_dependencies = 0.0;
        f64 time_waiting_for_dependencies = 0.0;
        f64 time_resolving = 0.0;
        f64 time_generating = 0.0;
        f64 time_converting = 0.0;

        for (Unit const & unit : context.units) {
            // print("  Unit '{}':\n", unit.label);
            // print("    Parsing: {.4} s\n", unit.time_per_stage[STAGE_PARSE - STAGE_FIRST_PROFILED]);
            time_parsing += unit.time_per_stage[STAGE_PARSE - STAGE_FIRST_PROFILED];
            // print("    Finding dependencies: {.4} s\n", unit.time_per_stage[STAGE_FIND_DEPENDENCIES - STAGE_FIRST_PROFILED]);
            time_finding_dependencies += unit.time_per_stage[STAGE_FIND_DEPENDENCIES - STAGE_FIRST_PROFILED];
            // print("    Waiting for dependencies: {.4} s\n", unit.time_per_stage[STAGE_WAIT_FOR_DEPENDENCIES - STAGE_FIRST_PROFILED]);
            time_waiting_for_dependencies += unit.time_per_stage[STAGE_WAIT_FOR_DEPENDENCIES - STAGE_FIRST_PROFILED];
            // print("    Resolving: {.4} s\n", unit.time_per_stage[STAGE_RESOLVE - STAGE_FIRST_PROFILED]);
            time_resolving += unit.time_per_stage[STAGE_RESOLVE - STAGE_FIRST_PROFILED];
            // print("    Generating: {.4} s\n", unit.time_per_stage[STAGE_GENERATE - STAGE_FIRST_PROFILED]);
            time_generating += unit.time_per_stage[STAGE_GENERATE - STAGE_FIRST_PROFILED];
            // print("    Converting: {.4} s\n", unit.time_per_stage[STAGE_CONVERT - STAGE_FIRST_PROFILED]);
            time_converting += unit.time_per_stage[STAGE_CONVERT - STAGE_FIRST_PROFILED];
        }

        print("  Total:\n");
        print("    Parsing: {.4} s\n", time_parsing);
        print("    Finding dependencies: {.4} s\n", time_finding_dependencies);
        print("    Waiting for dependencies: {.4} s\n", time_waiting_for_dependencies);
        print("    Resolving: {.4} s\n", time_resolving);
        print("    Generating: {.4} s\n", time_generating);
        print("    Converting: {.4} s\n", time_converting);
        print("    Merging: {.4} s\n", context.time_merging);
        print("    Writing objects: {.4} s\n", context.time_writing_object);
        print("    Linking: {.4} s\n", context.time_linking);
    }

    return TRUE;
}

int main(int argc, char ** argv) {
    utf8 ** arguments = (utf8 **)(argv + 1);
    u64 arguments_count = (u32)(argc - 1);

    b32 success = wlang(arguments, arguments_count);

    if (success) {
        return 0;
    } else {
        return 1;
    }
}
