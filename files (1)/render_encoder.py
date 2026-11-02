#!/usr/bin/env python3
"""
render_encoder.py — Step 4 of the BOE CodeGen build.

Walks each message's fields in wire order and turns them into a list
of "encode steps" (const / computed-reserve / int / string / enum_char
/ group_count), plus per-repeating-group metadata (element size, field
list). The template then turns each step into one C++ statement.
"""

import sys

from jinja2 import Environment, FileSystemLoader

from common import (
    BASE_DIR, SCHEMA_PATH, TEMPLATE_DIR, GENERATED_DIR,
    load_schema, field_byte_size,
)

TEMPLATE_NAME = "encoder.h.j2"
OUTPUT_PATH = GENERATED_DIR / "BOE_Encoder.h"


def build_message_steps(msg_def: dict, header: dict, data_types: dict) -> tuple[list, int]:
    """Returns (steps, fixed_part_bytes) for one message, in wire order."""
    steps = []
    fixed_part_bytes = 0

    def add(source_field: dict):
        nonlocal fixed_part_bytes
        kind = source_field.get("kind", "fixed")
        size = field_byte_size(source_field, data_types)
        fixed_part_bytes += size
        bits = size * 8

        if kind == "constant":
            steps.append({"op": "const", "name": source_field["name"],
                           "bytes": size, "bits": bits})
        elif kind == "computed":
            steps.append({"op": "computed_reserve", "name": source_field["name"],
                           "bytes": size, "bits": bits})
        elif kind == "constrained":
            steps.append({"op": "enum_char", "name": source_field["name"],
                           "bytes": size})
        elif kind == "group_count":
            steps.append({"op": "group_count", "name": source_field["name"],
                           "bytes": size, "bits": bits,
                           "group_field": source_field["group"]})
        else:
            type_def = data_types[source_field["type"]]
            op = "string" if type_def.get("kind") == "fixed_string" else "int"
            steps.append({"op": op, "name": source_field["name"],
                           "bytes": size, "bits": bits})

    for f in header["fields"]:
        add(f)
    for f in msg_def.get("fields", []):
        add(f)

    return steps, fixed_part_bytes


def build_group_context(msg_def: dict, groups: dict, data_types: dict) -> list:
    result = []
    for rg in msg_def.get("repeating_groups", []):
        group_def = groups[rg["group_type"]]
        element_bytes = sum(field_byte_size(gf, data_types) for gf in group_def["fields"])

        group_fields = []
        for gf in group_def["fields"]:
            gsize = field_byte_size(gf, data_types)
            type_def = data_types[gf["type"]]
            op = "string" if type_def.get("kind") == "fixed_string" else "int"
            group_fields.append({"op": op, "name": gf["name"],
                                  "bytes": gsize, "bits": gsize * 8})

        result.append({
            "name": rg["name"],
            "element_bytes": element_bytes,
            "fields": group_fields,
        })
    return result


def build_message_context(msg_name: str, msg_def: dict, header: dict,
                           groups: dict, data_types: dict) -> dict:
    steps, fixed_part_bytes = build_message_steps(msg_def, header, data_types)
    return {
        "name": msg_name,
        "fixed_part_bytes": fixed_part_bytes,
        "steps": steps,
        "groups": build_group_context(msg_def, groups, data_types),
    }


def main():
    if not SCHEMA_PATH.exists():
        print(f"Schema not found: {SCHEMA_PATH}", file=sys.stderr)
        sys.exit(1)

    schema = load_schema()
    data_types = schema["data_types"]
    header = schema["header"]
    groups = schema["groups"]
    messages = schema["messages"]

    context = {
        "schema_file": SCHEMA_PATH.name,
        "protocol": schema["protocol"],
        "exchange": schema["exchange"],
        "schema_version": schema["schema_version"],
        "messages": [
            build_message_context(name, msg_def, header, groups, data_types)
            for name, msg_def in messages.items()
        ],
    }

    env = Environment(
        loader=FileSystemLoader(str(TEMPLATE_DIR)),
        trim_blocks=True,
        lstrip_blocks=True,
    )
    template = env.get_template(TEMPLATE_NAME)
    rendered = template.render(**context)

    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_PATH.write_text(rendered)

    print(f"Rendered: {OUTPUT_PATH}")


if __name__ == "__main__":
    main()
