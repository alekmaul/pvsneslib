#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <tmx.h>

#define str_bool(b) (b==0? "false": "true")

void print_orient(enum tmx_map_orient orient) {
	switch(orient) {
		case O_NONE: printf("none");  break;
		case O_ORT:  printf("ortho"); break;
		case O_ISO:  printf("isome"); break;
		case O_STA:  printf("stagg"); break;
		case O_HEX:  printf("hexag"); break;
		default: printf("unknown");
	}
}

void print_stagger_index(enum tmx_stagger_index index) {
	switch(index) {
		case SI_NONE: printf("none"); break;
		case SI_EVEN: printf("even"); break;
		case SI_ODD:  printf("odd");  break;
		default: printf("unknown");
	}
}

void print_stagger_axis(enum tmx_stagger_axis axis) {
	switch(axis) {
		case SA_NONE: printf("none"); break;
		case SA_X:    printf("x");    break;
		case SA_Y:    printf("y");    break;
		default: printf("unknown");
	}
}

void print_renderorder(enum tmx_map_renderorder ro) {
	switch(ro) {
		case R_NONE:      printf("none");      break;
		case R_RIGHTDOWN: printf("rightdown"); break;
		case R_RIGHTUP:   printf("rightup");   break;
		case R_LEFTDOWN:  printf("leftdown");  break;
		case R_LEFTUP:    printf("leftup");    break;
		default: printf("unknown");
	}
}

void print_objectalignment(enum tmx_obj_alignment oa) {
	switch (oa) {
		case OA_NONE:        printf("unspecified"); break;
		case OA_TOP:         printf("top");         break;
		case OA_LEFT:        printf("left");        break;
		case OA_BOTTOM:      printf("bottom");      break;
		case OA_RIGHT:       printf("right");       break;
		case OA_CENTER:      printf("center");      break;
		case OA_TOPLEFT:     printf("topleft");     break;
		case OA_TOPRIGHT:    printf("topright");    break;
		case OA_BOTTOMLEFT:  printf("bottomleft");  break;
		case OA_BOTTOMRIGHT: printf("bottomright"); break;
		default: printf("unknown");
	}
}

void print_draworder(enum tmx_objgr_draworder dro) {
	switch(dro) {
		case G_NONE:    printf("none");    break;
		case G_INDEX:   printf("index");   break;
		case G_TOPDOWN: printf("topdown"); break;
		default: printf("unknown");
	}
}

void print_halign(enum tmx_horizontal_align halign) {
	switch(halign) {
		case HA_NONE:   printf("none");   break;
		case HA_LEFT:   printf("left");   break;
		case HA_CENTER: printf("center"); break;
		case HA_RIGHT:  printf("right");  break;
		default: printf("unknown");
	}
}

void print_valign(enum tmx_vertical_align valign) {
	switch(valign) {
		case VA_NONE:   printf("none");   break;
		case VA_TOP:    printf("top");    break;
		case VA_CENTER: printf("center"); break;
		case VA_BOTTOM: printf("bottom"); break;
		default: printf("unknown");
	}
}

void mk_padding(char pad[11], int depth) {
	if (depth>10) depth=10;
	if (depth>0) memset(pad, '\t', depth);
	pad[depth] = '\0';
}

void print_prop(tmx_property *p, void *depth) {
	char padding[12]; mk_padding(padding, (int)(uintptr_t)depth);

	printf("\n%s" "'%s'=(", padding, p->name);
	switch(p->type) {
		case PT_NONE:   printf("none");    break;
		case PT_INT:    printf("integer"); break;
		case PT_FLOAT:  printf("float");   break;
		case PT_BOOL:   printf("bool");    break;
		case PT_STRING: printf("string");  break;
		case PT_COLOR:  printf("color");   break;
		case PT_FILE:   printf("file");    break;
		default: printf("unknown");
	}
	printf(")");
	switch(p->type) {
		case PT_INT:    printf("%d", p->value.integer); break;
		case PT_FLOAT:  printf("%f", p->value.decimal); break;
		case PT_BOOL:   printf(p->value.integer? "true": "false"); break;
		case PT_COLOR:  printf("#%.6X", p->value.color); break;
		case PT_NONE:
		case PT_STRING:
		case PT_FILE:
		default:        printf("'%s'", p->value.string); break;
	}
}

void dump_prop(tmx_properties *p, int depth) {
	char padding[11]; mk_padding(padding, depth);

	printf("\n%s" "properties={", padding);
	if (!p) {
		printf(" (NULL) }");
	} else {
		tmx_property_foreach(p, print_prop, (void*)(long)(depth+1));
		printf("\n" "%s}", padding);
	}
}

