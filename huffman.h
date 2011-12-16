#ifndef HUFFMAN_H
#define HUFFMAN_H

#define ENGLISH		0
#define EALPHABETSZ	27
#define GERMAN 		1

#define ALPHABETSZ	50

#define MAXDEPTH	10





struct huffman_node {
	unsigned char character;
	unsigned int frequency;
	struct huffman_node *next[2];
};

struct schedule_node {
    struct schedule_node *next;
    struct huffman_node *huffman;
};

struct language_book {
	unsigned char length;
	unsigned char character[ALPHABETSZ];
	unsigned short frequency[ALPHABETSZ];
};

struct code_book {
	unsigned char alphabetsz;
	unsigned short *code;
	unsigned char *length;
};

void deconstruct_schedule(struct schedule_node *first);
struct schedule_node *construct_schedule(struct language_book *book, struct schedule_node *first);
struct huffman_node *extract_huffman_tree(struct schedule_node *first);
unsigned char get_language(char *input, unsigned char *array);
unsigned char analyze_frequency(unsigned char *array);
void traverse_tree(struct huffman_node *node, unsigned char depth, unsigned short counter);
void encode_huffman(char *input, char *output);
void decode_huffman(char *input, char *output, struct huffman_node *node);
unsigned char append_code(unsigned short code, unsigned char length,
							unsigned char free, int *bitstream,
							unsigned char mod);
#endif
