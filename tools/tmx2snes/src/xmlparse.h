/*---------------------------------------------------------------------------------

    xmlparse.h / xmlparse.c

    A minimal, dependency-free XML DOM parser, purpose-built to read the
    subset of XML used by Tiled's native .tmx map files and .tsx external
    tileset files. This is NOT a general-purpose/validating XML parser:
    it supports exactly what Tiled emits (elements, quoted attributes,
    text content, self-closing tags, comments, <?xml ... ?> declarations,
    and the small set of entity references Tiled uses), which keeps it
    small and dependency-free like the rest of the tmx2snes toolchain.

---------------------------------------------------------------------------------*/
#ifndef _TMX2SNES_XMLPARSE_H
#define _TMX2SNES_XMLPARSE_H

typedef struct xml_attr_s
{
    char *name;
    char *value;
    struct xml_attr_s *next;
} xml_attr_t;

typedef struct xml_node_s
{
    char *tag;                 // element name, e.g. "layer"
    xml_attr_t *attrs;         // linked list of attributes
    struct xml_node_s *children; // first child
    struct xml_node_s *next;   // next sibling
    char *text;                // concatenated text content directly inside this node (may be NULL)
} xml_node_t;

// Parses an in-memory, NUL-terminated XML buffer and returns the root element.
// Returns NULL on malformed XML. Caller must free with xml_free().
xml_node_t *xml_parse(char *buffer);

// Parses a file from disk (reads whole file into memory, then calls xml_parse).
xml_node_t *xml_parse_file(const char *path);

// Recursively frees a node tree.
void xml_free(xml_node_t *node);

// Convenience helpers.
const char *xml_attr(xml_node_t *node, const char *name);           // NULL if absent
const char *xml_attr_default(xml_node_t *node, const char *name, const char *def);
int xml_attr_int(xml_node_t *node, const char *name, int def);
float xml_attr_float(xml_node_t *node, const char *name, float def);
xml_node_t *xml_child(xml_node_t *node, const char *tag);           // first child with tag, or NULL
xml_node_t *xml_child_next(xml_node_t *node, xml_node_t *after, const char *tag); // iterate all children with tag

#endif