void print_obj_type(enum tmx_obj_type type) {
	switch(type) {
		case OT_NONE:     printf("none");     break;
		case OT_TILE:     printf("tile");     break;
		case OT_TEXT:     printf("text");     break;
		case OT_POINT:    printf("point");    break;
		case OT_SQUARE:   printf("square");   break;
		case OT_ELLIPSE:  printf("ellipse");  break;
		case OT_POLYGON:  printf("polygon");  break;
		case OT_POLYLINE: printf("polyline"); break;
		default: printf("unknown");
	}
}

void dump_points(double **p, int pl) {
	int i;
	for (i=0; i<pl; i++) {
		printf(" (%f, %f)", p[i][0], p[i][1]);
	}
}

void dump_objects(tmx_object *o, int depth);
void dump_tileset(tmx_tileset_list *tsl, int depth);

void dump_template(tmx_template *t, int depth) {
	char padding[11]; mk_padding(padding, depth);

	printf("\n%s" "template={", padding);
	if (!t) {
		printf(" (NULL) }");
	} else {
		if (t->tileset_ref) {
			dump_tileset(t->tileset_ref, depth+1);
		}
		dump_objects(t->object, depth+1);
		printf("\n%s}", padding);
	}
}

void dump_objects(tmx_object *o, int depth) {
	char padding[11]; mk_padding(padding, depth);

	printf("\n%s" "object={", padding);
	if (!o) {
		printf(" (NULL) }");
	} else {
		printf("\n%s\t" "id=%u", padding, o->id);
		printf("\n%s\t" "name='%s'", padding, o->name);
		printf("\n%s\t" "type='%s'", padding, o->type);
		printf("\n%s\t" "obj_type=", padding);  print_obj_type(o->obj_type);
		printf("\n%s\t" "x=%f", padding, o->x);
		printf("\n%s\t" "y=%f", padding, o->y);
		printf("\n%s\t" "rotation=%f", padding, o->rotation);
		printf("\n%s\t" "visible=%s", padding, str_bool(o->visible));
		if (o->obj_type == OT_POLYGON || o->obj_type == OT_POLYLINE) {
			printf("\n%s\t" "number of points='%d'", padding, o->content.shape->points_len);
			printf("\n%s\t" "points=", padding);
			dump_points(o->content.shape->points, o->content.shape->points_len);
		}
		else if (o->obj_type == OT_TEXT) {
			tmx_text *t = o->content.text;
			printf("\n%s\t" "fontfamily='%s'", padding, t->fontfamily);
			printf("\n%s\t" "pixelsize=%d", padding, t->pixelsize);
			printf("\n%s\t" "color=#%.6X", padding, t->color);
			printf("\n%s\t" "wrap=%s", padding, str_bool(t->wrap));
			printf("\n%s\t" "bold=%s", padding, str_bool(t->bold));
			printf("\n%s\t" "italic=%s", padding, str_bool(t->italic));
			printf("\n%s\t" "underline=%s", padding, str_bool(t->underline));
			printf("\n%s\t" "strikeout=%s", padding, str_bool(t->strikeout));
			printf("\n%s\t" "kerning=%s", padding, str_bool(t->kerning));
			printf("\n%s\t" "halign=", padding); print_halign(t->halign);
			printf("\n%s\t" "valign=", padding); print_valign(t->valign);
			printf("\n%s\t" "text='%s'", padding, t->text);
		}
		dump_template(o->template_ref, depth+1);
		dump_prop(o->properties, depth+1);
		printf("\n%s}", padding);
	}

	if (o && o->next) {
		dump_objects(o->next, depth);
	}
}

void dump_image(tmx_image *i, int depth) {
	char padding[11]; mk_padding(padding, depth);

	printf("\n%s" "image={", padding);
	if (i) {
		printf("\n%s\t" "source='%s'", padding, i->source);
		printf("\n%s\t" "height=%lu", padding, i->height);
		printf("\n%s\t" "width=%lu", padding, i->width);
		printf("\n%s\t" "uses_trans=%s", padding, str_bool(i->uses_trans));
		printf("\n%s\t" "trans=#%.6X", padding, i->trans);
		printf("\n%s}", padding);
	} else {
		printf(" (NULL) }");
	}
}

void dump_tile(tmx_tile *t, unsigned int tilecount, int depth) {
	unsigned int i, j;
	char padding[11]; mk_padding(padding, depth);
	for (i=0; i<tilecount; i++) {
		printf("\n%s" "tile={", padding);
		printf("\n%s\t" "id=%u", padding, t[i].id);
		printf("\n%s\t" "upper-left=(%u,%u)", padding, t[i].ul_x, t[i].ul_y);
		printf("\n%s\t" "type='%s'", padding, t[i].type);
		dump_image(t[i].image, depth+1);
		dump_prop(t[i].properties, depth+1);
		dump_objects(t[i].collision, depth+1);

		if (t[i].animation) {
			printf("\n%s\t" "animation={", padding);
			for (j=0; j<t[i].animation_len; j++) {
				printf("\n%s\t\t" "tile=%3u (%6ums)", padding, t[i].animation[j].tile_id, t[i].animation[j].duration);
			}
			printf("\n%s\t}", padding);
		}

		printf("\n%s}", padding);
	}
}

