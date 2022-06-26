b32 link(Context & context, utf8 const * architecture, utf8 const * entry, utf8 const * output_file, utf8 const * input_file, utf8 ** arguments, u64 arguments_count) {
    #if defined(__APPLE__)
        if (utf8_compare(architecture, "x64")) {
            architecture = "x86_64";
        }

        utf8 const * arguments_string = "";
        for (u64 arguments_index = 0; arguments_index < arguments_count; arguments_index += 1) {
            arguments_string = format("{} {}", arguments_string, arguments[arguments_index]);
        }

        utf8 const * command = format("ld -arch {} -platform_version macos 13.0.0 13.1 -syslibroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -lSystem -e \"{}\" {} {} -o \"{}\"", architecture, entry, input_file, arguments_string, output_file);

        if (context.verbose) {
            print("Linking with '{}'\n", command);
        }

        if (FILE * pipe = popen(command, "r")) {
            utf8 output[4096];
            while (fgets(output, sizeof(output), pipe) != NULL) {
                print("{}", output);
            }

            int result = pclose(pipe);
            if (result == 0) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            return FALSE;
        }
    #elif (_WIN64)
        utf8 const * linker = "C:\\Program Files\\LLVM\\bin\\lld-link.exe";
        utf8 * command = format("{} /SUBSYSTEM:CONSOLE /ENTRY:{} {} {d( )} /OUT:\"{}\"", linker, entry, input_file, arguments, output_file);

        if (context.verbose) {
            print("Linking with 'lld-link {}'\n", command);
        }

        STARTUPINFOA startup_info;
        startup_info.cb = sizeof(STARTUPINFOA); // DWORD
        startup_info.lpReserved = NULL; // LPSTR
        startup_info.lpDesktop = NULL; // LPSTR
        startup_info.lpTitle = NULL; // LPSTR
        startup_info.dwX = 0; // DWORD
        startup_info.dwY = 0; // DWORD
        startup_info.dwXSize = 0; // DWORD
        startup_info.dwYSize = 0; // DWORD
        startup_info.dwXCountChars = 0; // DWORD
        startup_info.dwYCountChars = 0; // DWORD
        startup_info.dwFillAttribute = 0; // DWORD
        startup_info.dwFlags = 0; // DWORD
        startup_info.wShowWindow = 0; // WORD
        startup_info.cbReserved2 = 0; // WORD
        startup_info.lpReserved2 = NULL; // LPBYTE
        startup_info.hStdInput = 0; // HANDLE
        startup_info.hStdOutput = 0; // HANDLE
        startup_info.hStdError = 0; // HANDLE

        PROCESS_INFORMATION process_information;

        BOOL result = CreateProcessA(NULL, command, NULL, NULL, 0, 0, NULL, NULL, &startup_info, &process_information);

        return FALSE;
    #endif
}
