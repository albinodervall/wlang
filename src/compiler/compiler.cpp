b32 compile(Context & context) {
    if (file_exists(context.input_file_name)) {
        // TODO: Error handling!
        Get_File_Result result = get_file(context.input_file_name, TRUE);
        utf8 const * source = (utf8 const *)result.bytes;

        push(context.units, Unit{
            .stage = STAGE_PARSE,
            .previous_stage = STAGE_PENDING,

            .label = context.input_file_name,
            .source = source
        });
    } else {
        report(context, ERROR_NO_SUCH_IMPORT, "no such file '" "\u001b[1m" "{}" "\u001b[0m" "'", context.input_file_name);
        return FALSE;
    }

    //
    Resolver resolver{
        .context = context,
        .root = make_root(),
        .tail = NULL
    };

    resolver.tail = resolver.root;
    //

    u32 number_of_progressed_units_last_step = 0;

    b32 running = TRUE;
    while (running) {
        if (context.verbose) {
            print("=============================================\n");
        }

        u32 number_of_completed_units = 0;
        u32 number_of_progressed_units = 0;

        for (u64 units_index = 0; units_index < context.units.number_of_elements; units_index += 1) {
            Unit & unit = context.units[units_index];

            unit.previous_stage = unit.stage;

            if (context.verbose) {
                if (unit.stage != STAGE_FAILED && unit.stage != STAGE_SUCCEEDED && unit.stage != STAGE_DISCARDED) {
                    print("Unit '{}' in stage '{}'\n", unit.label, stage_names[unit.stage]);
                }
            }

            f64 start_time = get_time();

            switch (unit.stage) {
                case STAGE_PENDING: {
                } break;

                case STAGE_DISCARDED:
                case STAGE_SUCCEEDED:
                case STAGE_FAILED: {
                    number_of_completed_units += 1;
                } break;

                case STAGE_PARSE: {
                    Parse_Result parse_result = parse(context, unit.label, unit.source);
                    if (parse_result.success) {
                        for (Expression * expression : parse_result.expressions) {
                            if (expression->type == EXPRESSION_IMPORT) {
                                utf8 const * import_file_name = expression->expression->string;

                                utf8 const * import_directory_name = get_directory_name(unit.label);
                                if (import_directory_name != NULL) {
                                    import_file_name = format("{}/{}", import_directory_name, import_file_name);
                                }

                                if (file_exists(import_file_name)) {
                                    Get_File_Result result = get_file(import_file_name, TRUE);
                                    utf8 const * source = (utf8 const *)result.bytes;

                                    push(context.units, Unit{
                                        .stage = STAGE_PARSE,
                                        // TODO: The label here should really be the original import file name, not the compounded one, I think we need to split the label into two parts!
                                        .label = import_file_name,
                                        .source = source
                                    });
                                } else {
                                    report(context, ERROR_NO_SUCH_IMPORT, "no such file '" "\u001b[1m" "{}" "\u001b[0m" "'", import_file_name);
                                }
                            } else if (expression->type == EXPRESSION_LINKER) {
                                utf8 * argument = (utf8 *)expression->expression->string; // TODO: Casting away const here!
                                push(context.linker_arguments, context.linker_arguments_capacity, context.linker_arguments_count, argument);
                            } else {
                                ASSERT(expression->flags & EXPRESSION_FLAGS_DECLARATION);
                                Expression_Declaration * expression_declaration = (Expression_Declaration *)expression;

                                push(context.units, Unit{
                                    .stage = STAGE_FIND_DEPENDENCIES,
                                    .label = format("{}#{}", unit.label, expression_declaration->declared_name),
                                    .source = unit.source,
                                    .tree = expression
                                });
                            }
                        }

                        unit.stage = STAGE_DISCARDED;
                    } else {
                        unit.stage = STAGE_FAILED;
                    }
                } break;

                case STAGE_FIND_DEPENDENCIES: {
                    if (find_dependencies(unit)) {
                        if (context.verbose) {
                            if (unit.dependencies.number_of_elements > 0) {
                                print("Dependencies for unit '{}':\n", unit.label);
                                for (Dependency & dependency : unit.dependencies) {
                                    print("  {} ({})\n", dependency.label, dependency.fulfilled ? "fulfilled" : "not fulfilled");
                                }
                            }
                        }

                        if (unit.dependencies.number_of_elements > 0) {
                            if (are_all_dependencies_fulfilled(unit, resolver)) {
                                unit.stage = STAGE_RESOLVE;
                            } else {
                                unit.stage = STAGE_WAIT_FOR_DEPENDENCIES;
                            }
                        } else {
                            unit.stage = STAGE_RESOLVE;
                        }
                    } else {
                        unit.stage = STAGE_FAILED;
                    }
                } break;

                case STAGE_WAIT_FOR_DEPENDENCIES: {
                    if (are_all_dependencies_fulfilled(unit, resolver)) {
                        unit.stage = STAGE_RESOLVE;
                    } else {
                        if (context.verbose) {
                            for (Dependency const & dependency : unit.dependencies) {
                                if (!dependency.fulfilled) {
                                    print("Unit '{}' waiting for dependency '{}' ({})\n", unit.label, dependency.label, dependency.fulfilled ? "fulfilled" : "not fulfilled");
                                }
                            }
                        }

                        if (number_of_progressed_units_last_step == 0) {
                            // TODO: I think it's a bit weird to have this here...
                            for (Dependency const & dependency : unit.dependencies) {
                                if (!dependency.fulfilled) {
                                    report(context, ERROR_UNDEFINED_SYMBOL, dependency.usage, "undefined identifier '{}'", dependency.label);
                                }
                            }

                            unit.stage = STAGE_FAILED;
                        }
                    }
                } break;

                case STAGE_RESOLVE: {
                    if (resolve(resolver, unit.tree)) {
                        unit.stage = STAGE_GENERATE;
                    } else {
                        unit.stage = STAGE_FAILED;
                    }
                } break;

                case STAGE_GENERATE: {
                    if (generate(unit)) {
                        if (context.verbose) {
                            // TODO: There should be a better way to determine if a unit has code...
                            if (unit.intermediate_code.buffer.number_of_bytes > 0) {
                                print("Disassembly for unit '{}' ({}):\n", unit.label, unit.intermediate_code.target);
                                print("{}", disassemble(unit.label, unit.registers, unit.intermediate_code));
                            }
                        }

                        unit.stage = STAGE_CONVERT;
                    } else {
                        unit.stage = STAGE_FAILED;
                    }
                } break;

                case STAGE_CONVERT: {
                    if (convert(context, unit.exports, unit.registers, unit.intermediate_code, unit.output_environment, unit.machine_code, context.target)) {
                        if (!context.quiet) {
                            // TODO: Print machine code disassembly!
                        }

                        unit.stage = STAGE_SUCCEEDED;
                    } else {
                        unit.stage = STAGE_FAILED;
                    }
                } break;
            }

            f64 end_time = get_time();
            f64 elapsed_time = end_time - start_time;

            if (unit.stage != unit.previous_stage) {
                if (context.verbose) {
                    if (unit.stage == STAGE_DISCARDED) {
                        print("Unit '{}' discarded in stage '{}'\n", unit.label, stage_names[unit.previous_stage]);
                    } else if (unit.stage == STAGE_FAILED) {
                        print("Unit '{}' failed in stage '{}'\n", unit.label, stage_names[unit.previous_stage]);
                    } else if (unit.stage == STAGE_SUCCEEDED) {
                        print("Unit '{}' succeeded in stage '{}'\n", unit.label, stage_names[unit.previous_stage]);
                    }
                }

                number_of_progressed_units += 1;

                if (unit.previous_stage >= STAGE_FIRST_PROFILED && unit.previous_stage <= STAGE_LAST_PROFILED) {
                    unit.time_per_stage[unit.previous_stage - STAGE_FIRST_PROFILED] += elapsed_time;
                }
            } else {
                if (unit.stage >= STAGE_FIRST_PROFILED && unit.stage <= STAGE_LAST_PROFILED) {
                    unit.time_per_stage[unit.stage - STAGE_FIRST_PROFILED] += elapsed_time;
                }
            }
        }

        number_of_progressed_units_last_step = number_of_progressed_units;

        if (number_of_completed_units == context.units.number_of_elements) {
            running = FALSE;
        }
    }

    for (Unit const & unit : context.units) {
        if (unit.stage == STAGE_FAILED) {
            ASSERT(context.errors.number_of_elements > 0);
        }
    }

    if (context.errors.number_of_elements == 0) {
        f64 merge_start_time = get_time();
        Merge_Result result = merge(context);
        f64 merge_end_time = get_time();
        context.time_merging = merge_end_time - merge_start_time;

        utf8 const * entry = NULL;

        for (Object_Symbol const & symbol : result.environment.symbols) {
            if (symbol.flags & OBJECT_SYMBOL_ENTRY_FUNCTION) {
                if (entry != NULL) {
                    report(context, ERROR_MULTIPLE_ENTRY, "program has multiple entry points");
                    return FALSE;
                }

                #if defined(_WIN64)
                    entry = format("{}", symbol.name);
                #else
                    entry = format("_{}", symbol.name);
                #endif
            }
        }

        if (entry == NULL) {
            report(context, ERROR_NO_ENTRY, "program has no entry points");
            return FALSE;
        }

        if (result.code.buffer.number_of_bytes > 0) {
            // TODO: Allow cross-compilation!
            utf8 const * output_path_name = context.output_file_name;
            if (context.output_directory_name) {
                output_path_name = format("{}/{}", context.output_directory_name, context.output_file_name);
            }

            f64 object_start_time = get_time();

            #if defined(_WIN64)
                write_coff(context, format("{}.obj", output_path_name), result.environment, result.code);
            #else
                write_macho(context, format("{}.o", output_path_name), result.environment, result.code);
            #endif

            f64 object_end_time = get_time();
            context.time_writing_object = object_end_time - object_start_time;

            f64 link_start_time = get_time();

            #if defined(_WIN64)
                b32 linked = link(context, result.code.target, entry, output_path_name, format("{}.obj", output_path_name), context.linker_arguments, context.linker_arguments_count);
            #else
                b32 linked = link(context, result.code.target, entry, output_path_name, format("{}.o", output_path_name), context.linker_arguments, context.linker_arguments_count);
            #endif

            f64 link_end_time = get_time();
            context.time_linking = link_end_time - link_start_time;

            return linked;
        } else {
            report(context, "No code generated.");
            return FALSE;
        }
    }

    return TRUE;
}
