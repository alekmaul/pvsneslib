/*
	Node allocation
*/

#include <string.h>

#include <libxml/xmlmemory.h>

#include "tmx.h"
#include "tmx_utils.h"

void set_alloc_functions() {
	if (!tmx_alloc_func) tmx_alloc_func = realloc;
	if (!tmx_free_func) tmx_free_func = free;
}

static void* tmx_malloc(size_t len) {
	return tmx_alloc_func(NULL, len);
}

void setup_libxml_mem() {
	xmlMemSetup((xmlFreeFunc)tmx_free_func, (xmlMallocFunc)tmx_malloc, (xmlReallocFunc)tmx_alloc_func, (xmlStrdupFunc)tmx_strdup);
}

static void* node_alloc(size_t size) {
	void *res = tmx_alloc_func(NULL, size);
	if (res) {
		memset(res, 0, size);
	} else {
		tmx_errno = E_ALLOC;
	}
	return res;
}

tmx_property* alloc_prop(void) {
	return (tmx_property*)node_alloc(sizeof(tmx_property));
}

tmx_image* alloc_image(void) {
	return (tmx_image*)node_alloc(sizeof(tmx_image));
}

tmx_shape* alloc_shape(void) {
	return (tmx_shape*)node_alloc(sizeof(tmx_shape));
}

tmx_text* alloc_text(void) {
	tmx_text *res = (tmx_text*)node_alloc(sizeof(tmx_text));
	if (res) {
		res->pixelsize = 16;
		res->kerning = 1;
		res->valign = VA_TOP;
		res->halign = HA_LEFT;
	}
	return res;
}

tmx_object* alloc_object(void) {
	tmx_object *res = (tmx_object*)node_alloc(sizeof(tmx_object));
	if (res) {
		res->visible = 1;
	}
	return res;
}

tmx_object_group* alloc_objgr(void) {
	return (tmx_object_group*)node_alloc(sizeof(tmx_object_group));
}

tmx_layer* alloc_layer(void) {
	tmx_layer *res = (tmx_layer*)node_alloc(sizeof(tmx_layer));
	if (res) {
		res->opacity = 1.0f;
		res->visible = 1;
		res->tintcolor = 0xFFFFFFFF;
	}
	return res;
}

tmx_tile* alloc_tiles(int count) {
	return (tmx_tile*)node_alloc(count * sizeof(tmx_tile));
}

tmx_tileset* alloc_tileset(void) {
	return (tmx_tileset*)node_alloc(sizeof(tmx_tileset));
}

tmx_tileset_list* alloc_tileset_list(void) {
	return (tmx_tileset_list*)node_alloc(sizeof(tmx_tileset_list));
}

tmx_template* alloc_template(void) {
	tmx_template* res = (tmx_template*)node_alloc(sizeof(tmx_template));
	res->object = alloc_object();
	return res;
}

tmx_map* alloc_map(void) {
	return (tmx_map*)node_alloc(sizeof(tmx_map));
}

resource_holder* pack_tileset_resource(tmx_tileset *value) {
	resource_holder *res = node_alloc(sizeof(resource_holder));
	if (res) {
		res->type = RC_TSX;
		res->resource.tileset = value;
	}
	return res;
}

resource_holder* pack_template_resource(tmx_template *value) {
	resource_holder *res = node_alloc(sizeof(resource_holder));
	if (res) {
		res->type = RC_TX;
		res->resource.template = value;
	}
	return res;
}

/*
	Node free
*/

void free_property(tmx_property *p) {
	if (p) {
		tmx_free_func(p->name);
		if (p->type == PT_STRING || p->type == PT_FILE || p->type == PT_NONE) {
			tmx_free_func(p->value.string);
		}
		tmx_free_func(p);
	}
}

void free_props(tmx_properties *h) {
	free_hashtable((void*)h, property_deallocator);
}

void free_obj(tmx_object *o) {
	if (o) {
		free_obj(o->next);
		tmx_free_func(o->name);
		if (o->obj_type == OT_POLYGON || o->obj_type == OT_POLYLINE) {
			if (o->content.shape) {
				if (o->content.shape->points) {
					tmx_free_func(*(o->content.shape->points));
					tmx_free_func(o->content.shape->points);
				}
				tmx_free_func(o->content.shape);
			}
		}
		else if (o->obj_type == OT_TEXT) {
			if (o->content.text) {
				if (o->content.text->fontfamily) tmx_free_func(o->content.text->fontfamily);
				if (o->content.text->text) tmx_free_func(o->content.text->text);
				tmx_free_func(o->content.text);
			}
		}
		tmx_free_func(o->type);
		free_props(o->properties);
		if (o->template_ref && o->template_ref->is_embedded) {
			free_template(o->template_ref);
		}
		tmx_free_func(o);
	}
}

void free_objgr(tmx_object_group *o) {
	if (o) {
		free_obj(o->head);
		tmx_free_func(o);
	}
}

void free_image(tmx_image *i) {
	if (i) {
		tmx_free_func(i->source);
		if (tmx_img_free_func) {
			tmx_img_free_func(i->resource_image);
		}
		tmx_free_func(i);
	}
}

void free_layers(tmx_layer *l) {
	if (l) {
		free_layers(l->next);
		tmx_free_func(l->name);
		if (l->type == L_LAYER) {
			tmx_free_func(l->content.gids);
		}
		else if (l->type == L_OBJGR) {
			free_objgr(l->content.objgr);
		}
		else if (l->type == L_IMAGE) {
			free_image(l->content.image);
		}
		else if (l->type == L_GROUP) {
			free_layers(l->content.group_head);
		}
		free_props(l->properties);
		tmx_free_func(l);
	}
}

void free_tiles(tmx_tile *t, int tilecount) {
	int i;
	if (t) {
		for (i=0; i<tilecount; i++) {
			free_props(t[i].properties);
			free_image(t[i].image);
			free_obj(t[i].collision);
			tmx_free_func(t[i].animation);
			tmx_free_func(t[i].type);
		}
	}
}

void free_ts(tmx_tileset *ts) {
	if (ts) {
		tmx_free_func(ts->name);
		free_image(ts->image);
		free_props(ts->properties);
		free_tiles(ts->tiles, ts->tilecount);
		tmx_free_func(ts->tiles);
		tmx_free_func(ts);
	}
}

void free_ts_list(tmx_tileset_list *tsl) {
	if (tsl) {
		free_ts_list(tsl->next);
		if (tsl->is_embedded) {
			free_ts(tsl->tileset);
		}
		tmx_free_func(tsl->source);
		tmx_free_func(tsl);
	}
}

void free_template(tmx_template *tmpl) {
	if (tmpl) {
		free_ts_list(tmpl->tileset_ref);
		free_obj(tmpl->object);
	}
	tmx_free_func(tmpl);
}

void property_deallocator(void *val, const char *key UNUSED) {
	free_property((tmx_property*)val);
}

void resource_deallocator(void *val, const char *key UNUSED) {
	resource_holder *rc_holder;
	if (val) {
		rc_holder = (resource_holder*)val;
		if (rc_holder->type == RC_TSX)
			free_ts(rc_holder->resource.tileset);
		else if (rc_holder->type == RC_TX)
			free_template(rc_holder->resource.template);
		tmx_free_func(val);
	}
}
