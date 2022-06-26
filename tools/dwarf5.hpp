enum DW_UT {
    DW_UT_compile = 0x01,
    DW_UT_type = 0x02,
    DW_UT_partial = 0x03,
    DW_UT_skeleton = 0x04,
    DW_UT_split_compile = 0x05,
    DW_UT_split_type = 0x06,
    DW_UT_lo_user = 0x80,
    DW_UT_hi_user = 0xff,
};

utf8 const * dw_ut_to_string(u64 dw_ut) {
    switch (dw_ut) {
        case DW_UT_compile: { return "DW_UT_compile"; } break;
        case DW_UT_type: { return "DW_UT_type"; } break;
        case DW_UT_partial: { return "DW_UT_partial"; } break;
        case DW_UT_skeleton: { return "DW_UT_skeleton"; } break;
        case DW_UT_split_compile: { return "DW_UT_split_compile"; } break;
        case DW_UT_split_type: { return "DW_UT_split_type"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

enum DW_TAG {
    DW_TAG_array_type = 0x01,
    DW_TAG_class_type = 0x02,
    DW_TAG_entry_point = 0x03,
    DW_TAG_enumeration_type = 0x04,
    DW_TAG_formal_parameter = 0x05,
    DW_TAG_imported_declaration = 0x08,
    DW_TAG_label = 0x0a,
    DW_TAG_lexical_block = 0x0b,
    DW_TAG_member = 0x0d,
    DW_TAG_pointer_type = 0x0f,
    DW_TAG_reference_type = 0x10,
    DW_TAG_compile_unit = 0x11,
    DW_TAG_string_type = 0x12,
    DW_TAG_structure_type = 0x13,
    DW_TAG_subroutine_type = 0x15,
    DW_TAG_typedef = 0x16,
    DW_TAG_union_type = 0x17,
    DW_TAG_unspecified_parameters = 0x18,
    DW_TAG_variant = 0x19,
    DW_TAG_common_block = 0x1a,
    DW_TAG_common_inclusion = 0x1b,
    DW_TAG_inheritance = 0x1c,
    DW_TAG_inlined_subroutine = 0x1d,
    DW_TAG_module = 0x1e,
    DW_TAG_ptr_to_member_type = 0x1f,
    DW_TAG_set_type = 0x20,
    DW_TAG_subrange_type = 0x21,
    DW_TAG_with_stmt = 0x22,
    DW_TAG_access_declaration = 0x23,
    DW_TAG_base_type = 0x24,
    DW_TAG_catch_block = 0x25,
    DW_TAG_const_type = 0x26,
    DW_TAG_constant = 0x27,
    DW_TAG_enumerator = 0x28,
    DW_TAG_file_type = 0x29,
    DW_TAG_friend = 0x2a,
    DW_TAG_namelist = 0x2b,
    DW_TAG_namelist_item = 0x2c,
    DW_TAG_packed_type = 0x2d,
    DW_TAG_subprogram = 0x2e,
    DW_TAG_template_type_parameter = 0x2f,
    DW_TAG_template_value_parameter = 0x30,
    DW_TAG_thrown_type = 0x31,
    DW_TAG_try_block = 0x32,
    DW_TAG_variant_part = 0x33,
    DW_TAG_variable = 0x34,
    DW_TAG_volatile_type = 0x35,
    DW_TAG_dwarf_procedure = 0x36,
    DW_TAG_restrict_type = 0x37,
    DW_TAG_interface_type = 0x38,
    DW_TAG_namespace = 0x39,
    DW_TAG_imported_module = 0x3a,
    DW_TAG_unspecified_type = 0x3b,
    DW_TAG_partial_unit = 0x3c,
    DW_TAG_imported_unit = 0x3d,
    DW_TAG_condition = 0x3f,
    DW_TAG_shared_type = 0x40,
    DW_TAG_type_unit = 0x41,
    DW_TAG_rvalue_reference_type = 0x42,
    DW_TAG_template_alias = 0x43,
    DW_TAG_coarray_type = 0x44,
    DW_TAG_generic_subrange = 0x45,
    DW_TAG_dynamic_type = 0x46,
    DW_TAG_atomic_type = 0x47,
    DW_TAG_call_site = 0x48,
    DW_TAG_call_site_parameter = 0x49,
    DW_TAG_skeleton_unit = 0x4a,
    DW_TAG_immutable_type = 0x4b,
};

utf8 const * dw_tag_to_string(u64 dw_tag) {
    switch (dw_tag) {
        case DW_TAG_array_type: { return "DW_TAG_array_type"; } break;
        case DW_TAG_class_type: { return "DW_TAG_class_type"; } break;
        case DW_TAG_entry_point: { return "DW_TAG_entry_point"; } break;
        case DW_TAG_enumeration_type: { return "DW_TAG_enumeration_type"; } break;
        case DW_TAG_formal_parameter: { return "DW_TAG_formal_parameter"; } break;
        case DW_TAG_imported_declaration: { return "DW_TAG_imported_declaration"; } break;
        case DW_TAG_label: { return "DW_TAG_label"; } break;
        case DW_TAG_lexical_block: { return "DW_TAG_lexical_block"; } break;
        case DW_TAG_member: { return "DW_TAG_member"; } break;
        case DW_TAG_pointer_type: { return "DW_TAG_pointer_type"; } break;
        case DW_TAG_reference_type: { return "DW_TAG_reference_type"; } break;
        case DW_TAG_compile_unit: { return "DW_TAG_compile_unit"; } break;
        case DW_TAG_string_type: { return "DW_TAG_string_type"; } break;
        case DW_TAG_structure_type: { return "DW_TAG_structure_type"; } break;
        case DW_TAG_subroutine_type: { return "DW_TAG_subroutine_type"; } break;
        case DW_TAG_typedef: { return "DW_TAG_typedef"; } break;
        case DW_TAG_union_type: { return "DW_TAG_union_type"; } break;
        case DW_TAG_unspecified_parameters: { return "DW_TAG_unspecified_parameters"; } break;
        case DW_TAG_variant: { return "DW_TAG_variant"; } break;
        case DW_TAG_common_block: { return "DW_TAG_common_block"; } break;
        case DW_TAG_common_inclusion: { return "DW_TAG_common_inclusion"; } break;
        case DW_TAG_inheritance: { return "DW_TAG_inheritance"; } break;
        case DW_TAG_inlined_subroutine: { return "DW_TAG_inlined_subroutine"; } break;
        case DW_TAG_module: { return "DW_TAG_module"; } break;
        case DW_TAG_ptr_to_member_type: { return "DW_TAG_ptr_to_member_type"; } break;
        case DW_TAG_set_type: { return "DW_TAG_set_type"; } break;
        case DW_TAG_subrange_type: { return "DW_TAG_subrange_type"; } break;
        case DW_TAG_with_stmt: { return "DW_TAG_with_stmt"; } break;
        case DW_TAG_access_declaration: { return "DW_TAG_access_declaration"; } break;
        case DW_TAG_base_type: { return "DW_TAG_base_type"; } break;
        case DW_TAG_catch_block: { return "DW_TAG_catch_block"; } break;
        case DW_TAG_const_type: { return "DW_TAG_const_type"; } break;
        case DW_TAG_constant: { return "DW_TAG_constant"; } break;
        case DW_TAG_enumerator: { return "DW_TAG_enumerator"; } break;
        case DW_TAG_file_type: { return "DW_TAG_file_type"; } break;
        case DW_TAG_friend: { return "DW_TAG_friend"; } break;
        case DW_TAG_namelist: { return "DW_TAG_namelist"; } break;
        case DW_TAG_namelist_item: { return "DW_TAG_namelist_item"; } break;
        case DW_TAG_packed_type: { return "DW_TAG_packed_type"; } break;
        case DW_TAG_subprogram: { return "DW_TAG_subprogram"; } break;
        case DW_TAG_template_type_parameter: { return "DW_TAG_template_type_parameter"; } break;
        case DW_TAG_template_value_parameter: { return "DW_TAG_template_value_parameter"; } break;
        case DW_TAG_thrown_type: { return "DW_TAG_thrown_type"; } break;
        case DW_TAG_try_block: { return "DW_TAG_try_block"; } break;
        case DW_TAG_variant_part: { return "DW_TAG_variant_part"; } break;
        case DW_TAG_variable: { return "DW_TAG_variable"; } break;
        case DW_TAG_volatile_type: { return "DW_TAG_volatile_type"; } break;
        case DW_TAG_dwarf_procedure: { return "DW_TAG_dwarf_procedure"; } break;
        case DW_TAG_restrict_type: { return "DW_TAG_restrict_type"; } break;
        case DW_TAG_interface_type: { return "DW_TAG_interface_type"; } break;
        case DW_TAG_namespace: { return "DW_TAG_namespace"; } break;
        case DW_TAG_imported_module: { return "DW_TAG_imported_module"; } break;
        case DW_TAG_unspecified_type: { return "DW_TAG_unspecified_type"; } break;
        case DW_TAG_partial_unit: { return "DW_TAG_partial_unit"; } break;
        case DW_TAG_imported_unit: { return "DW_TAG_imported_unit"; } break;
        case DW_TAG_condition: { return "DW_TAG_condition"; } break;
        case DW_TAG_shared_type: { return "DW_TAG_shared_type"; } break;
        case DW_TAG_type_unit: { return "DW_TAG_type_unit"; } break;
        case DW_TAG_rvalue_reference_type: { return "DW_TAG_rvalue_reference_type"; } break;
        case DW_TAG_template_alias: { return "DW_TAG_template_alias"; } break;
        case DW_TAG_coarray_type: { return "DW_TAG_coarray_type"; } break;
        case DW_TAG_generic_subrange: { return "DW_TAG_generic_subrange"; } break;
        case DW_TAG_dynamic_type: { return "DW_TAG_dynamic_type"; } break;
        case DW_TAG_atomic_type: { return "DW_TAG_atomic_type"; } break;
        case DW_TAG_call_site: { return "DW_TAG_call_site"; } break;
        case DW_TAG_call_site_parameter: { return "DW_TAG_call_site_parameter"; } break;
        case DW_TAG_skeleton_unit: { return "DW_TAG_skeleton_unit"; } break;
        case DW_TAG_immutable_type: { return "DW_TAG_immutable_type"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

enum DW_AT {
    DW_AT_sibling = 0x01,
    DW_AT_location = 0x02,
    DW_AT_name = 0x03,
    DW_AT_ordering = 0x09,
    DW_AT_byte_size = 0x0b,
    DW_AT_bit_size = 0x0d,
    DW_AT_stmt_list = 0x10,
    DW_AT_low_pc = 0x11,
    DW_AT_high_pc = 0x12,
    DW_AT_language = 0x13,
    DW_AT_discr = 0x15,
    DW_AT_discr_value = 0x16,
    DW_AT_visibility = 0x17,
    DW_AT_import = 0x18,
    DW_AT_string_length = 0x19,
    DW_AT_common_reference = 0x1a,
    DW_AT_comp_dir = 0x1b,
    DW_AT_const_value = 0x1c,
    DW_AT_containing_type = 0x1d,
    DW_AT_default_value = 0x1e,
    DW_AT_inline = 0x20,
    DW_AT_is_optional = 0x21,
    DW_AT_lower_bound = 0x22,
    DW_AT_producer = 0x25,
    DW_AT_prototyped = 0x27,
    DW_AT_return_addr = 0x2a,
    DW_AT_start_scope = 0x2c,
    DW_AT_bit_stride = 0x2e,
    DW_AT_upper_bound = 0x2f,
    DW_AT_abstract_origin = 0x31,
    DW_AT_accessibility = 0x32,
    DW_AT_address_class = 0x33,
    DW_AT_artificial = 0x34,
    DW_AT_base_types = 0x35,
    DW_AT_calling_convention = 0x36,
    DW_AT_count = 0x37,
    DW_AT_data_member_location = 0x38,
    DW_AT_decl_column = 0x39,
    DW_AT_decl_file = 0x3a,
    DW_AT_decl_line = 0x3b,
    DW_AT_declaration = 0x3c,
    DW_AT_discr_list = 0x3d,
    DW_AT_encoding = 0x3e,
    DW_AT_external = 0x3f,
    DW_AT_frame_base = 0x40,
    DW_AT_friend = 0x41,
    DW_AT_identifier_case = 0x42,
    DW_AT_namelist_item = 0x44,
    DW_AT_priority = 0x45,
    DW_AT_segment = 0x46,
    DW_AT_specification = 0x47,
    DW_AT_static_link = 0x48,
    DW_AT_type = 0x49,
    DW_AT_use_location = 0x4a,
    DW_AT_variable_parameter = 0x4b,
    DW_AT_virtuality = 0x4c,
    DW_AT_vtable_elem_location = 0x4d,
    DW_AT_allocated = 0x4e,
    DW_AT_associated = 0x4f,
    DW_AT_data_location = 0x50,
    DW_AT_byte_stride = 0x51,
    DW_AT_entry_pc = 0x52,
    DW_AT_use_UTF8 = 0x53,
    DW_AT_extension = 0x54,
    DW_AT_ranges = 0x55,
    DW_AT_trampoline = 0x56,
    DW_AT_call_column = 0x57,
    DW_AT_call_file = 0x58,
    DW_AT_call_line = 0x59,
    DW_AT_description = 0x5a,
    DW_AT_binary_scale = 0x5b,
    DW_AT_decimal_scale = 0x5c,
    DW_AT_small = 0x5d,
    DW_AT_decimal_sign = 0x5e,
    DW_AT_digit_count = 0x5f,
    DW_AT_picture_string = 0x60,
    DW_AT_mutable = 0x61,
    DW_AT_threads_scaled = 0x62,
    DW_AT_explicit = 0x63,
    DW_AT_object_pointer = 0x64,
    DW_AT_endianity = 0x65,
    DW_AT_elemental = 0x66,
    DW_AT_pure = 0x67,
    DW_AT_recursive = 0x68,
    DW_AT_signature = 0x69,
    DW_AT_main_subprogram = 0x6a,
    DW_AT_data_bit_offset = 0x6b,
    DW_AT_const_expr = 0x6c,
    DW_AT_enum_class = 0x6d,
    DW_AT_linkage_name = 0x6e,
    DW_AT_string_length_bit_size = 0x6f,
    DW_AT_string_length_byte_size = 0x70,
    DW_AT_rank = 0x71,
    DW_AT_str_offsets_base = 0x72,
    DW_AT_addr_base = 0x73,
    DW_AT_rnglists_base = 0x74,
    DW_AT_dwo_name = 0x76,
    DW_AT_reference = 0x77,
    DW_AT_rvalue_reference = 0x78,
    DW_AT_macros = 0x79,
    DW_AT_call_all_calls = 0x7a,
    DW_AT_call_all_source_calls = 0x7b,
    DW_AT_call_all_tail_calls = 0x7c,
    DW_AT_call_return_pc = 0x7d,
    DW_AT_call_value = 0x7e,
    DW_AT_call_origin = 0x7f,
    DW_AT_call_parameter = 0x80,
    DW_AT_call_pc = 0x81,
    DW_AT_call_tail_call = 0x82,
    DW_AT_call_target = 0x83,
    DW_AT_call_target_clobbered = 0x84,
    DW_AT_call_data_location = 0x85,
    DW_AT_call_data_value = 0x86,
    DW_AT_noreturn = 0x87,
    DW_AT_alignment = 0x88,
    DW_AT_export_symbols = 0x89,
    DW_AT_deleted = 0x8a,
    DW_AT_defaulted = 0x8b,
    DW_AT_loclists_base = 0x8c,

    DW_AT_LLVM_sysroot = 0x3e02,
    DW_AT_APPLE_sdk = 0x3fef,
    DW_AT_APPLE_omit_frame_ptr = 0x3fe7,
};

utf8 const * dw_at_to_string(u64 dw_at) {
    switch (dw_at) {
        case DW_AT_sibling: { return "DW_AT_sibling"; } break;
        case DW_AT_location: { return "DW_AT_location"; } break;
        case DW_AT_name: { return "DW_AT_name"; } break;
        case DW_AT_ordering: { return "DW_AT_ordering"; } break;
        case DW_AT_byte_size: { return "DW_AT_byte_size"; } break;
        case DW_AT_bit_size: { return "DW_AT_bit_size"; } break;
        case DW_AT_stmt_list: { return "DW_AT_stmt_list"; } break;
        case DW_AT_low_pc: { return "DW_AT_low_pc"; } break;
        case DW_AT_high_pc: { return "DW_AT_high_pc"; } break;
        case DW_AT_language: { return "DW_AT_language"; } break;
        case DW_AT_discr: { return "DW_AT_discr"; } break;
        case DW_AT_discr_value: { return "DW_AT_discr_value"; } break;
        case DW_AT_visibility: { return "DW_AT_visibility"; } break;
        case DW_AT_import: { return "DW_AT_import"; } break;
        case DW_AT_string_length: { return "DW_AT_string_length"; } break;
        case DW_AT_common_reference: { return "DW_AT_common_reference"; } break;
        case DW_AT_comp_dir: { return "DW_AT_comp_dir"; } break;
        case DW_AT_const_value: { return "DW_AT_const_value"; } break;
        case DW_AT_containing_type: { return "DW_AT_containing_type"; } break;
        case DW_AT_default_value: { return "DW_AT_default_value"; } break;
        case DW_AT_inline: { return "DW_AT_inline"; } break;
        case DW_AT_is_optional: { return "DW_AT_is_optional"; } break;
        case DW_AT_lower_bound: { return "DW_AT_lower_bound"; } break;
        case DW_AT_producer: { return "DW_AT_producer"; } break;
        case DW_AT_prototyped: { return "DW_AT_prototyped"; } break;
        case DW_AT_return_addr: { return "DW_AT_return_addr"; } break;
        case DW_AT_start_scope: { return "DW_AT_start_scope"; } break;
        case DW_AT_bit_stride: { return "DW_AT_bit_stride"; } break;
        case DW_AT_upper_bound: { return "DW_AT_upper_bound"; } break;
        case DW_AT_abstract_origin: { return "DW_AT_abstract_origin"; } break;
        case DW_AT_accessibility: { return "DW_AT_accessibility"; } break;
        case DW_AT_address_class: { return "DW_AT_address_class"; } break;
        case DW_AT_artificial: { return "DW_AT_artificial"; } break;
        case DW_AT_base_types: { return "DW_AT_base_types"; } break;
        case DW_AT_calling_convention: { return "DW_AT_calling_convention"; } break;
        case DW_AT_count: { return "DW_AT_count"; } break;
        case DW_AT_data_member_location: { return "DW_AT_data_member_location"; } break;
        case DW_AT_decl_column: { return "DW_AT_decl_column"; } break;
        case DW_AT_decl_file: { return "DW_AT_decl_file"; } break;
        case DW_AT_decl_line: { return "DW_AT_decl_line"; } break;
        case DW_AT_declaration: { return "DW_AT_declaration"; } break;
        case DW_AT_discr_list: { return "DW_AT_discr_list"; } break;
        case DW_AT_encoding: { return "DW_AT_encoding"; } break;
        case DW_AT_external: { return "DW_AT_external"; } break;
        case DW_AT_frame_base: { return "DW_AT_frame_base"; } break;
        case DW_AT_friend: { return "DW_AT_friend"; } break;
        case DW_AT_identifier_case: { return "DW_AT_identifier_case"; } break;
        case DW_AT_namelist_item: { return "DW_AT_namelist_item"; } break;
        case DW_AT_priority: { return "DW_AT_priority"; } break;
        case DW_AT_segment: { return "DW_AT_segment"; } break;
        case DW_AT_specification: { return "DW_AT_specification"; } break;
        case DW_AT_static_link: { return "DW_AT_static_link"; } break;
        case DW_AT_type: { return "DW_AT_type"; } break;
        case DW_AT_use_location: { return "DW_AT_use_location"; } break;
        case DW_AT_variable_parameter: { return "DW_AT_variable_parameter"; } break;
        case DW_AT_virtuality: { return "DW_AT_virtuality"; } break;
        case DW_AT_vtable_elem_location: { return "DW_AT_vtable_elem_location"; } break;
        case DW_AT_allocated: { return "DW_AT_allocated"; } break;
        case DW_AT_associated: { return "DW_AT_associated"; } break;
        case DW_AT_data_location: { return "DW_AT_data_location"; } break;
        case DW_AT_byte_stride: { return "DW_AT_byte_stride"; } break;
        case DW_AT_entry_pc: { return "DW_AT_entry_pc"; } break;
        case DW_AT_use_UTF8: { return "DW_AT_use_UTF8"; } break;
        case DW_AT_extension: { return "DW_AT_extension"; } break;
        case DW_AT_ranges: { return "DW_AT_ranges"; } break;
        case DW_AT_trampoline: { return "DW_AT_trampoline"; } break;
        case DW_AT_call_column: { return "DW_AT_call_column"; } break;
        case DW_AT_call_file: { return "DW_AT_call_file"; } break;
        case DW_AT_call_line: { return "DW_AT_call_line"; } break;
        case DW_AT_description: { return "DW_AT_description"; } break;
        case DW_AT_binary_scale: { return "DW_AT_binary_scale"; } break;
        case DW_AT_decimal_scale: { return "DW_AT_decimal_scale"; } break;
        case DW_AT_small: { return "DW_AT_small"; } break;
        case DW_AT_decimal_sign: { return "DW_AT_decimal_sign"; } break;
        case DW_AT_digit_count: { return "DW_AT_digit_count"; } break;
        case DW_AT_picture_string: { return "DW_AT_picture_string"; } break;
        case DW_AT_mutable: { return "DW_AT_mutable"; } break;
        case DW_AT_threads_scaled: { return "DW_AT_threads_scaled"; } break;
        case DW_AT_explicit: { return "DW_AT_explicit"; } break;
        case DW_AT_object_pointer: { return "DW_AT_object_pointer"; } break;
        case DW_AT_endianity: { return "DW_AT_endianity"; } break;
        case DW_AT_elemental: { return "DW_AT_elemental"; } break;
        case DW_AT_pure: { return "DW_AT_pure"; } break;
        case DW_AT_recursive: { return "DW_AT_recursive"; } break;
        case DW_AT_signature: { return "DW_AT_signature"; } break;
        case DW_AT_main_subprogram: { return "DW_AT_main_subprogram"; } break;
        case DW_AT_data_bit_offset: { return "DW_AT_data_bit_offset"; } break;
        case DW_AT_const_expr: { return "DW_AT_const_expr"; } break;
        case DW_AT_enum_class: { return "DW_AT_enum_class"; } break;
        case DW_AT_linkage_name: { return "DW_AT_linkage_name"; } break;
        case DW_AT_string_length_bit_size: { return "DW_AT_string_length_bit_size"; } break;
        case DW_AT_string_length_byte_size: { return "DW_AT_string_length_byte_size"; } break;
        case DW_AT_rank: { return "DW_AT_rank"; } break;
        case DW_AT_str_offsets_base: { return "DW_AT_str_offsets_base"; } break;
        case DW_AT_addr_base: { return "DW_AT_addr_base"; } break;
        case DW_AT_rnglists_base: { return "DW_AT_rnglists_base"; } break;
        case DW_AT_dwo_name: { return "DW_AT_dwo_name"; } break;
        case DW_AT_reference: { return "DW_AT_reference"; } break;
        case DW_AT_rvalue_reference: { return "DW_AT_rvalue_reference"; } break;
        case DW_AT_macros: { return "DW_AT_macros"; } break;
        case DW_AT_call_all_calls: { return "DW_AT_call_all_calls"; } break;
        case DW_AT_call_all_source_calls: { return "DW_AT_call_all_source_calls"; } break;
        case DW_AT_call_all_tail_calls: { return "DW_AT_call_all_tail_calls"; } break;
        case DW_AT_call_return_pc: { return "DW_AT_call_return_pc"; } break;
        case DW_AT_call_value: { return "DW_AT_call_value"; } break;
        case DW_AT_call_origin: { return "DW_AT_call_origin"; } break;
        case DW_AT_call_parameter: { return "DW_AT_call_parameter"; } break;
        case DW_AT_call_pc: { return "DW_AT_call_pc"; } break;
        case DW_AT_call_tail_call: { return "DW_AT_call_tail_call"; } break;
        case DW_AT_call_target: { return "DW_AT_call_target"; } break;
        case DW_AT_call_target_clobbered: { return "DW_AT_call_target_clobbered"; } break;
        case DW_AT_call_data_location: { return "DW_AT_call_data_location"; } break;
        case DW_AT_call_data_value: { return "DW_AT_call_data_value"; } break;
        case DW_AT_noreturn: { return "DW_AT_noreturn"; } break;
        case DW_AT_alignment: { return "DW_AT_alignment"; } break;
        case DW_AT_export_symbols: { return "DW_AT_export_symbols"; } break;
        case DW_AT_deleted: { return "DW_AT_deleted"; } break;
        case DW_AT_defaulted: { return "DW_AT_defaulted"; } break;
        case DW_AT_loclists_base: { return "DW_AT_loclists_base"; } break;

        case DW_AT_LLVM_sysroot: { return "DW_AT_LLVM_sysroot"; } break;
        case DW_AT_APPLE_sdk: { return "DW_AT_APPLE_sdk"; } break;
        case DW_AT_APPLE_omit_frame_ptr: { return "DW_AT_APPLE_omit_frame_ptr"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

utf8 const * dw_at_classes_to_string(u64 dw_at) {
    switch (dw_at) {
        case DW_AT_sibling: { return "reference"; } break;
        case DW_AT_location: { return "exprloc, loclist"; } break;
        case DW_AT_name: { return "string"; } break;
        case DW_AT_ordering: { return "constant"; } break;
        case DW_AT_byte_size: { return "constant, exprloc, reference"; } break;
        case DW_AT_bit_size: { return "constant, exprloc, reference"; } break;
        case DW_AT_stmt_list: { return "constant, exprloc, reference"; } break;
        case DW_AT_low_pc: { return "lineptr"; } break;
        case DW_AT_high_pc: { return "address"; } break;
        case DW_AT_language: { return "address, constant"; } break;
        case DW_AT_discr: { return "reference"; } break;
        case DW_AT_discr_value: { return "constant"; } break;
        case DW_AT_visibility: { return "constant"; } break;
        case DW_AT_import: { return "reference"; } break;
        case DW_AT_string_length: { return "exprloc, loclist, reference"; } break;
        case DW_AT_common_reference: { return "reference"; } break;
        case DW_AT_comp_dir: { return "string"; } break;
        case DW_AT_const_value: { return "block, constant, string"; } break;
        case DW_AT_containing_type: { return "reference"; } break;
        case DW_AT_default_value: { return "constant, reference, flag"; } break;
        case DW_AT_inline: { return "constant"; } break;
        case DW_AT_is_optional: { return "flag"; } break;
        case DW_AT_lower_bound: { return "constant, exprloc, reference"; } break;
        case DW_AT_producer: { return "string"; } break;
        case DW_AT_prototyped: { return "flag"; } break;
        case DW_AT_return_addr: { return "exprloc, loclist"; } break;
        case DW_AT_start_scope: { return "constant, rnglist"; } break;
        case DW_AT_bit_stride: { return "constant, exprloc, reference"; } break;
        case DW_AT_upper_bound: { return "constant, exprloc, reference"; } break;
        case DW_AT_abstract_origin: { return "reference"; } break;
        case DW_AT_accessibility: { return "constant"; } break;
        case DW_AT_address_class: { return "constant"; } break;
        case DW_AT_artificial: { return "flag"; } break;
        case DW_AT_base_types: { return "reference"; } break;
        case DW_AT_calling_convention: { return "constant"; } break;
        case DW_AT_count: { return "constant, exprloc, reference"; } break;
        case DW_AT_data_member_location: { return "constant, exprloc, loclist"; } break;
        case DW_AT_decl_column: { return "constant"; } break;
        case DW_AT_decl_file: { return "constant"; } break;
        case DW_AT_decl_line: { return "constant"; } break;
        case DW_AT_declaration: { return "flag"; } break;
        case DW_AT_discr_list: { return "block"; } break;
        case DW_AT_encoding: { return "constant"; } break;
        case DW_AT_external: { return "flag"; } break;
        case DW_AT_frame_base: { return "exprloc, loclist"; } break;
        case DW_AT_friend: { return "reference"; } break;
        case DW_AT_identifier_case: { return "constant"; } break;
        case DW_AT_namelist_item: { return "reference"; } break;
        case DW_AT_priority: { return "reference"; } break;
        case DW_AT_segment: { return "exprloc, loclist"; } break;
        case DW_AT_specification: { return "reference"; } break;
        case DW_AT_static_link: { return "exprloc, loclist"; } break;
        case DW_AT_type: { return "reference"; } break;
        case DW_AT_use_location: { return "exprloc, loclist"; } break;
        case DW_AT_variable_parameter: { return "flag"; } break;
        case DW_AT_virtuality: { return "constant"; } break;
        case DW_AT_vtable_elem_location: { return "exprloc, loclist"; } break;
        case DW_AT_allocated: { return "constant, exprloc, reference"; } break;
        case DW_AT_associated: { return "constant, exprloc, reference"; } break;
        case DW_AT_data_location: { return "exprloc"; } break;
        case DW_AT_byte_stride: { return "constant, exprloc, reference"; } break;
        case DW_AT_entry_pc: { return "address, constant"; } break;
        case DW_AT_use_UTF8: { return "flag"; } break;
        case DW_AT_extension: { return "reference"; } break;
        case DW_AT_ranges: { return "rnglist"; } break;
        case DW_AT_trampoline: { return "address, flag, reference, string"; } break;
        case DW_AT_call_column: { return "constant"; } break;
        case DW_AT_call_file: { return "constant"; } break;
        case DW_AT_call_line: { return "constant"; } break;
        case DW_AT_description: { return "string"; } break;
        case DW_AT_binary_scale: { return "constant"; } break;
        case DW_AT_decimal_scale: { return "constant"; } break;
        case DW_AT_small: { return "reference"; } break;
        case DW_AT_decimal_sign: { return "constant"; } break;
        case DW_AT_digit_count: { return "constant"; } break;
        case DW_AT_picture_string: { return "string"; } break;
        case DW_AT_mutable: { return "flag"; } break;
        case DW_AT_threads_scaled: { return "flag"; } break;
        case DW_AT_explicit: { return "flag"; } break;
        case DW_AT_object_pointer: { return "reference"; } break;
        case DW_AT_endianity: { return "constant"; } break;
        case DW_AT_elemental: { return "flag"; } break;
        case DW_AT_pure: { return "flag"; } break;
        case DW_AT_recursive: { return "flag"; } break;
        case DW_AT_signature: { return "reference"; } break;
        case DW_AT_main_subprogram: { return "flag"; } break;
        case DW_AT_data_bit_offset: { return "constant"; } break;
        case DW_AT_const_expr: { return "flag"; } break;
        case DW_AT_enum_class: { return "flag"; } break;
        case DW_AT_linkage_name: { return "string"; } break;
        case DW_AT_string_length_bit_size: { return "constant"; } break;
        case DW_AT_string_length_byte_size: { return "constant"; } break;
        case DW_AT_rank: { return "constant, exprloc"; } break;
        case DW_AT_str_offsets_base: { return "stroffsetsptr"; } break;
        case DW_AT_addr_base: { return "addrptr"; } break;
        case DW_AT_rnglists_base: { return "rnglistptr"; } break;
        case DW_AT_dwo_name: { return "string"; } break;
        case DW_AT_reference: { return "flag"; } break;
        case DW_AT_rvalue_reference: { return "flag"; } break;
        case DW_AT_macros: { return "macptr"; } break;
        case DW_AT_call_all_calls: { return "flag"; } break;
        case DW_AT_call_all_source_calls: { return "flag"; } break;
        case DW_AT_call_all_tail_calls: { return "flag"; } break;
        case DW_AT_call_return_pc: { return "address"; } break;
        case DW_AT_call_value: { return "exprloc"; } break;
        case DW_AT_call_origin: { return "exprloc"; } break;
        case DW_AT_call_parameter: { return "reference"; } break;
        case DW_AT_call_pc: { return "address"; } break;
        case DW_AT_call_tail_call: { return "flag"; } break;
        case DW_AT_call_target: { return "exprloc"; } break;
        case DW_AT_call_target_clobbered: { return "exprloc"; } break;
        case DW_AT_call_data_location: { return "exprloc"; } break;
        case DW_AT_call_data_value: { return "exprloc"; } break;
        case DW_AT_noreturn: { return "flag"; } break;
        case DW_AT_alignment: { return "constant"; } break;
        case DW_AT_export_symbols: { return "flag"; } break;
        case DW_AT_deleted: { return "flag"; } break;
        case DW_AT_defaulted: { return "constant"; } break;
        case DW_AT_loclists_base: { return "loclistsptr"; } break;

        case DW_AT_LLVM_sysroot: { return "string"; } break;
        case DW_AT_APPLE_sdk: { return "string"; } break;
        case DW_AT_APPLE_omit_frame_ptr: { return "??"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

enum DW_FORM {
    DW_FORM_addr = 0x01,
    DW_FORM_block2 = 0x03,
    DW_FORM_block4 = 0x04,
    DW_FORM_data2 = 0x05,
    DW_FORM_data4 = 0x06,
    DW_FORM_data8 = 0x07,
    DW_FORM_string = 0x08,
    DW_FORM_block = 0x09,
    DW_FORM_block1 = 0x0a,
    DW_FORM_data1 = 0x0b,
    DW_FORM_flag = 0x0c,
    DW_FORM_sdata = 0x0d,
    DW_FORM_strp = 0x0e,
    DW_FORM_udata = 0x0f,
    DW_FORM_ref_addr = 0x10,
    DW_FORM_ref1 = 0x11,
    DW_FORM_ref2 = 0x12,
    DW_FORM_ref4 = 0x13,
    DW_FORM_ref8 = 0x14,
    DW_FORM_ref_udata = 0x15,
    DW_FORM_indirect = 0x16,
    DW_FORM_sec_offset = 0x17,
    DW_FORM_exprloc = 0x18,
    DW_FORM_flag_present = 0x19,
    DW_FORM_strx = 0x1a,
    DW_FORM_addrx = 0x1b,
    DW_FORM_ref_sup4 = 0x1c,
    DW_FORM_strp_sup = 0x1d,
    DW_FORM_data16 = 0x1e,
    DW_FORM_line_strp = 0x1f,
    DW_FORM_ref_sig8 = 0x20,
    DW_FORM_implicit_const = 0x21,
    DW_FORM_loclistx = 0x22,
    DW_FORM_rnglistx = 0x23,
    DW_FORM_ref_sup8 = 0x24,
    DW_FORM_strx1 = 0x25,
    DW_FORM_strx2 = 0x26,
    DW_FORM_strx3 = 0x27,
    DW_FORM_strx4 = 0x28,
    DW_FORM_addrx1 = 0x29,
    DW_FORM_addrx2 = 0x2a,
    DW_FORM_addrx3 = 0x2b,
    DW_FORM_addrx4 = 0x2c,
};

utf8 const * dw_form_to_string(u64 dw_form) {
    switch (dw_form) {
        case DW_FORM_addr: { return "DW_FORM_addr"; } break;
        case DW_FORM_block2: { return "DW_FORM_block2"; } break;
        case DW_FORM_block4: { return "DW_FORM_block4"; } break;
        case DW_FORM_data2: { return "DW_FORM_data2"; } break;
        case DW_FORM_data4: { return "DW_FORM_data4"; } break;
        case DW_FORM_data8: { return "DW_FORM_data8"; } break;
        case DW_FORM_string: { return "DW_FORM_string"; } break;
        case DW_FORM_block: { return "DW_FORM_block"; } break;
        case DW_FORM_block1: { return "DW_FORM_block1"; } break;
        case DW_FORM_data1: { return "DW_FORM_data1"; } break;
        case DW_FORM_flag: { return "DW_FORM_flag"; } break;
        case DW_FORM_sdata: { return "DW_FORM_sdata"; } break;
        case DW_FORM_strp: { return "DW_FORM_strp"; } break;
        case DW_FORM_udata: { return "DW_FORM_udata"; } break;
        case DW_FORM_ref_addr: { return "DW_FORM_ref_addr"; } break;
        case DW_FORM_ref1: { return "DW_FORM_ref1"; } break;
        case DW_FORM_ref2: { return "DW_FORM_ref2"; } break;
        case DW_FORM_ref4: { return "DW_FORM_ref4"; } break;
        case DW_FORM_ref8: { return "DW_FORM_ref8"; } break;
        case DW_FORM_ref_udata: { return "DW_FORM_ref_udata"; } break;
        case DW_FORM_indirect: { return "DW_FORM_indirect"; } break;
        case DW_FORM_sec_offset: { return "DW_FORM_sec_offset"; } break;
        case DW_FORM_exprloc: { return "DW_FORM_exprloc"; } break;
        case DW_FORM_flag_present: { return "DW_FORM_flag_present"; } break;
        case DW_FORM_strx: { return "DW_FORM_strx"; } break;
        case DW_FORM_addrx: { return "DW_FORM_addrx"; } break;
        case DW_FORM_ref_sup4: { return "DW_FORM_ref_sup4"; } break;
        case DW_FORM_strp_sup: { return "DW_FORM_strp_sup"; } break;
        case DW_FORM_data16: { return "DW_FORM_data16"; } break;
        case DW_FORM_line_strp: { return "DW_FORM_line_strp"; } break;
        case DW_FORM_ref_sig8: { return "DW_FORM_ref_sig8"; } break;
        case DW_FORM_implicit_const: { return "DW_FORM_implicit_const"; } break;
        case DW_FORM_loclistx: { return "DW_FORM_loclistx"; } break;
        case DW_FORM_rnglistx: { return "DW_FORM_rnglistx"; } break;
        case DW_FORM_ref_sup8: { return "DW_FORM_ref_sup8"; } break;
        case DW_FORM_strx1: { return "DW_FORM_strx1"; } break;
        case DW_FORM_strx2: { return "DW_FORM_strx2"; } break;
        case DW_FORM_strx3: { return "DW_FORM_strx3"; } break;
        case DW_FORM_strx4: { return "DW_FORM_strx4"; } break;
        case DW_FORM_addrx1: { return "DW_FORM_addrx1"; } break;
        case DW_FORM_addrx2: { return "DW_FORM_addrx2"; } break;
        case DW_FORM_addrx3: { return "DW_FORM_addrx3"; } break;
        case DW_FORM_addrx4: { return "DW_FORM_addrx4"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

enum DW_LANG {
    DW_LANG_C89 = 0x01,
    DW_LANG_C = 0x02,
    DW_LANG_Ada83 = 0x03,
    DW_LANG_C_plus_plus = 0x04,
    DW_LANG_Cobol74 = 0x05,
    DW_LANG_Cobol85 = 0x06,
    DW_LANG_Fortran77 = 0x07,
    DW_LANG_Fortran90 = 0x08,
    DW_LANG_Pascal83 = 0x09,
    DW_LANG_Modula2 = 0x0a,
    DW_LANG_Java = 0x0b,
    DW_LANG_C99 = 0x0c,
    DW_LANG_Ada95 = 0x0d,
    DW_LANG_Fortran95 = 0x0e,
    DW_LANG_PLI = 0x0f,
    DW_LANG_ObjC = 0x10,
    DW_LANG_ObjC_plus_plus = 0x11,
    DW_LANG_UPC = 0x12,
    DW_LANG_D = 0x13,
    DW_LANG_Python = 0x14,
    DW_LANG_OpenCL = 0x15,
    DW_LANG_Go = 0x16,
    DW_LANG_Modula3 = 0x17,
    DW_LANG_Haskell = 0x18,
    DW_LANG_C_plus_plus_03 = 0x19,
    DW_LANG_C_plus_plus_11 = 0x1a,
    DW_LANG_OCaml = 0x1b,
    DW_LANG_Rust = 0x1c,
    DW_LANG_C11 = 0x1d,
    DW_LANG_Swift = 0x1e,
    DW_LANG_Julia = 0x1f,
    DW_LANG_Dylan = 0x20,
    DW_LANG_C_plus_plus_14 = 0x21,
    DW_LANG_Fortran03 = 0x22,
    DW_LANG_Fortran08 = 0x23,
    DW_LANG_RenderScript = 0x24,
    DW_LANG_BLISS = 0x25,
};

utf8 const * dw_lang_to_string(u64 dw_lang) {
    switch (dw_lang) {
        case DW_LANG_C89: { return "DW_LANG_C89"; } break;
        case DW_LANG_C: { return "DW_LANG_C"; } break;
        case DW_LANG_Ada83: { return "DW_LANG_Ada83"; } break;
        case DW_LANG_C_plus_plus: { return "DW_LANG_C_plus_plus"; } break;
        case DW_LANG_Cobol74: { return "DW_LANG_Cobol74"; } break;
        case DW_LANG_Cobol85: { return "DW_LANG_Cobol85"; } break;
        case DW_LANG_Fortran77: { return "DW_LANG_Fortran77"; } break;
        case DW_LANG_Fortran90: { return "DW_LANG_Fortran90"; } break;
        case DW_LANG_Pascal83: { return "DW_LANG_Pascal83"; } break;
        case DW_LANG_Modula2: { return "DW_LANG_Modula2"; } break;
        case DW_LANG_Java: { return "DW_LANG_Java"; } break;
        case DW_LANG_C99: { return "DW_LANG_C99"; } break;
        case DW_LANG_Ada95: { return "DW_LANG_Ada95"; } break;
        case DW_LANG_Fortran95: { return "DW_LANG_Fortran95"; } break;
        case DW_LANG_PLI: { return "DW_LANG_PLI"; } break;
        case DW_LANG_ObjC: { return "DW_LANG_ObjC"; } break;
        case DW_LANG_ObjC_plus_plus: { return "DW_LANG_ObjC_plus_plus"; } break;
        case DW_LANG_UPC: { return "DW_LANG_UPC"; } break;
        case DW_LANG_D: { return "DW_LANG_D"; } break;
        case DW_LANG_Python: { return "DW_LANG_Python"; } break;
        case DW_LANG_OpenCL: { return "DW_LANG_OpenCL"; } break;
        case DW_LANG_Go: { return "DW_LANG_Go"; } break;
        case DW_LANG_Modula3: { return "DW_LANG_Modula3"; } break;
        case DW_LANG_Haskell: { return "DW_LANG_Haskell"; } break;
        case DW_LANG_C_plus_plus_03: { return "DW_LANG_C_plus_plus_03"; } break;
        case DW_LANG_C_plus_plus_11: { return "DW_LANG_C_plus_plus_11"; } break;
        case DW_LANG_OCaml: { return "DW_LANG_OCaml"; } break;
        case DW_LANG_Rust: { return "DW_LANG_Rust"; } break;
        case DW_LANG_C11: { return "DW_LANG_C11"; } break;
        case DW_LANG_Swift: { return "DW_LANG_Swift"; } break;
        case DW_LANG_Julia: { return "DW_LANG_Julia"; } break;
        case DW_LANG_Dylan: { return "DW_LANG_Dylan"; } break;
        case DW_LANG_C_plus_plus_14: { return "DW_LANG_C_plus_plus_14"; } break;
        case DW_LANG_Fortran03: { return "DW_LANG_Fortran03"; } break;
        case DW_LANG_Fortran08: { return "DW_LANG_Fortran08"; } break;
        case DW_LANG_RenderScript: { return "DW_LANG_RenderScript"; } break;
        case DW_LANG_BLISS: { return "DW_LANG_BLISS"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

enum DW_LNS {
    DW_LNS_copy = 0x01,
    DW_LNS_advance_pc = 0x02,
    DW_LNS_advance_line = 0x03,
    DW_LNS_set_file = 0x04,
    DW_LNS_set_column = 0x05,
    DW_LNS_negate_stmt = 0x06,
    DW_LNS_set_basic_block = 0x07,
    DW_LNS_const_add_pc = 0x08,
    DW_LNS_fixed_advance_pc = 0x09,
    DW_LNS_set_prologue_end = 0x0a,
    DW_LNS_set_epilogue_begin = 0x0b,
    DW_LNS_set_isa = 0x0c,
};

utf8 const * dw_lns_to_string(u64 dw_lns) {
    switch (dw_lns) {
        case DW_LNS_copy: { return "DW_LNS_copy"; } break;
        case DW_LNS_advance_pc: { return "DW_LNS_advance_pc"; } break;
        case DW_LNS_advance_line: { return "DW_LNS_advance_line"; } break;
        case DW_LNS_set_file: { return "DW_LNS_set_file"; } break;
        case DW_LNS_set_column: { return "DW_LNS_set_column"; } break;
        case DW_LNS_negate_stmt: { return "DW_LNS_negate_stmt"; } break;
        case DW_LNS_set_basic_block: { return "DW_LNS_set_basic_block"; } break;
        case DW_LNS_const_add_pc: { return "DW_LNS_const_add_pc"; } break;
        case DW_LNS_fixed_advance_pc: { return "DW_LNS_fixed_advance_pc"; } break;
        case DW_LNS_set_prologue_end: { return "DW_LNS_set_prologue_end"; } break;
        case DW_LNS_set_epilogue_begin: { return "DW_LNS_set_epilogue_begin"; } break;
        case DW_LNS_set_isa: { return "DW_LNS_set_isa"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

enum DW_LNCT {
    DW_LNCT_path = 0x01,
    DW_LNCT_directory_index = 0x02,
    DW_LNCT_timestamp = 0x03,
    DW_LNCT_size = 0x04,
    DW_LNCT_MD5 = 0x05,
    DW_LNCT_lo_user = 0x2000,
    DW_LNCT_hi_user = 0x3fff,
};

utf8 const * dw_lnct_to_string(u64 dw_lnct) {
    switch (dw_lnct) {
        case DW_LNCT_path: { return "DW_LNCT_path"; } break;
        case DW_LNCT_directory_index: { return "DW_LNCT_directory_index"; } break;
        case DW_LNCT_timestamp: { return "DW_LNCT_timestamp"; } break;
        case DW_LNCT_size: { return "DW_LNCT_size"; } break;
        case DW_LNCT_MD5: { return "DW_LNCT_MD5"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

enum DW_CHILDREN : u8 {
    DW_CHILDREN_no = 0x00,
    DW_CHILDREN_yes = 0x01,
};

utf8 const * dw_children_to_string(DW_CHILDREN dw_children) {
    switch (dw_children) {
        case DW_CHILDREN_no: { return "DW_CHILDREN_no"; } break;
        case DW_CHILDREN_yes: { return "DW_CHILDREN_yes"; } break;

        default: {
            ASSERT_NOT_REACHED();
        } break;
    }
}

