"""
common.py — shared schema-loading helpers for the BOE codegen scripts.

Both render_struct.py and render_encoder.py need to: load the YAML,
map YAML types to C++ types, compute a field's byte size, and turn a
constrained-field description into a valid C++ enumerator identifier.
Factored out here so both renderers agree on these rules by construction
instead of by copy-paste.
"""

import re
from pathlib import Path

import yaml

BASE_DIR = Path(__file__).resolve().parent.parent  # -> boe_codegen/
SCHEMA_PATH = BASE_DIR / "schema" / "boe_schema.yaml"
TEMPLATE_DIR = BASE_DIR / "templates"
GENERATED_DIR = BASE_DIR / "generated"

PRIMITIVE_CPP_TYPES = {
    "binary_uint8": "std::uint8_t",
    "binary_uint16": "std::uint16_t",
    "binary_uint32": "std::uint32_t",
}


def load_schema() -> dict:
    with open(SCHEMA_PATH) as f:
        return yaml.safe_load(f)


def field_byte_size(field: dict, data_types: dict) -> int:
    """Resolve a field's wire size in bytes."""
    type_def = data_types[field["type"]]
    if type_def.get("kind") == "fixed_string":
        return field["size"]
    return type_def["size"]


def cpp_type_for(field: dict, data_types: dict) -> str:
    type_def = data_types[field["type"]]
    if type_def.get("kind") == "fixed_string":
        return f"std::array<char, {field['size']}>"
    return PRIMITIVE_CPP_TYPES[field["type"]]


def to_pascal_identifier(description: str) -> str:
    """
    'Not authorized (invalid username/password)' -> 'NotAuthorized'
    Truncates at the first '(' so parenthetical detail doesn't bleed
    into the enumerator name, then strips non-alphanumerics.
    """
    head = description.split("(")[0]
    words = re.findall(r"[A-Za-z0-9]+", head)
    return "".join(w.capitalize() for w in words)


def ordered_fields(msg_def: dict, header: dict):
    """Yield (source, field_dict) in wire order: header fields first,
    then this message's own fields. Repeating groups are handled
    separately since they're variable-length."""
    for f in header["fields"]:
        yield ("header", f)
    for f in msg_def.get("fields", []):
        yield ("message", f)
