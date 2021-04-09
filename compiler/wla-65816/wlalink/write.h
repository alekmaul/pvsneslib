
#ifndef _WRITE_H
#define _WRITE_H

int fix_references(void);
int fix_labels(void);
int transform_stack_definitions(void);
int insert_sections(void);
int write_symbol_file(char *outname, unsigned char mode);
int write_gb_header(int name_status);
int write_rom_file(char *outname);
int compute_pending_calculations(void);
int compute_stack(struct stack *s, int *result);
int write_bank_header_calculations(struct stack *sta);
int write_bank_header_references(struct reference *r);
int parse_stack(struct stack *sta);
int get_snes_pc_bank(struct label *l);
int correct_65816_library_sections(void);
int is_label_anonymous(char *l);
int is_label_section_local(char* l);
int is_label_file_local(char* l);
struct label *get_closest_anonymous_label(char *name, int rom_address, int file_id, struct label *l, int section_status, int section);

#endif
