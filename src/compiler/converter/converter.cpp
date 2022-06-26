b32 convert(Context & context, Array<Symbol> const & exports, Array<Register> const & registers, Code const & input, Object_Environment & output_environment, Code & output, Target target) {
    ASSERT(utf8_compare(input.target, "intermediate"));

    if (target == TARGET_ARM64) {
        return convert_arm64(exports, registers, input, output_environment, output);
    } else if (target == TARGET_X64) {
        return convert_x64(exports, registers, input, output_environment, output);
    } else {
        // TODO: target names print here!
        report(context, "unknown target architecture '" "\u001b[1m" "{}" "\u001b[0m" "', valid architectures are '" "\u001b[1m" "x64" "\u001b[0m" "' and '" "\u001b[1m" "arm64" "\u001b[0m" "'", target_names[target]);
        return FALSE;
    }
}
