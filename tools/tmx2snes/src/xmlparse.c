/*---------------------------------------------------------------------------------

    xmlparse.c - see xmlparse.h

---------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "xmlparse.h"
#include "errors.h"

//-------------------------------------------------------------------------------------------------
// small growable string buffer, used while collecting text/attribute content
typedef struct
{
    char *data;
    size_t len;
    size_t cap;
} xstrbuf_t;

static void xsb_init(xstrbuf_t *b)
{
    b->cap = 64;
    b->len = 0;
    b->data = (char *)malloc(b->cap);
    b->data[0] = '\0';
}

static void xsb_putc(xstrbuf_t *b, char c)
{
    if (b->len + 2 > b->cap)
    {
        b->cap *= 2;
        b->data = (char *)realloc(b->data, b->cap);
    }
    b->data[b->len++] = c;
    b->data[b->len] = '\0';
}

//-------------------------------------------------------------------------------------------------
static const char *xml_skip_ws(const char *p)
{
    while (*p && isspace((unsigned char)*p)) p++;
    return p;
}

// Decodes a single XML entity starting at '&' (p points at '&'). Writes decoded
// byte(s) to buf, returns pointer just past the terminating ';'.
static const char *xml_decode_entity(const char *p, xstrbuf_t *buf)
{
    if (strncmp(p, "&lt;", 4) == 0) { xsb_putc(buf, '<'); return p + 4; }
    if (strncmp(p, "&gt;", 4) == 0) { xsb_putc(buf, '>'); return p + 4; }
    if (strncmp(p, "&amp;", 5) == 0) { xsb_putc(buf, '&'); return p + 5; }
    if (strncmp(p, "&apos;", 6) == 0) { xsb_putc(buf, '\''); return p + 6; }
    if (strncmp(p, "&quot;", 6) == 0) { xsb_putc(buf, '"'); return p + 6; }
    if (strncmp(p, "&#x", 3) == 0 || strncmp(p, "&#X", 3) == 0)
    {
        char *end;
        long code = strtol(p + 3, &end, 16);
        if (*end == ';')
        {
            xsb_putc(buf, (char)code); // ASCII-range codepoints are all tmx2snes needs
            return end + 1;
        }
    }
    if (strncmp(p, "&#", 2) == 0)
    {
        char *end;
        long code = strtol(p + 2, &end, 10);
        if (*end == ';')
        {
            xsb_putc(buf, (char)code);
            return end + 1;
        }
    }
    // Not a recognized entity: emit literally.
    xsb_putc(buf, *p);
    return p + 1;
}

//-------------------------------------------------------------------------------------------------
static xml_node_t *xml_alloc_node(void)
{
    xml_node_t *n = (xml_node_t *)calloc(1, sizeof(xml_node_t));
    return n;
}

static xml_attr_t *xml_alloc_attr(void)
{
    xml_attr_t *a = (xml_attr_t *)calloc(1, sizeof(xml_attr_t));
    return a;
}

// Parses a quoted attribute value (single or double quotes), decoding entities.
static const char *xml_parse_attr_value(const char *p, char **out)
{
    char quote;
    xstrbuf_t buf;
    xsb_init(&buf);

    quote = *p;
    if (quote != '"' && quote != '\'')
    {
        free(buf.data);
        *out = NULL;
        return p;
    }
    p++;

    while (*p && *p != quote)
    {
        if (*p == '&') p = xml_decode_entity(p, &buf);
        else { xsb_putc(&buf, *p); p++; }
    }
    if (*p == quote) p++;

    *out = buf.data;
    return p;
}

// Parses "name='value' name2=\"value2\" ..." following a tag name, up to '>' or '/'.
static const char *xml_parse_attrs(const char *p, xml_attr_t **out_list)
{
    xml_attr_t *head = NULL, *tail = NULL;

    for (;;)
    {
        p = xml_skip_ws(p);
        if (*p == '>' || *p == '/' || *p == '\0') break;

        xstrbuf_t namebuf;
        xsb_init(&namebuf);
        while (*p && *p != '=' && !isspace((unsigned char)*p) && *p != '>' && *p != '/')
        {
            xsb_putc(&namebuf, *p);
            p++;
        }
        p = xml_skip_ws(p);
        if (*p != '=')
        {
            // malformed / boolean attribute with no value: skip it
            free(namebuf.data);
            continue;
        }
        p++; // consume '='
        p = xml_skip_ws(p);

        char *value = NULL;
        p = xml_parse_attr_value(p, &value);

        xml_attr_t *a = xml_alloc_attr();
        a->name = namebuf.data;
        a->value = value ? value : strdup("");

        if (!head) head = tail = a;
        else { tail->next = a; tail = a; }
    }

    *out_list = head;
    return p;
}

// Parses a single element starting at '<' (p points at '<', not whitespace before it).
// Returns pointer just after the element's closing tag (or after '/>').
static const char *xml_parse_element(const char *p, xml_node_t **out_node)
{
    xml_node_t *node;
    xstrbuf_t tagbuf;

    if (*p != '<') { *out_node = NULL; return p; }
    p++;

    xsb_init(&tagbuf);
    while (*p && !isspace((unsigned char)*p) && *p != '>' && *p != '/')
    {
        xsb_putc(&tagbuf, *p);
        p++;
    }

    node = xml_alloc_node();
    node->tag = tagbuf.data;

    p = xml_parse_attrs(p, &node->attrs);
    p = xml_skip_ws(p);

    if (*p == '/' && *(p + 1) == '>')
    {
        // self-closing: <tag .../>
        *out_node = node;
        return p + 2;
    }

    if (*p == '>') p++;

    // parse children / text content until matching close tag
    {
        xstrbuf_t textbuf;
        xsb_init(&textbuf);

        xml_node_t *child_head = NULL, *child_tail = NULL;

        for (;;)
        {
            if (*p == '\0')
            {
                fatal("unexpected end of file while parsing XML element <%s>", node->tag);
            }

            if (*p == '<')
            {
                if (strncmp(p, "<!--", 4) == 0)
                {
                    p += 4;
                    const char *end = strstr(p, "-->");
                    p = end ? end + 3 : p + strlen(p);
                    continue;
                }
                if (strncmp(p, "<![CDATA[", 9) == 0)
                {
                    p += 9;
                    const char *end = strstr(p, "]]>");
                    size_t n = end ? (size_t)(end - p) : strlen(p);
                    size_t i;
                    for (i = 0; i < n; i++) xsb_putc(&textbuf, p[i]);
                    p = end ? end + 3 : p + n;
                    continue;
                }
                if (*(p + 1) == '/')
                {
                    // closing tag: </tag>
                    p += 2;
                    p = xml_skip_ws(p);
                    // (we trust well-formedness of Tiled-generated files, skip name match)
                    while (*p && *p != '>') p++;
                    if (*p == '>') p++;
                    break;
                }
                else
                {
                    xml_node_t *child = NULL;
                    p = xml_parse_element(p, &child);
                    if (child)
                    {
                        if (!child_head) child_head = child_tail = child;
                        else { child_tail->next = child; child_tail = child; }
                    }
                    continue;
                }
            }
            else if (*p == '&')
            {
                p = xml_decode_entity(p, &textbuf);
                continue;
            }
            else
            {
                xsb_putc(&textbuf, *p);
                p++;
            }
        }

        node->children = child_head;
        // trim text: keep it only if it has non-whitespace content, otherwise NULL it
        {
            const char *t = textbuf.data;
            int has_content = 0;
            while (*t) { if (!isspace((unsigned char)*t)) { has_content = 1; break; } t++; }
            if (has_content) node->text = textbuf.data;
            else free(textbuf.data);
        }
    }

    *out_node = node;
    return p;
}

//-------------------------------------------------------------------------------------------------
xml_node_t *xml_parse(char *buffer)
{
    const char *p = buffer;
    xml_node_t *root = NULL;

    // skip BOM if present
    if ((unsigned char)p[0] == 0xEF && (unsigned char)p[1] == 0xBB && (unsigned char)p[2] == 0xBF) p += 3;

    for (;;)
    {
        p = xml_skip_ws(p);
        if (*p == '\0') break;

        if (strncmp(p, "<?xml", 5) == 0)
        {
            const char *end = strstr(p, "?>");
            p = end ? end + 2 : p + strlen(p);
            continue;
        }
        if (strncmp(p, "<!--", 4) == 0)
        {
            const char *end = strstr(p, "-->");
            p = end ? end + 3 : p + strlen(p);
            continue;
        }
        if (strncmp(p, "<!DOCTYPE", 9) == 0)
        {
            const char *end = strchr(p, '>');
            p = end ? end + 1 : p + strlen(p);
            continue;
        }

        if (*p == '<')
        {
            p = xml_parse_element(p, &root);
            break; // root element found, we're done (TMX/TSX have a single root)
        }

        // unexpected character before root element
        p++;
    }

    return root;
}

xml_node_t *xml_parse_file(const char *path)
{
    FILE *f;
    long size;
    char *buffer;
    xml_node_t *root;

    f = fopen(path, "rb");
    if (f == NULL)
    {
        fatal("can't open XML file [%s]", path);
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = (char *)malloc((size_t)size + 1);
    if (buffer == NULL)
    {
        fclose(f);
        fatal("can't allocate memory to read XML file [%s]", path);
    }
    if (fread(buffer, 1, (size_t)size, f) != (size_t)size)
    {
        fclose(f);
        free(buffer);
        fatal("failed to read XML file [%s]", path);
    }
    buffer[size] = '\0';
    fclose(f);

    root = xml_parse(buffer);
    // Note: buffer is intentionally leaked as-is for node tag/text storage independence
    // (all node/attr strings are copied into their own allocations during parsing, so
    // it is safe to free it -- but we keep the tool's memory model simple/short-lived,
    // matching how tmx2snes already frees only at end-of-process).
    return root;
}

//-------------------------------------------------------------------------------------------------
static void xml_free_attrs(xml_attr_t *a)
{
    while (a)
    {
        xml_attr_t *next = a->next;
        free(a->name);
        free(a->value);
        free(a);
        a = next;
    }
}

void xml_free(xml_node_t *node)
{
    while (node)
    {
        xml_node_t *next = node->next;
        xml_free_attrs(node->attrs);
        xml_free(node->children);
        free(node->tag);
        free(node->text);
        free(node);
        node = next;
    }
}

//-------------------------------------------------------------------------------------------------
const char *xml_attr(xml_node_t *node, const char *name)
{
    xml_attr_t *a;
    if (!node) return NULL;
    for (a = node->attrs; a; a = a->next)
    {
        if (strcmp(a->name, name) == 0) return a->value;
    }
    return NULL;
}

const char *xml_attr_default(xml_node_t *node, const char *name, const char *def)
{
    const char *v = xml_attr(node, name);
    return v ? v : def;
}

int xml_attr_int(xml_node_t *node, const char *name, int def)
{
    const char *v = xml_attr(node, name);
    return v ? atoi(v) : def;
}

float xml_attr_float(xml_node_t *node, const char *name, float def)
{
    const char *v = xml_attr(node, name);
    return v ? (float)atof(v) : def;
}

xml_node_t *xml_child(xml_node_t *node, const char *tag)
{
    xml_node_t *c;
    if (!node) return NULL;
    for (c = node->children; c; c = c->next)
    {
        if (strcmp(c->tag, tag) == 0) return c;
    }
    return NULL;
}

xml_node_t *xml_child_next(xml_node_t *node, xml_node_t *after, const char *tag)
{
    xml_node_t *c = after ? after->next : (node ? node->children : NULL);
    for (; c; c = c->next)
    {
        if (strcmp(c->tag, tag) == 0) return c;
    }
    return NULL;
}
