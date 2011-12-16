#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "huffman.h"

struct schedule_node *sched;

struct huffman_node *english;
struct huffman_node *german;

struct language_book english_book = {EALPHABETSZ, {'\0', 'z', 'q', 'x', 'j', 'k', 'v',
					'b', 'p', 'y', 'g', 'f', 'w', 'm', 'u', 'c',
					'l', 'd', 'r', 'h', 's', 'n', 'i', 'o', 'a',
					't', 'e'}, {3, 74, 95, 150, 153, 772, 978, 1492,
					1929, 1974, 2015, 2228, 2360, 2406, 2758, 2782,
					4025, 4253, 5987, 6094, 6327, 6749, 6966, 7507,
					8167, 9056, 12700}};

struct code_book *code_en;

unsigned char letterfreq[EALPHABETSZ];
char *sample = "This is just a simple test which should show us the distribution of single char in the alphabet";
char *longword = "Antidisestablishmentarianism";
char longwordencode[64];
char longworddecode[64];
/*char *inputtest = "test"; */
/*char outputtest[256]; */

//unsigned short temptest[EALPHABETSZ];
//char code_book_en[EALPHABETSZ][9];

int main(void)
{
	sched = calloc(1, sizeof(struct schedule_node));
	sched->next = NULL;
	sched->huffman = NULL;
	english = calloc(1, sizeof(struct huffman_node));

	code_en = calloc(1, sizeof(struct code_book));
	code_en->alphabetsz = EALPHABETSZ;
	code_en->code = calloc(EALPHABETSZ, sizeof(unsigned short));
	code_en->length = calloc(EALPHABETSZ, sizeof(unsigned char));


    if (construct_schedule(&english_book, sched) == NULL) {
        deconstruct_schedule(sched);
        printf("Scheduler failed!\n");
        return 0;
    }

    /* deconstruct_schedule(sched);
    printf("Success\n"); */

    if ((english = extract_huffman_tree(sched)) == NULL) {
        printf("Error\n");
        /* deinitialization */
        return 0;
    }

    traverse_tree(english, 0, 0);
    printf("Done!\n");

    encode_huffman(longword, longwordencode);
    decode_huffman(longwordencode, longworddecode, english);
    printf("%s\n", longworddecode);

    /* get_language(sample, letterfreq); */

    return 0;
}


struct schedule_node *construct_schedule(struct language_book *book,
                                          struct schedule_node *first)
{
    struct huffman_node *tmphuff;
    struct schedule_node *tmpsched = NULL;
    struct schedule_node *tmpschedold = NULL;

    unsigned char i = 0;

    for (i=0;i<book->length;i++) {
        tmpsched = calloc(1,sizeof(struct schedule_node));
        if (tmpsched == NULL) {
            printf("Schedule Node: Alloc failure.\n");
            return NULL;
        }
        if (i != 0) /* next ptr is set after 1st iteration */
            tmpschedold->next = tmpsched;
        else
            first->next = tmpsched;
        tmpsched->huffman = calloc(1,sizeof(struct huffman_node));
        if (tmpsched->huffman == NULL) {
            printf("Huffman Node: Alloc failure.\n");
            return NULL;
        }
        tmphuff = tmpsched->huffman;
        tmphuff->character = book->character[i];
        tmphuff->frequency = book->frequency[i];
        tmphuff->next[0] = NULL;
        tmphuff->next[1] = NULL;
        //tmphuff->previous = NULL;
        tmpschedold = tmpsched;
    }
    tmpsched->next = NULL; /* last elem */
    return tmpsched;
}

void deconstruct_schedule(struct schedule_node *first)
{
    struct schedule_node *tmpold = NULL;
    struct schedule_node *tmp = first;
    while (tmp != NULL) {
        if(tmp->huffman != NULL)
            free(tmp->huffman);
        tmpold = tmp;
        if(tmp->next != NULL)
            tmp = tmp->next;
        else {
            free(tmpold);
            break;
        }
        free(tmpold);
    }
}


void insert_schedule_node(struct schedule_node *node,
                           struct schedule_node *tree)
{
    struct schedule_node *tmpold = tree;
	struct schedule_node *tmp = tree->next;

    //if (tmp->next == NULL) {	    /* only 1 elem left */
    //    tree->next = node;        /* tree points to node */
	//	tree->next->next = NULL;  /* tree->huffman contains the final huffman tree */
	//	huffman_tree = tree->next->huffman;
	//	free(tree);
	//	return;
	//}

	while (node->huffman->frequency > tmp->huffman->frequency) {
		if (tmp->next == NULL) {    /* was last element */
			tmp->next = node;       /* append new element */
			return;
		}
		tmpold = tmp;
		tmp = tmp->next;		    /* continue search */
	}
    node->next = tmp;               /* insert node */
    tmpold->next = node;
}


