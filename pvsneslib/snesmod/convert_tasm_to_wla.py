#!/usr/bin/env python3
"""
Convert TASM SPC700 assembly to WLA-DX SPC700 syntax.
"""

import re
import sys

def convert_hex(match):
    """Convert TASM hex (0F0h) to WLA-DX hex ($F0)"""
    hex_val = match.group(1)
    return f"${hex_val.upper()}"

def convert_line(line, in_enum=False):
    """Convert a single line from TASM to WLA-DX syntax"""
    original = line

    # Skip empty lines and pure comments
    stripped = line.strip()
    if not stripped or stripped.startswith(';'):
        return line, None

    # Handle #define with backslash continuation (multi-statement macros)
    if stripped.startswith('#define'):
        if '\\' in line:
            match = re.match(r'#define\s+(\w+)(?:\(([^)]*)\))?\s*(.*)', stripped)
            if match:
                name = match.group(1)
                args = match.group(2) or ''
                body = match.group(3)
                body = body.replace('\\', '\n    ')
                if args:
                    return f".MACRO {name} ARGS {args}\n    {body}\n.ENDM\n", 'macro'
                else:
                    return f".MACRO {name}\n    {body}\n.ENDM\n", 'macro'
        else:
            match = re.match(r'#define\s+(\w+)(?:\(([^)]*)\))?\s*(.*)', stripped)
            if match:
                name = match.group(1)
                args = match.group(2)
                value = match.group(3)
                if args:
                    return f".MACRO {name} ARGS {args}\n    {value}\n.ENDM\n", 'macro'
                else:
                    if re.match(r'\s*(TCALL|MOV|JMP|CALL|RET|BRA|BNE|BEQ|NOP)\b', value, re.IGNORECASE):
                        return f".MACRO {name}\n    {value}\n.ENDM\n", 'macro'
                    return f".DEFINE {name} {value}\n", 'define'

    # Handle .define
    if stripped.startswith('.define'):
        # Skip LBYTE/HBYTE definitions - WLA-DX uses < and > operators
        if re.match(r'\.define\s+(LBYTE|HBYTE)\(', stripped, re.IGNORECASE):
            return '', None
        match = re.match(r'^(\s*)\.define\s+(\w+)\(([^)]*)\)\s*(.*)', line)
        if match:
            indent = match.group(1)
            name = match.group(2)
            args = match.group(3)
            value = match.group(4)
            return f"{indent}.MACRO {name} ARGS {args}\n{indent}    {value}\n{indent}.ENDM\n", 'macro'
        match = re.match(r'^(\s*)\.define\s+(\w+)\s+(.*)', line)
        if match:
            indent = match.group(1)
            name = match.group(2)
            value = match.group(3).strip()
            # If value is an instruction or references a macro that is an instruction
            if re.match(r'(TCALL|MOV|JMP|CALL|RET|BRA|BNE|BEQ|NOP)\b', value, re.IGNORECASE):
                return f"{indent}.MACRO {name}\n{indent}    {value}\n{indent}.ENDM\n", 'macro'
            # If value is another macro name (like SPROC2 = SPROC), create a macro that calls it
            if re.match(r'^[A-Z_][A-Z0-9_]*$', value):
                return f"{indent}.MACRO {name}\n{indent}    {value}\n{indent}.ENDM\n", 'macro'
            return f"{indent}.DEFINE {name} {value}\n", 'define'

    # Handle NAME = value -> .EQU NAME value
    match = re.match(r'^(\s*)([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.+)$', line)
    if match:
        indent = match.group(1)
        name = match.group(2)
        value = match.group(3)
        value = re.sub(r'([0-9A-Fa-f]+)[hH]\b', convert_hex, value)
        return f"{indent}.EQU {name} {value}\n", 'equ'

    # Handle .block n (RAM variables)
    match = re.match(r'^(\s*)([A-Za-z_][A-Za-z0-9_]*):\s*\.block\s+(\d+)', line)
    if match:
        indent = match.group(1)
        label = match.group(2)
        size = match.group(3)
        return f"{indent}{label} dsb {size}\n", 'ram'

    # Handle .org -> .ORG (must check before label detection)
    if '.org' in line.lower():
        line = re.sub(r'\.org\s+', '.ORG ', line, flags=re.IGNORECASE)
        line = re.sub(r'([0-9A-Fa-f]+)[hH]\b', convert_hex, line)
        return line, 'org'

    # Handle .word -> .DW
    line = re.sub(r'\.word\s+', '.DW ', line)

    # Handle .byte -> .DB
    line = re.sub(r'\.byte\s+', '.DB ', line)

    # Convert hex numbers
    line = re.sub(r'\b([0-9][0-9A-Fa-f]*)[hH]\b', convert_hex, line)

    # Convert binary: %110 -> %00000110
    # Also handle TASM suffix notation: 11100000b -> %11100000
    def pad_binary(match):
        bits = match.group(1)
        return f"%{bits.zfill(8)}"
    line = re.sub(r'%([01]+)\b', pad_binary, line)
    # Convert TASM binary suffix: 11100000b -> %11100000
    line = re.sub(r'\b([01]+)b\b', r'%\1', line)

    # Convert LBYTE(x) -> <x and HBYTE(x) -> >x
    line = re.sub(r'\bLBYTE\(([^)]+)\)', r'<\1', line, flags=re.IGNORECASE)
    line = re.sub(r'\bHBYTE\(([^)]+)\)', r'>\1', line, flags=re.IGNORECASE)

    # Convert bit-branch instructions: bbc4 addr, label -> bbc addr.4, label
    # TASM: bbc4 m0, label / bbs5 m0, label
    # WLA-DX: bbc m0.4, label / bbs m0.5, label
    line = re.sub(r'\b(bbc|bbs)([0-7])\s+([^,]+),', r'\1 \3.\2,', line, flags=re.IGNORECASE)
    # Also handle set1/clr1: set1 4, addr -> set1 addr.4
    line = re.sub(r'\b(set1|clr1)\s+([0-7])\s*,\s*(\S+)', r'\1 \3.\2', line, flags=re.IGNORECASE)

    # WLA-DX SPC700 needs ! prefix for call/jmp absolute addresses
    # call label -> call !label
    # jmp label -> jmp !label
    # jmp $addr -> jmp !$addr
    # For indirect jumps: jmp [addr+x] -> jmp [!addr+x]
    def add_absolute_prefix(match):
        instr = match.group(1)
        target = match.group(2)
        # For indirect addressing [label+x], add ! inside the brackets
        if target.startswith('['):
            # Add ! after the opening bracket, before the label
            inner = target[1:-1]  # Remove [ and ]
            # Add ! before any label in the expression
            inner = re.sub(r'([A-Za-z_][A-Za-z0-9_]*)', r'!\1', inner, count=1)
            return f"{instr} [{inner}]"
        # Already has !, leave it alone
        if target.startswith('!'):
            return f"{instr} {target}"
        return f"{instr} !{target}"
    # Match labels, hex addresses ($xxx), and indirect addressing [...]
    line = re.sub(r'\b(call|jmp)\s+(\[[^\]]+\]|\$[0-9A-Fa-f]+|[A-Za-z_][A-Za-z0-9_]*)', add_absolute_prefix, line, flags=re.IGNORECASE)

    return line, 'code'