void dump_tileset(tmx_tileset_list *tsl, int depth) {
	char padding[11]; mk_padding(padding, depth);
	if (tsl) {
		tmx_tileset *t = tsl->tileset;
		printf("\n%stileset={", padding);
		if (t) {
			printf("\n%s\t" "firstgid=%u", padding, tsl->firstgid);
			printf("\n%s\t" "is_embedded=%i", padding, tsl->is_embedded);
			printf("\n%s\t" "source=%s", padding, tsl->source);
			printf("\n%s\t" "name=%s", padding, t->name);
			printf("\n%s\t" "tilecount=%u", padding, t->tilecount);
			printf("\n%s\t" "tile_height=%u", padding, t->tile_height);
			printf("\n%s\t" "tile_width=%u", padding, t->tile_width);
			printf("\n%s\t" "margin=%u", padding, t->margin);
			printf("\n%s\t" "spacing=%u", padding, t->spacing);
			printf("\n%s\t" "x_offset=%d", padding, t->x_offset);
			printf("\n%s\t" "y_offset=%d", padding, t->y_offset);
			printf("\n%s\t" "objectalignment=", padding); print_objectalignment(t->objectalignment);
			dump_image(t->image, depth+1);
			dump_tile(t->tiles, t->tilecount, depth+1);
			dump_prop(t->properties, depth+1);
			printf("\n%s}", padding);
		} else {
			printf(" (NULL) }");
		}

		if (tsl->next) {
			dump_tileset(tsl->next, depth);
		}
	}
}

void dump_layer(tmx_layer *l, unsigned int tc, int depth) {
	unsigned int i;
	char padding[11]; mk_padding(padding, depth);

	printf("\n%slayer={", padding);
	if (!l) {
		printf("%s\t (NULL) }", padding);
	} else {
		printf("\n%s\t" "id='%d'", padding, l->id);
		printf("\n%s\t" "name='%s'", padding, l->name);
		printf("\n%s\t" "visible=%s", padding, str_bool(l->visible));
		printf("\n%s\t" "opacity='%f'", padding, l->opacity);
		printf("\n%s\t" "offsetx=%d", padding, l->offsetx);
		printf("\n%s\t" "offsety=%d", padding, l->offsety);
		printf("\n%s\t" "parallaxx=%d", padding, l->parallaxx);
		printf("\n%s\t" "parallaxy=%d", padding, l->parallaxy);
		printf("\n%s\t" "tintcolor=#%.6X", padding, l->tintcolor);
		if (l->type == L_LAYER && l->content.gids) {
			printf("\n%s\t" "type=Layer" "\n%s\t" "tiles=", padding, padding);
			for (i=0; i<tc; i++) {
				printf("%u,", l->content.gids[i] & TMX_FLIP_BITS_REMOVAL);
			}
		} else if (l->type == L_OBJGR) {
			printf("\n%s\t" "color=#%.6X", padding, l->content.objgr->color);
			printf("\n%s\t" "draworder=", padding); print_draworder(l->content.objgr->draworder);
			printf("\n%s\t" "type=ObjectGroup", padding);
			dump_objects(l->content.objgr->head, depth+1);
		} else if (l->type == L_IMAGE) {
			printf("\n%s\t" "type=ImageLayer", padding);
			dump_image(l->content.image, depth+1);
		} else if (l->type == L_GROUP) {
			printf("\n%s\t" "type=Group", padding);
			dump_layer(l->content.group_head, tc, depth+1);
		}
		dump_prop(l->properties, depth+1);
		printf("\n%s}", padding);
	}

	if (l) {
		if (l->next) dump_layer(l->next, tc, depth);
	}
}

void dump_map(tmx_map *m) {
	if (!m) tmx_perror("error");
	printf("map={");
	if (m) {
		printf("\n\t" "orient="); print_orient(m->orient);
		printf("\n\t" "renderorder=%d", m->renderorder);
		printf("\n\t" "height=%u", m->height);
		printf("\n\t" "width=%u", m->width);
		printf("\n\t" "theight=%u", m->tile_height);
		printf("\n\t" "twidth=%u", m->tile_width);
		printf("\n\t" "bgcol=#%.6X", m->backgroundcolor);
		printf("\n\t" "staggerindex="); print_stagger_index(m->stagger_index);
		printf("\n\t" "staggeraxis="); print_stagger_axis(m->stagger_axis);
		printf("\n\t" "hexsidelength=%d", m->hexsidelength);
	} else {
		printf("\n(NULL)");
	}
	printf("\n}");

	if (m) {
		dump_tileset(m->ts_head, 0);
		dump_layer(m->ly_head, m->height * m->width, 0);
		dump_prop(m->properties, 0);
		tmx_map_free(m);
	}
	printf("\n");
}

