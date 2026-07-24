/*---------------------------------------------------------------------------------

    tmxconv.c

    Converts a native Tiled .tmx map (XML) into an in-memory buffer holding
    the same JSON text Tiled's own "Export As... JSON" would produce for
    that map -- restricted to exactly the fields cute_tiled.h (as vendored
    in this project) understands. This lets tmx2snes read .tmx files
    directly without requiring a separate JSON export step from Tiled.

    Notes on the target format (see cute_tiled.h dispatch tables):
      - tilesets must be embedded (no "source" key at the tileset level,
        which cute_tiled treats as "external tileset, fields left zeroed").
      - layer data must be a plain JSON integer array under "data" (no
        "encoding"/"compression" keys -- those are hard errors in this
        vendored cute_tiled). Only TMX's "csv" data encoding is supported
        here (Tiled's Map Properties > Tile Layer Format must be set to
        CSV); base64/zlib/gzip-encoded layers are rejected with a message
        telling the user to switch Tiled's setting and re-save.
      - property objects must be emitted as {"name":..,"type":..,"value":..}
        in that exact order (cute_tiled's property reader is positional).

---------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>

#include "tmxconv.h"
#include "xmlparse.h"
#include "errors.h"

//-------------------------------------------------------------------------------------------------
// growable output buffer
typedef struct
{
    char *data;
    size_t len;
    size_t cap;
} jbuf_t;

static void jb_init(jbuf_t *b)
{
    b->cap = 4096;
    b->len = 0;
    b->data = (char *)malloc(b->cap);
    b->data[0] = '\0';
}

static void jb_ensure(jbuf_t *b, size_t extra)
{
    if (b->len + extra + 1 > b->cap)
    {
        while (b->len + extra + 1 > b->cap) b->cap *= 2;
        b->data = (char *)realloc(b->data, b->cap);
    }
}

static void jb_raw(jbuf_t *b, const char *s)
{
    size_t n = strlen(s);
    jb_ensure(b, n);
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
}

static void jb_fmt(jbuf_t *b, const char *fmt, ...)
{
    char tmp[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);
    jb_raw(b, tmp);
}

// Emits a JSON-escaped, quoted string.
static void jb_str(jbuf_t *b, const char *s)
{
    jb_raw(b, "\"");
    if (s)
    {
        for (const unsigned char *p = (const unsigned char *)s; *p; p++)
        {
            switch (*p)
            {
                case '"':  jb_raw(b, "\\\""); break;
                case '\\': jb_raw(b, "\\\\"); break;
                case '\n': jb_raw(b, "\\n");  break;
                case '\r': jb_raw(b, "\\r");  break;
                case '\t': jb_raw(b, "\\t");  break;
                default:
                    if (*p < 0x20) jb_fmt(b, "\\u%04x", *p);
                    else
                    {
                        char c = (char)*p;
                        jb_ensure(b, 1);
                        b->data[b->len++] = c;
                        b->data[b->len] = '\0';
                    }
                    break;
            }
        }
    }
    jb_raw(b, "\"");
}

//-------------------------------------------------------------------------------------------------
// Collects a <data> element's tile GIDs into a malloc'd array of uint32_t.
// Only CSV (Tiled's "CSV" Tile Layer Format setting) and the legacy
// uncompressed XML <tile gid=".."/> form are supported: set the map's
// "Tile Layer Format" to CSV in Tiled's map properties before saving.
static uint32_t *read_layer_data(xml_node_t *data_node, int *out_count)
{
    const char *encoding = xml_attr(data_node, "encoding");

    if (xml_child(data_node, "chunk") != NULL)
    {
        fatal("infinite maps (chunked layer data) are not supported by tmx2snes -- disable 'Infinite map' in Tiled's map properties");
    }

    if (encoding == NULL)
    {
        // legacy XML tile list: <tile gid=".."/> per cell
        int cap = 1024, n = 0;
        uint32_t *arr = (uint32_t *)malloc(cap * sizeof(uint32_t));
        for (xml_node_t *t = xml_child(data_node, "tile"); t; t = xml_child_next(data_node, t, "tile"))
        {
            if (n == cap) { cap *= 2; arr = (uint32_t *)realloc(arr, cap * sizeof(uint32_t)); }
            arr[n++] = (uint32_t)strtoul(xml_attr_default(t, "gid", "0"), NULL, 10);
        }
        *out_count = n;
        return arr;
    }

    if (strcmp(encoding, "csv") == 0)
    {
        int cap = 1024, n = 0;
        uint32_t *arr = (uint32_t *)malloc(cap * sizeof(uint32_t));
        const char *p = data_node->text ? data_node->text : "";
        while (*p)
        {
            while (*p && (isspace((unsigned char)*p) || *p == ',')) p++;
            if (!*p) break;
            char *end;
            unsigned long v = strtoul(p, &end, 10);
            if (end == p) break;
            if (n == cap) { cap *= 2; arr = (uint32_t *)realloc(arr, cap * sizeof(uint32_t)); }
            arr[n++] = (uint32_t)v;
            p = end;
        }
        *out_count = n;
        return arr;
    }

    fatal("layer data encoding '%s' is not supported by tmx2snes -- in Tiled, set "
          "Map > Map Properties... > Tile Layer Format to 'CSV' and re-save", encoding);
    return NULL; // unreachable
}

//-------------------------------------------------------------------------------------------------
// Emits a <properties> element (if present) as a JSON "properties" array.
static void emit_properties(jbuf_t *jb, xml_node_t *parent)
{
    xml_node_t *props = xml_child(parent, "properties");
    if (!props) return;

    jb_raw(jb, ",\"properties\":[");
    int first = 1;
    for (xml_node_t *prop = xml_child(props, "property"); prop; prop = xml_child_next(props, prop, "property"))
    {
        if (!first) jb_raw(jb, ",");
        first = 0;

        const char *name = xml_attr_default(prop, "name", "");
        const char *type = xml_attr_default(prop, "type", "string");
        const char *value = xml_attr(prop, "value");
        if (!value) value = prop->text ? prop->text : ""; // multiline strings store value as text content

        jb_raw(jb, "{\"name\":");
        jb_str(jb, name);
        jb_raw(jb, ",\"type\":");
        jb_str(jb, type);
        jb_raw(jb, ",\"value\":");

        if (strcmp(type, "bool") == 0)
        {
            jb_raw(jb, (strcmp(value, "true") == 0) ? "true" : "false");
        }
        else if (strcmp(type, "int") == 0 || strcmp(type, "float") == 0)
        {
            jb_raw(jb, (value[0] != '\0') ? value : "0");
        }
        else
        {
            // string, color, file, object -- all represented as JSON strings,
            // exactly like Tiled's own JSON property export
            jb_str(jb, value);
        }
        jb_raw(jb, "}");
    }
    jb_raw(jb, "]");
}

//-------------------------------------------------------------------------------------------------
static void emit_tile_descriptor(jbuf_t *jb, xml_node_t *tile, int *first)
{
    if (!*first) jb_raw(jb, ",");
    *first = 0;

    jb_fmt(jb, "{\"id\":%d", xml_attr_int(tile, "id", 0));

    const char *type = xml_attr(tile, "type");
    if (!type) type = xml_attr(tile, "class"); // Tiled 1.9+ renamed type->class on some elements
    if (type)
    {
        jb_raw(jb, ",\"type\":");
        jb_str(jb, type);
    }

    xml_node_t *anim = xml_child(tile, "animation");
    if (anim)
    {
        jb_raw(jb, ",\"animation\":[");
        int afirst = 1;
        for (xml_node_t *fr = xml_child(anim, "frame"); fr; fr = xml_child_next(anim, fr, "frame"))
        {
            if (!afirst) jb_raw(jb, ",");
            afirst = 0;
            jb_fmt(jb, "{\"tileid\":%d,\"duration\":%d}", xml_attr_int(fr, "tileid", 0), xml_attr_int(fr, "duration", 0));
        }
        jb_raw(jb, "]");
    }

    emit_properties(jb, tile);
    jb_raw(jb, "}");
}

// Emits a full tileset object (either inline in the .tmx, or loaded from an
// external .tsx referenced by "source"). firstgid always comes from the
// <tileset> reference element in the map, per the TMX/TMJ spec.
static void emit_tileset(jbuf_t *jb, xml_node_t *tileset_ref, const char *tmx_dir, bool isquiet)
{
    int firstgid = xml_attr_int(tileset_ref, "firstgid", 1);
    const char *source = xml_attr(tileset_ref, "source");
    xml_node_t *ts = tileset_ref; // node that actually holds the tileset fields
    xml_node_t *loaded_tsx = NULL;

    if (source)
    {
        char tsxpath[FILENAME_MAX];
        snprintf(tsxpath, sizeof(tsxpath), "%s%s", tmx_dir, source);
        if (!isquiet) info("Loading external tileset [%s]...", tsxpath);
        loaded_tsx = xml_parse_file(tsxpath);
        if (!loaded_tsx) fatal("failed to parse external tileset [%s]", tsxpath);
        ts = loaded_tsx;
    }

    jb_fmt(jb, "{\"firstgid\":%d", firstgid);
    jb_raw(jb, ",\"name\":"); jb_str(jb, xml_attr_default(ts, "name", ""));
    jb_fmt(jb, ",\"tilewidth\":%d,\"tileheight\":%d,\"tilecount\":%d,\"columns\":%d,\"margin\":%d,\"spacing\":%d",
           xml_attr_int(ts, "tilewidth", 0), xml_attr_int(ts, "tileheight", 0),
           xml_attr_int(ts, "tilecount", 0), xml_attr_int(ts, "columns", 0),
           xml_attr_int(ts, "margin", 0), xml_attr_int(ts, "spacing", 0));

    xml_node_t *image = xml_child(ts, "image");
    if (image)
    {
        jb_raw(jb, ",\"image\":"); jb_str(jb, xml_attr_default(image, "source", ""));
        jb_fmt(jb, ",\"imagewidth\":%d,\"imageheight\":%d",
               xml_attr_int(image, "width", 0), xml_attr_int(image, "height", 0));
    }

    emit_properties(jb, ts);

    xml_node_t *first_tile = xml_child(ts, "tile");
    if (first_tile)
    {
        jb_raw(jb, ",\"tiles\":[");
        int first = 1;
        for (xml_node_t *tile = first_tile; tile; tile = xml_child_next(ts, tile, "tile"))
        {
            emit_tile_descriptor(jb, tile, &first);
        }
        jb_raw(jb, "]");
    }

    jb_raw(jb, "}");

    if (loaded_tsx) xml_free(loaded_tsx);
}

//-------------------------------------------------------------------------------------------------
static void emit_object(jbuf_t *jb, xml_node_t *obj, int *first)
{
    if (!*first) jb_raw(jb, ",");
    *first = 0;

    const char *type = xml_attr(obj, "type");
    if (!type) type = xml_attr(obj, "class");
    if (!type) type = "";

    jb_fmt(jb, "{\"id\":%d,\"name\":", xml_attr_int(obj, "id", 0));
    jb_str(jb, xml_attr_default(obj, "name", ""));
    jb_raw(jb, ",\"type\":");
    jb_str(jb, type);
    jb_fmt(jb, ",\"x\":%g,\"y\":%g,\"width\":%g,\"height\":%g,\"rotation\":%g,\"visible\":%s",
           xml_attr_float(obj, "x", 0), xml_attr_float(obj, "y", 0),
           xml_attr_float(obj, "width", 0), xml_attr_float(obj, "height", 0),
           xml_attr_float(obj, "rotation", 0),
           xml_attr_int(obj, "visible", 1) ? "true" : "false");

    const char *gid = xml_attr(obj, "gid");
    if (gid) jb_fmt(jb, ",\"gid\":%s", gid);

    emit_properties(jb, obj);
    jb_raw(jb, "}");
}

static void emit_layer(jbuf_t *jb, xml_node_t *layer, const char *tag, int *first)
{
    if (!*first) jb_raw(jb, ",");
    *first = 0;

    jb_fmt(jb, "{\"id\":%d,\"name\":", xml_attr_int(layer, "id", 0));
    jb_str(jb, xml_attr_default(layer, "name", ""));
    jb_fmt(jb, ",\"x\":%d,\"y\":%d,\"opacity\":%g,\"visible\":%s",
           xml_attr_int(layer, "x", 0), xml_attr_int(layer, "y", 0),
           xml_attr_float(layer, "opacity", 1.0f),
           xml_attr_int(layer, "visible", 1) ? "true" : "false");

    if (strcmp(tag, "layer") == 0)
    {
        int width = xml_attr_int(layer, "width", 0);
        int height = xml_attr_int(layer, "height", 0);
        jb_raw(jb, ",\"type\":\"tilelayer\"");
        jb_fmt(jb, ",\"width\":%d,\"height\":%d", width, height);

        xml_node_t *data_node = xml_child(layer, "data");
        int count = 0;
        uint32_t *gids = data_node ? read_layer_data(data_node, &count) : NULL;

        jb_raw(jb, ",\"data\":[");
        for (int i = 0; i < count; i++)
        {
            if (i) jb_raw(jb, ",");
            jb_fmt(jb, "%u", gids[i]); // unsigned: preserves flip-flag bit pattern, matches Tiled's own JSON export
        }
        jb_raw(jb, "]");
        free(gids);
    }
    else // objectgroup
    {
        jb_raw(jb, ",\"type\":\"objectgroup\"");
        jb_raw(jb, ",\"objects\":[");
        int ofirst = 1;
        for (xml_node_t *obj = xml_child(layer, "object"); obj; obj = xml_child_next(layer, obj, "object"))
        {
            emit_object(jb, obj, &ofirst);
        }
        jb_raw(jb, "]");
    }

    emit_properties(jb, layer);
    jb_raw(jb, "}");
}

//-------------------------------------------------------------------------------------------------
char *tmx_convert_to_json(const char *tmxpath, bool isquiet)
{
    xml_node_t *root;
    jbuf_t jb;
    char tmx_dir[FILENAME_MAX] = "";

    if (!isquiet) info("Parsing native tmx file [%s]...", tmxpath);

    root = xml_parse_file(tmxpath);
    if (!root || strcmp(root->tag, "map") != 0)
    {
        fatal("[%s] does not look like a valid Tiled .tmx map (missing <map> root element)", tmxpath);
    }

    // directory part of tmxpath, used to resolve external .tsx references
    {
        const char *slash = strrchr(tmxpath, '/');
        const char *bslash = strrchr(tmxpath, '\\');
        const char *last = slash > bslash ? slash : bslash;
        if (last) snprintf(tmx_dir, sizeof(tmx_dir), "%.*s", (int)(last - tmxpath + 1), tmxpath);
    }

    if (xml_attr_int(root, "infinite", 0))
    {
        fatal("infinite maps are not supported by tmx2snes -- disable 'Infinite map' in Tiled's map properties");
    }

    jb_init(&jb);
    jb_raw(&jb, "{\"type\":\"map\"");
    jb_raw(&jb, ",\"version\":"); jb_str(&jb, xml_attr_default(root, "version", "1.10"));
    jb_raw(&jb, ",\"tiledversion\":"); jb_str(&jb, xml_attr_default(root, "tiledversion", ""));
    jb_raw(&jb, ",\"orientation\":"); jb_str(&jb, xml_attr_default(root, "orientation", "orthogonal"));
    jb_raw(&jb, ",\"renderorder\":"); jb_str(&jb, xml_attr_default(root, "renderorder", "right-down"));
    jb_fmt(&jb, ",\"width\":%d,\"height\":%d,\"tilewidth\":%d,\"tileheight\":%d",
           xml_attr_int(root, "width", 0), xml_attr_int(root, "height", 0),
           xml_attr_int(root, "tilewidth", 0), xml_attr_int(root, "tileheight", 0));
    jb_raw(&jb, ",\"infinite\":false");
    jb_fmt(&jb, ",\"nextlayerid\":%d,\"nextobjectid\":%d",
           xml_attr_int(root, "nextlayerid", 1), xml_attr_int(root, "nextobjectid", 1));

    emit_properties(&jb, root);

    jb_raw(&jb, ",\"tilesets\":[");
    {
        int first = 1;
        for (xml_node_t *ts = xml_child(root, "tileset"); ts; ts = xml_child_next(root, ts, "tileset"))
        {
            if (!first) jb_raw(&jb, ",");
            first = 0;
            emit_tileset(&jb, ts, tmx_dir, isquiet);
        }
    }
    jb_raw(&jb, "]");

    jb_raw(&jb, ",\"layers\":[");
    {
        int first = 1;
        for (xml_node_t *child = root->children; child; child = child->next)
        {
            if (strcmp(child->tag, "layer") == 0 || strcmp(child->tag, "objectgroup") == 0)
            {
                emit_layer(&jb, child, child->tag, &first);
            }
            else if (strcmp(child->tag, "group") == 0 || strcmp(child->tag, "imagelayer") == 0)
            {
                warning("layer '%s' of type <%s> is not supported by tmx2snes and will be skipped",
                        xml_attr_default(child, "name", "?"), child->tag);
            }
        }
    }
    jb_raw(&jb, "]");

    jb_raw(&jb, "}");

    xml_free(root);

    return jb.data;
}
