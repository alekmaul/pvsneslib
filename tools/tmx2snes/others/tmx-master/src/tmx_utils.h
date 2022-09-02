/* Private Header */

#pragma once

#include "tmx.h"

#ifndef TMXUTILS_H
#define TMXUTILS_H

/* UNUSED macro to suppress `unused parameter` warnings with GCC and CLANG */
#ifdef __GNUC__
#define UNUSED __attribute__((__unused__))
#else
#define UNUSED
#endif

/*
	Resource holder type an deallocator - tmx_rc.c
*/
enum resource_type { RC_TSX, RC_TX };
typedef struct _rc_holder {
	enum resource_type type;
	union {
		tmx_tileset  *tileset;
		tmx_template *template;
	} resource;
} resource_holder;
int add_tileset(tmx_resource_manager *rc_mgr, const char *key, tmx_tileset *value);
int add_template(tmx_resource_manager *rc_mgr, const char *key, tmx_template *value);

/*
	XML Parser implementation - tmx_xml.c
*/
tmx_map* parse_xml(tmx_resource_manager *rc_mgr, const char *filename);
tmx_map* parse_xml_buffer(tmx_resource_manager *rc_mgr, const char *buffer, int len);
tmx_map* parse_xml_fd(tmx_resource_manager *rc_mgr, int fd);
tmx_map* parse_xml_callback(tmx_resource_manager *rc_mgr, tmx_read_functor callback, void *userdata);

tmx_tileset* parse_tsx_xml(const char *filename);
tmx_tileset* parse_tsx_xml_buffer(const char *buffer, int len);
tmx_tileset* parse_tsx_xml_fd(int fd);
tmx_tileset* parse_tsx_xml_callback(tmx_read_functor callback, void *userdata);

tmx_template* parse_tx_xml(tmx_resource_manager *rc_mgr, const char *filename);
tmx_template* parse_tx_xml_buffer(tmx_resource_manager *rc_mgr, const char *buffer, int len);
tmx_template* parse_tx_xml_fd(tmx_resource_manager *rc_mgr, int fd);
tmx_template* parse_tx_xml_callback(tmx_resource_manager *rc_mgr, tmx_read_functor callback, void *userdata);

/*
	Memory management, node allocation and free - tmx_mem.c
*/
void set_alloc_functions();
void setup_libxml_mem();

tmx_property*     alloc_prop(void);
tmx_image*        alloc_image(void);
tmx_shape*        alloc_shape(void);
tmx_text*         alloc_text(void);
tmx_object*       alloc_object(void);
tmx_object_group* alloc_objgr(void);
tmx_layer*        alloc_layer(void);
tmx_tile*         alloc_tiles(int count);
tmx_tileset*      alloc_tileset(void);
tmx_tileset_list* alloc_tileset_list(void);
tmx_template*     alloc_template(void);
tmx_map*          alloc_map(void);

resource_holder* pack_tileset_resource(tmx_tileset *value);
resource_holder* pack_template_resource(tmx_template *value);

void free_property(tmx_property *p);
void free_props(tmx_properties *h);
void free_obj(tmx_object *o);
void free_objgr(tmx_object_group *o);
void free_image(tmx_image *i);
void free_layers(tmx_layer *l);
void free_tiles(tmx_tile *t, int tilecount);
void free_ts(tmx_tileset *ts);
void free_ts_list(tmx_tileset_list *tsl);
void free_template(tmx_template *tmpl);

void resource_deallocator(void *val, const char *key);
void property_deallocator(void *val, const char *key);

/*
	Misc - tmx_utils.c
*/
#define MAX(a,b) (a<b) ? b: a;

enum enccmp_t { CSV, B64Z, B64, B64ZSTD };
int data_decode(const char *source, enum enccmp_t type, size_t gids_count, uint32_t **gids);

void map_post_parsing(tmx_map **map);
int set_tiles_runtime_props(tmx_tileset *ts);
int mk_map_tile_array(tmx_map *map);

enum tmx_map_orient parse_orient(const char *orient_str);
enum tmx_map_renderorder parse_renderorder(const char *renderorder);
enum tmx_obj_alignment parse_obj_alignment(const char *objalign_str);
enum tmx_objgr_draworder parse_objgr_draworder(const char *draworder);
enum tmx_stagger_index parse_stagger_index(const char *staggerindex);
enum tmx_stagger_axis parse_stagger_axis(const char *staggeraxis);
enum tmx_property_type parse_property_type(const char *propertytype);
enum tmx_horizontal_align parse_horizontal_align(const char *horalign);
enum tmx_vertical_align parse_vertical_align(const char *veralign);
enum tmx_layer_type parse_layer_type(const char *nodename);
int parse_boolean(const char *boolean);
uint32_t get_color_rgb(const char *c);

int count_char_occurences(const char *str, char c);
char* str_trim(char *str);
char* tmx_strdup(const char *str);

size_t dirpath_len(const char *str);
char* mk_absolute_path(const char *base_path, const char *rel_path);
void* load_image(void **ptr, const char *base_path, const char *rel_path);

/*
	Hashtable - tmx_hash.c
*/
typedef void (*hashtable_entry_deallocator)(void *val, const char *key);
typedef void (*hashtable_foreach_functor)(void *val, void *userdata, const char *key);

void* mk_hashtable(unsigned int initial_size);
void  hashtable_set(void *hashtable, const char *key, void *val, hashtable_entry_deallocator deallocator);
void* hashtable_get(void *hashtable, const char *key);
void  hashtable_rm(void *hashtable, const char *key, hashtable_entry_deallocator deallocator);
void  hashtable_foreach(void *hashtable, hashtable_foreach_functor functor, void *userdata);
void  free_hashtable(void *hashtable, hashtable_entry_deallocator deallocator);

/*
	Error handling - tmx_err.c
*/
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#define snprintf _snprintf
#endif

extern char _tmx_custom_msg[256];
#define tmx_err(code, ...) tmx_errno = code; snprintf(_tmx_custom_msg, 256, __VA_ARGS__)

#endif /* TMXUTILS_H */