static int mal_vs_free_count = 0;

void* dbg_alloc(void *address, size_t len) {
	if (!address) mal_vs_free_count++; /* !realloc */
	return realloc(address, len);
}

void dbg_free(void *address) {
	if (address) mal_vs_free_count--;
	free(address);
}

/* for tmx_load_callback */
int read_function(void *file, char *buffer, int len) {
	int res;
	res = fread(buffer, 1, len, file);
	if (ferror(file)) {
		perror("error");
	}
	return res;
}

int isOption(const char *arg) {
	return (strlen(arg) > 2) && !strncmp("--", arg, 2);
}

int isMap(const char *arg) {
	int len = strlen(arg);
	return !strncmp(".tmx", arg+len-4, 4);
}

void printUsage(const char *arg0) {
	fprintf(stderr, "usage: %s [--use-rc-mgr] { [--fd|--buffer|--callback] <map.tmx|tileset.tsx> }...\n", arg0);
}

int main(int argc, char *argv[]) {
	tmx_map *m = NULL;
	tmx_resource_manager *rc_mgr = NULL;
	int fd, it;
	FILE *file;
	long size;
	char *buffer;

	if (argc < 2) {
		printUsage(argv[0]);
		return EXIT_FAILURE;
	}

	tmx_alloc_func = dbg_alloc; /* alloc/free dbg */
	tmx_free_func  = dbg_free;

	/* Parses each argument, loads each file (map or tileset) using the specified method (none, fd, buffer, callback) */
	for (it = 1; it < argc; it++) {
		if (isOption(argv[it])) {
			if (it == 1 && !strcmp("--help", argv[it])) {
				printUsage(argv[0]);
				return EXIT_SUCCESS;
			}
			else if (it == 1 && !strcmp("--use-rc-mgr", argv[it])) {
				rc_mgr = tmx_make_resource_manager();
				if (!rc_mgr) {
					tmx_perror("error");
				}
			}
			else if (!strcmp("--fd", argv[it])) {
				fd = open(argv[++it], O_RDONLY);
				if (fd == -1) {
					perror("error");
				}
				else {
					if (isMap(argv[it])) {
						m = tmx_rcmgr_load_fd(rc_mgr, fd);
						dump_map(m);
					}
					else {
						tmx_load_tileset_fd(rc_mgr, fd, argv[it]);
					}
					close(fd);
				}
			}
			else if (!strcmp("--buffer", argv[it])) {
				file = fopen(argv[++it], "r");
				if (!file) {
					perror("error");
				}
				else {
					fseek(file, 0, SEEK_END);
					size = ftell(file);
					rewind(file);

					buffer = (char*)dbg_alloc(NULL, size);
					if (fread(buffer, 1, size, file)+1 && ferror(file))
					{
						perror("error");
					}
					else {
						if (isMap(argv[it])) {
							m = tmx_rcmgr_load_buffer(rc_mgr, buffer, size);
							dump_map(m);
						}
						else {
							tmx_load_tileset_buffer(rc_mgr, buffer, size, argv[it]);
						}
					}

					dbg_free(buffer);
					fclose(file);
				}
			}
			else if (!strcmp("--callback", argv[it])) {
				file = fopen(argv[++it], "r");
				if (!file) {
					perror("error");
				}
				else {
					if (isMap(argv[it])) {
						m = tmx_rcmgr_load_callback(rc_mgr, read_function, file);
						dump_map(m);
					}
					else {
						tmx_load_tileset_callback(rc_mgr, read_function, file, argv[it]);
					}
					fclose(file);
				}
			}
			else {
				if (it == 1) fprintf(stderr, "unknown option: %s\nvalid options are --use-rc-mgr\n", argv[1]);
				fprintf(stderr, "unknown load method: %s\nvalid methods are --fd, --buffer, --callback\n", argv[it]);
			}
		}
		else {
			if (isMap(argv[it])) {
				m = tmx_rcmgr_load(rc_mgr, argv[it]);
				dump_map(m);
			}
			else {
				tmx_load_tileset(rc_mgr, argv[it]);
			}
		}
	}

	if (rc_mgr) {
		tmx_free_resource_manager(rc_mgr);
	}

	printf("%d mem alloc not freed\n", mal_vs_free_count);

#ifdef PAUSE
	puts("press <Enter> to quit\n");
	getchar();
#endif

	return EXIT_SUCCESS;
}