def convert_file(input_path, output_path):
    """Convert entire file from TASM to WLA-DX"""

    with open(input_path, 'r') as f:
        lines = f.readlines()

    # Categorize and convert all lines
    macros_and_defines = []
    equates = []
    ram_vars = []
    code_lines = []

    in_multiline_define = False
    multiline_buffer = ""
    in_ram_section = False

    for i, line in enumerate(lines):
        # Skip original header (first 8 lines)
        if i < 8:
            continue

        # Handle multi-line #define
        if in_multiline_define:
            multiline_buffer += line
            if '\\' not in line:
                in_multiline_define = False
                converted, ltype = convert_line(multiline_buffer)
                macros_and_defines.append(converted)
                multiline_buffer = ""
            continue

        if '#define' in line and '\\' in line:
            in_multiline_define = True
            multiline_buffer = line
            continue

        converted, ltype = convert_line(line)

        if ltype == 'macro' or ltype == 'define':
            macros_and_defines.append(converted)
        elif ltype == 'equ':
            equates.append(converted)
        elif ltype == 'ram':
            ram_vars.append(converted)
            in_ram_section = True
        elif ltype == 'org':
            in_ram_section = False
            code_lines.append(converted)
        elif ltype == 'code' or ltype is None:
            if in_ram_section:
                stripped_conv = converted.strip()
                # Check if this is a label-only line (e.g., ch_start:)
                label_match = re.match(r'^([A-Za-z_][A-Za-z0-9_]*):\s*$', stripped_conv)
                if label_match:
                    label = label_match.group(1)
                    ram_vars.append(f"{label} dsb 0\n")
                elif stripped_conv.startswith(';') or not stripped_conv:
                    # Comment or blank line in RAM section
                    ram_vars.append(converted)
                elif 'dsb' in converted.lower():
                    ram_vars.append(converted)
                else:
                    # This might be code that shouldn't be in RAM section
                    code_lines.append(converted)
            else:
                code_lines.append(converted)

    # Build final output
    header = """;========================================================
; SM-SPC - WLA-DX SPC700 Version
;
; snesmod spc driver
; Converted from TASM to WLA-DX syntax
;
; (c) 2009 Mukunda Johnson
; Bugfix by KungFuFurby
; WLA-DX conversion by pvsneslib team
;========================================================

.MEMORYMAP
    DEFAULTSLOT 0
    SLOTSIZE $10000
    SLOT 0 $0000
.ENDME

.ROMBANKMAP
    BANKSTOTAL 1
    BANKSIZE $10000
    BANKS 1
.ENDRO

.BANK 0 SLOT 0

"""

    output = [header]

    # Add macros and defines first
    output.append(";--- MACROS AND DEFINES ---\n")
    output.extend(macros_and_defines)
    output.append("\n")

    # Add equates
    output.append(";--- CONSTANTS ---\n")
    output.extend(equates)
    output.append("\n")

    # Add RAM variables in ENUM
    output.append(";--- RAM VARIABLES (Zero Page) ---\n")
    output.append(".ENUM $0000\n")
    output.extend(ram_vars)
    output.append(".ENDE\n\n")

    # Add code - skip .ORG lines (we'll use section address) and use FORCE
    output.append(";--- CODE ---\n")
    output.append(".ORG $400\n")
    output.append(".SECTION \"SPC_CODE\" FORCE\n")
    for line in code_lines:
        # Skip .ORG directives since we're at fixed address
        if '.ORG' in line:
            continue
        output.append(line)
    output.append("\n.ENDS\n")

    with open(output_path, 'w') as f:
        f.writelines(output)

    print(f"Converted {input_path} -> {output_path}")
    print(f"  Macros/Defines: {len(macros_and_defines)}")
    print(f"  Constants: {len(equates)}")
    print(f"  RAM vars: {len(ram_vars)}")
    print(f"  Code lines: {len(code_lines)}")

if __name__ == '__main__':
    input_file = 'sm_spc.as7'
    output_file = 'sm_spc_wla.asm'

    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    if len(sys.argv) > 2:
        output_file = sys.argv[2]

    convert_file(input_file, output_file)