struct huffman_node *extract_huffman_tree(struct schedule_node *first)
{
    struct huffman_node *parent;
    struct huffman_node *ptr;
    struct huffman_node *tmp1, *tmp2;
    struct schedule_node *firstcpy = first->next;
    struct schedule_node *tmp = firstcpy;
    struct schedule_node *head = calloc(1, sizeof(struct schedule_node));
    head->huffman = NULL;
    while (tmp != NULL) {           /* at least 2 more elem */
        tmp1 = tmp->huffman;        /* smaller elem */
        tmp2 = tmp->next->huffman;  /* larger elem */
        parent = calloc(1, sizeof(struct huffman_node));
        if (parent == NULL) {
            printf("Huffman Node: Alloc failure!\n");
            return NULL;
        }
        parent->character = 0;
        //parent->previous = NULL;
        parent->next[0] = tmp1;     /* smaller is left */
        parent->next[1] = tmp2;     /* larger is right */
        parent->frequency = tmp1->frequency + tmp2->frequency;
        tmp->next->huffman = parent;/* 2nd sched points to parent now */
        if (firstcpy->next->next == NULL) {	/* schedule tree empty */
        	ptr = tmp->next->huffman;
        	free(tmp);
        	free(head);
        	return ptr;
        }
        firstcpy = firstcpy->next->next;  /* first points now to 3rd elem*/
        tmp->next->next = NULL;		/* elem is isolated */
        head->next = firstcpy;
        insert_schedule_node(tmp->next, head);
        free(tmp);                  /* first elem is freed */
        firstcpy = head->next;
        tmp = firstcpy;
    }
    return NULL;
}

unsigned char get_language(char *input, unsigned char *array) {
	unsigned char offset;
	char *temp = input;
	memset(letterfreq, '0', EALPHABETSZ*sizeof(char));
	while (*temp != '\0') {
		if (isupper(*temp))
			offset = 'A';
		else if (islower(*temp))
			offset = 'a';
		else if ((*temp) == 32) {
			temp++;
			continue;	/* blank */
		}
		else {
			printf("Invalid char %c\n", *temp);
			return 0;
		}
		array[(*temp++)-offset]++;
	}
	return analyze_frequency(array);
}

unsigned char analyze_frequency(unsigned char *array)
{
	return 1;
}

void traverse_tree(struct huffman_node *node, unsigned char depth, unsigned short counter)
{
	unsigned short val;
	unsigned short temp;
	unsigned char offset;

	if (node == NULL)
		return;
	if (node->next[0] == NULL && node->next[1] == NULL) {
		offset = (node->character == '\0') ? 0 : 96;
		val = counter>>(MAXDEPTH-depth);
		code_en->code[(node->character) - offset] = val;
		code_en->length[(node->character) - offset] = depth;
	}
		traverse_tree(node->next[0], depth+1, counter);	/* left child */
		temp = counter+(1<<((MAXDEPTH -1)-depth));
		traverse_tree(node->next[1], depth+1, temp); /* right child */

}

void decode_huffman(char *input, char *output, struct huffman_node *node)
{
	unsigned char path;
	unsigned char iteration = 0;
	char lastchar = 1;
	char *tempin = input;
	char *tempout = output;
	int bitstream = *((int *)(tempin));
	struct huffman_node *tmpnode;
	while (lastchar != '\0') {
		tmpnode = node;
		while (tmpnode->next[0] != NULL && tmpnode->next[1] != NULL) {
			path = (bitstream >> (31 - iteration++)) & 0x1;
			tmpnode = tmpnode->next[path];
			if (iteration == 32) {
				tempin += 4;
				bitstream = *((int *)(tempin));
				iteration = 0;
			}
		}
		lastchar = tmpnode->character;
		*tempout++ = lastchar;
	}
}

void encode_huffman(char *input, char *output)
{

	unsigned char modulo, offset, length;
	unsigned short code;
	unsigned char freebits = 32;
	int bitstream = 0;
	unsigned char cont = 1;	/* end of text */
	char *tempin = input;
	char *tempout = output;
	while ( cont) {	/* end of string not yet reached */
		if (islower(*tempin))
			offset = 96;
		else if (isupper(*tempin))
			offset = 64;
		else if (*tempin == '\0') {
			offset = 0;
			cont = 0;
		}
		code = code_en->code[(*tempin)-offset];
		length = code_en->length[(*tempin)-offset];
		modulo = append_code(code, length, freebits, &bitstream, 0);
		if (modulo == 0)
			freebits = freebits - length;
		else if (modulo == 255) {
			memcpy(tempout, &bitstream, sizeof(int));
			tempout = tempout + 4;
			freebits = 32;
			bitstream = 0;
		}
		else {
			memcpy(tempout, &bitstream, sizeof(int));
			tempout = tempout + 4;
			freebits = 32;
			bitstream = 0;
			append_code(code, length, freebits, &bitstream, modulo);
			freebits = freebits - modulo;
		}
		tempin++;
	}
	memcpy(tempout, &bitstream, sizeof(int)); /* copy ..\n sequence */
}

unsigned char append_code(unsigned short code, unsigned char length,
							unsigned char free, int *bitstream,
							unsigned char mod)
{
	unsigned char modulo, leftover;
	int mask, tempbit;
	leftover = (mod != 0) ? mod : length;
	if (length > free) {	/* code & mask (nr of bits to append), shift to position */
		mask = (1 << free) -1;
		tempbit = (code >> (length - free)) & mask;
		(*bitstream) = (*bitstream) | tempbit ;
		modulo = length - free;
	}
	else {
		mask = (1 << leftover) -1;
		tempbit = (code & mask) << (free-leftover);
		(*bitstream) = (*bitstream) | tempbit;
		modulo = (free == length) ? 255 : 0;
	}
	return modulo;
}

















