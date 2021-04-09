
int add_reference(struct reference *r);
int add_stack(struct stack *sta);
int add_label(struct label *l);
int add_section(struct section *s);
int parse_data_blocks(void);
int obtain_source_file_names(void);
int obtain_memorymap(void);
int obtain_rombankmap(void);
int obtain_rombanks(void);
int collect_dlr(void);
int clean_up_dlr(void);
