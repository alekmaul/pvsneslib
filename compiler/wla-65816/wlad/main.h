int main(int argc, char *argv[]);
int output_bank_opcodes(int fs, int *b, unsigned char *in, int bank_size, int *i);
int output_bank_data(int fs, int *b, unsigned char *in, int bank_size, int *i);
int output_bank_data_detect_strings(int fs, int *b, unsigned char *in, int bank_size, int *i);
int parse_flags(char *f);
int letter_check(char c);
