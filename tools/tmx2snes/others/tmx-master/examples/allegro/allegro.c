#include <stdio.h>
#include <tmx.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define DISPLAY_H 600
#define DISPLAY_W 800

void* Allegro5_tex_loader(const char *path) {
	ALLEGRO_BITMAP *res    = NULL;
	ALLEGRO_PATH   *alpath = NULL;

	if (!(alpath = al_create_path(path))) return NULL;

	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
	res = al_load_bitmap(al_path_cstr(alpath, ALLEGRO_NATIVE_PATH_SEP));

	al_destroy_path(alpath);

	return (void*)res;
}

ALLEGRO_COLOR int_to_al_color(int color) {
	tmx_col_floats res = tmx_col_to_floats(color);
	return *((ALLEGRO_COLOR*)&res);
}

#define LINE_THICKNESS 2.5

void draw_polyline(double **points, double x, double y, int pointsc, ALLEGRO_COLOR color) {
	int i;
	for (i=1; i<pointsc; i++) {
		al_draw_line(x+points[i-1][0], y+points[i-1][1], x+points[i][0], y+points[i][1], color, LINE_THICKNESS);
	}
}

void draw_polygone(double **points, double x, double y, int pointsc, ALLEGRO_COLOR color) {
	draw_polyline(points, x, y, pointsc, color);
	if (pointsc > 2) {
		al_draw_line(x+points[0][0], y+points[0][1], x+points[pointsc-1][0], y+points[pointsc-1][1], color, LINE_THICKNESS);
	}
}

void draw_objects(tmx_object_group *objgr) {
	ALLEGRO_COLOR color = int_to_al_color(objgr->color);
	tmx_object *head = objgr->head;
	while (head) {
		if (head->visible) {
			if (head->obj_type == OT_SQUARE) {
				al_draw_rectangle(head->x, head->y, head->x+head->width, head->y+head->height, color, LINE_THICKNESS);
			}
			else if (head->obj_type  == OT_POLYGON) {
				draw_polygone(head->content.shape->points, head->x, head->y, head->content.shape->points_len, color);
			}
			else if (head->obj_type == OT_POLYLINE) {
				draw_polyline(head->content.shape->points, head->x, head->y, head->content.shape->points_len, color);
			}
			else if (head->obj_type == OT_ELLIPSE) {
				al_draw_ellipse(head->x + head->width/2.0, head->y + head->height/2.0, head->width/2.0, head->height/2.0, color, LINE_THICKNESS);
			}
		}
		head = head->next;
	}
}

void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags) {
	ALLEGRO_COLOR colour = al_map_rgba_f(opacity, opacity, opacity, opacity);
	al_draw_tinted_bitmap_region((ALLEGRO_BITMAP*)image, colour, sx, sy, sw, sh, dx, dy, flags);
}

void draw_layer(tmx_map *map, tmx_layer *layer) {
	unsigned long i, j;
	unsigned int gid, x, y, w, h, flags;
	float op;
	tmx_tileset *ts;
	tmx_image *im;
	void* image;
	op = layer->opacity;
	for (i=0; i<map->height; i++) {
		for (j=0; j<map->width; j++) {
			gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
			if (map->tiles[gid] != NULL) {
				ts = map->tiles[gid]->tileset;
				im = map->tiles[gid]->image;
				x  = map->tiles[gid]->ul_x;
				y  = map->tiles[gid]->ul_y;
				w  = ts->tile_width;
				h  = ts->tile_height;
				if (im) {
					image = im->resource_image;
				}
				else {
					image = ts->image->resource_image;
				}
				flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
				draw_tile(image, x, y, w, h, j*ts->tile_width, i*ts->tile_height, op, flags);
			}
		}
	}
}

void draw_image_layer(tmx_image *image) {
	ALLEGRO_BITMAP *bitmap = (ALLEGRO_BITMAP*)image->resource_image;
	al_draw_bitmap(bitmap, 0, 0, 0);
}

void draw_all_layers(tmx_map *map, tmx_layer *layers) {
	while (layers) {
		if (layers->visible) {

			if (layers->type == L_GROUP) {
				draw_all_layers(map, layers->content.group_head); // recursive call
			}
			else if (layers->type == L_OBJGR) {
				draw_objects(layers->content.objgr);
			}
			else if (layers->type == L_IMAGE) {
				draw_image_layer(layers->content.image);
			}
			else if (layers->type == L_LAYER) {
				draw_layer(map, layers);
			}
		}
		layers = layers->next;
	}
}

void render_map(tmx_map *map) {
	al_clear_to_color(int_to_al_color(map->backgroundcolor));
	draw_all_layers(map, map->ly_head);
}

int main(int argc, char **argv) {
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *equeue = NULL;
	ALLEGRO_EVENT ev;

	if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon()) {
		fputs("Cannot initialise the Allegro library", stderr);
		return 1;
	}

	display = al_create_display(DISPLAY_W, DISPLAY_H);
	if (!display) {
		fputs("Cannot create a display", stderr);
		return 1;
	}
	al_set_window_title(display, "Allegro example");

	equeue = al_create_event_queue();
	if (!equeue) {
		fputs("Cannot create an event queue", stderr);
		return 1;
	}

	timer = al_create_timer(1.0/30.0);
	if (!timer) {
		fputs("Cannot create a timer", stderr);
		return 1;
	}

	al_register_event_source(equeue, al_get_display_event_source(display));
	al_register_event_source(equeue, al_get_timer_event_source(timer));
	al_start_timer(timer);

	/* Set the callback globs in the main function */
	tmx_img_load_func = Allegro5_tex_loader;
	tmx_img_free_func = (void (*)(void*))al_destroy_bitmap;

	tmx_map *map = tmx_load(argv[1]);
	if (map == NULL) {
		tmx_perror("Cannot load map");
		return 1;
	}

	while (al_wait_for_event(equeue, &ev), 1) {

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;

		render_map(map);
		al_flip_display();
	}

	tmx_map_free(map);

	al_destroy_timer(timer);
	al_destroy_event_queue(equeue);
	al_destroy_display(display);

	return 0;
}