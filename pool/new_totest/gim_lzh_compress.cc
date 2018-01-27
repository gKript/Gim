/**************************************************************
	lzhuf.c
	written by Haruyasu Yoshizaki 11/20/1988
	some minor changes 4/6/1989
	comments translated by Haruhiko Okumura 4/7/1989
**************************************************************/

/*
LZHUF.C (c)1989 by Haruyasu Yoshizaki, Haruhiko Okumura, and Kenji Rikitake.
All rights reserved. Permission granted for non-commercial use.
*/ 
/*
32 Bit Implementation and generic useable form by Alex Polverini of JapoTek
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/gim_lzh.h"

#define p_malloc(n)		malloc(n)
#define p_free(n)		free(n)
#define p_realloc(v,n)  realloc(v,n)

//#include <io.h>

typedef unsigned char uchar;

static long codesize,OutCounter;
static unsigned char *SourceData;
static unsigned char *OutData;
static unsigned long SourceDataSize=0;
static unsigned long SourceDataCounter=0;

void Errore()
{
	fprintf(stderr,"\nError in compress.c !\n");
	exit(EXIT_FAILURE);
}

#define	CHUNK_SIZE	10000

int OutByte(uchar b)
{
if (!OutData) {
  if (!(OutData=(unsigned char *)(CHUNK_SIZE))) return 1;
  OutCounter=0;
  }
else
if (!(OutCounter % CHUNK_SIZE)) {
	void *p=p_realloc(OutData,OutCounter+CHUNK_SIZE);
    if (!p) {
      //p_free(OutData);
      OutData=NULL;
      return 1;
      }
	else OutData=(unsigned char *)p;
	}
OutData[OutCounter++]=b;
return 0;
}

uchar *AddByte1(uchar *p,uchar b)	// Not used now
{
static int Counter=0;

if (!p) {
  if (!(p=(uchar *)p_malloc(CHUNK_SIZE))) return NULL;
  p[0]=b;
  Counter=1;
  return p;
  }
else
  if (Counter % CHUNK_SIZE) {
    p[Counter++]=b;
    return p;
    }
  else {
	void *_p;
    if (!(_p=p_realloc(p,Counter+CHUNK_SIZE))) {
      //p_free(p); 
	  return NULL;
      }
	else
		p=(uchar *)_p;
    p[Counter++]=b;
    return p;
    }
}

/********** LZSS compression **********/

#define N		4096	/* buffer size */
#define F		60	/* lookahead buffer size */
#define THRESHOLD	2
#define NIL		N	/* leaf of tree */

static uchar	text_buf[N + F - 1];
static int	match_position;
static int  match_length;
static int 	lson[N + 1];
static int  rson[N + 257];
static int  dad[N + 1];

void InitTree(void)  /* initialize trees */
{
int	i;

for (i = N + 1; i <= N + 256; i++)
	rson[i] = NIL;			/* root */
for (i = 0; i < N; i++)
	dad[i] = NIL;			/* node */
}

void InsertNode(int r)  /* insert to tree */
{
	int	i, p, cmp;
	unsigned char  *key;
	unsigned c;

	cmp = 1;
	key = &text_buf[r];
	p = N + 1 + key[0];
	rson[r] = lson[r] = NIL;
	match_length = 0;
	for ( ; ; ) {
		if (cmp >= 0) {
			if (rson[p] != NIL)
				p = rson[p];
			else {
				rson[p] = r;
				dad[r] = p;
				return;
			}
		} else {
			if (lson[p] != NIL)
				p = lson[p];
			else {
				lson[p] = r;
				dad[r] = p;
				return;
			}
		}
		for (i = 1; i < F; i++)
			if ((cmp = key[i] - text_buf[p + i]) != 0)
				break;
		if (i > THRESHOLD) {
			if (i > match_length) {
				match_position = ((r - p) & (N - 1)) - 1;
				if ((match_length = i) >= F)
					break;
			}
			if (i == match_length) {
				if ((c = ((r - p) & (N - 1)) - 1) < match_position) {
					match_position = c;
				}
			}
		}
	}
	dad[r] = dad[p];
	lson[r] = lson[p];
	rson[r] = rson[p];
	dad[lson[p]] = r;
	dad[rson[p]] = r;
	if (rson[dad[p]] == p)
		rson[dad[p]] = r;
	else
		lson[dad[p]] = r;
	dad[p] = NIL;  /* remove p */
}

void DeleteNode(int p)  /* remove from tree */
{
	int	q;

	if (dad[p] == NIL)
		return;			/* not registered */
	if (rson[p] == NIL)
		q = lson[p];
	else
	if (lson[p] == NIL)
		q = rson[p];
	else {
		q = lson[p];
		if (rson[q] != NIL) {
			do {
				q = rson[q];
			} while (rson[q] != NIL);
			rson[dad[q]] = lson[q];
			dad[lson[q]] = dad[q];
			lson[q] = lson[p];
			dad[lson[p]] = q;
		}
		rson[q] = rson[p];
		dad[rson[p]] = q;
	}
	dad[q] = dad[p];
	if (rson[dad[p]] == p)
		rson[dad[p]] = q;
	else
		lson[dad[p]] = q;
	dad[p] = NIL;
}

/* Huffman coding */

#define N_CHAR  	(256 - THRESHOLD + F)
				/* kinds of characters (character code = 0..N_CHAR-1) */
#define T 		(N_CHAR * 2 - 1)	/* size of table */
#define R 		(T - 1)			/* position of root */
#define MAX_FREQ	0x8000		/* updates tree when the */
					/* root frequency comes to this value. */

/* table for encoding and decoding the upper 6 bits of position */

/* for encoding */
static uchar p_len[64] = {
	0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
};

static uchar p_code[64] = {
	0x00, 0x20, 0x30, 0x40, 0x50, 0x58, 0x60, 0x68,
	0x70, 0x78, 0x80, 0x88, 0x90, 0x94, 0x98, 0x9C,
	0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC,
	0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
	0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
	0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

/* for decoding */
static uchar d_code[256] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
	0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
	0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
	0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
	0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
	0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
	0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
	0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
	0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
	0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
	0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
	0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
	0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

static uchar d_len[256] = {
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

static unsigned freq[T + 1];	/* frequency table */

static int	prnt[T + N_CHAR];	/* pointers to parent nodes, except for the */
			/* elements [T..T + N_CHAR - 1] which are used to get */
			/* the positions of leaves corresponding to the codes. */

static int	son[T];		/* poshorters to child nodes (son[], son[] + 1) */

static unsigned getbuf;
static uchar getlen;

int	GetBit(void)	/* get one bit */
{
	short	i;

	while (getlen <= 8) {
		if(SourceDataCounter >= SourceDataSize)
			i=0;
		else {
//		if ((i = getc(infile)) < 0) i = 0;
			i=*SourceData++;
			SourceDataCounter ++;
		}
		getbuf |= i << (8 - getlen);
		getlen += 8;
	}
	i = getbuf;
	getbuf <<= 1;
	getlen--;
	return (i < 0);
}

int	GetByte(void)	/* get one byte */
{
	unsigned short i;

	while (getlen <= 8) {
//		if ((i = getc(infile)) < 0) i = 0;
		if(SourceDataCounter >= SourceDataSize)
			i=0;
		else {
			i=*SourceData++;
			SourceDataCounter ++;
			}
		getbuf |= i << (8 - getlen);
		getlen += 8;
	}
	i = getbuf;
	getbuf <<= 8;
	getlen -= 8;
	return i >> 8;
}

static unsigned putbuf;
static uchar putlen;

void Putcode(int l, unsigned c)		/* output c bits of code */
{
	putbuf |= c >> putlen;
	if ((putlen += l) >= 8) {
		if (OutByte((uchar)(putbuf>>8))) Errore();
		if ((putlen -= 8) >= 8) {
			if (OutByte((uchar)putbuf)) Errore();
			codesize += 2;
			putlen -= 8;
			putbuf = c << (l - putlen);
		} else {
			putbuf <<= 8;
			codesize++;
		}
	}
}


/* initialization of tree */

void StartHuff(void)
{
	int	i, j;

	for (i = 0; i < N_CHAR; i++) {
		freq[i] = 1;
		son[i] = i + T;
		prnt[i + T] = i;
	}
	i = 0; j = N_CHAR;
	while (j <= R) {
		freq[j] = freq[i] + freq[i + 1];
		son[j] = i;
		prnt[i] = prnt[i + 1] = j;
		i += 2; j++;
	}
	freq[T] = 0xffffffff;
	prnt[R] = 0;
}


/* reconstruction of tree */

void reconst(void)
{
	int	i, j, k;
	unsigned f, l;

	/* collect leaf nodes in the first half of the table */
	/* and replace the freq by (freq + 1) / 2. */
	j = 0;
	for (i = 0; i < T; i++) {
		if (son[i] >= T) {
			freq[j] = (freq[i] + 1) / 2;
			son[j] = son[i];
			j++;
		}
	}
	/* begin constructing tree by connecting sons */
	for (i = 0, j = N_CHAR; j < T; i += 2, j++) {
		k = i + 1;
		f = freq[j] = freq[i] + freq[k];
		for (k = j - 1; f < freq[k]; k--);
		k++;
		l = (j - k) * sizeof(int);
		memmove(&freq[k + 1], &freq[k], l);
		freq[k] = f;
		memmove(&son[k + 1], &son[k], l);
		son[k] = i;
	}
	/* connect prnt */
	for (i = 0; i < T; i++) {
		if ((k = son[i]) >= T) {
			prnt[k] = i;
		} else {
			prnt[k] = prnt[k + 1] = i;
		}
	}
}


/* increment frequency of given code by one, and update tree */

void update(int c)
{
	int	i, j, k, l;

	if (freq[R] == MAX_FREQ) {
		reconst();
	}
	c = prnt[c + T];
	do {
		k = ++freq[c];

		/* if the order is disturbed, exchange nodes */
		if (k > freq[l = c + 1]) {
			while (k > freq[++l]);
			l--;
			freq[c] = freq[l];
			freq[l] = k;

			i = son[c];
			prnt[i] = l;
			if (i < T) prnt[i + 1] = l;

			j = son[l];
			son[l] = i;

			prnt[j] = c;
			if (j < T) prnt[j + 1] = c;
			son[c] = j;

			c = l;
		}
	} while ((c = prnt[c]) != 0);	/* repeat up to root */
}

static unsigned code;
static unsigned len;

void EncodeChar(unsigned c)
{
	unsigned i;
	int	j, k;

	i = 0;
	j = 0;
	k = prnt[c + T];

	/* travel from leaf to root */
	do {
		i >>= 1;

		/* if node's address is odd-numbered, choose bigger brother node */
		if (k & 1) i += 0x8000;

		j++;
	} while ((k = prnt[k]) != R);
	Putcode(j, i);
	code = i;
	len = j;
	update(c);
}

void EncodePosition(unsigned c)
{
	unsigned i;

	/* output upper 6 bits by table lookup */
	i = c >> 6;
	Putcode(p_len[i], (unsigned)p_code[i] << 8);

	/* output lower 6 bits verbatim */
	Putcode(6, (c & 0x3f) << 10);
}

void EncodeEnd(void)
{
	if (putlen) {
		if (OutByte((uchar)(putbuf>>8))) Errore();
		codesize++;
	}
}

int	DecodeChar(void)
{
	unsigned c;

	c = son[R];

	/* travel from root to leaf, */
	/* choosing the smaller child node (son[]) if the read bit is 0, */
	/* the bigger (son[]+1} if 1 */
	while (c < T) {
		c += GetBit();
		c = son[c];
	}
	c -= T;
	update(c);
	return c;
}

int	DecodePosition(void)
{
	unsigned i, j, c;

	/* recover upper 6 bits from table */
	i = GetByte();
	c = (unsigned)d_code[i] << 6;
	j = d_len[i];

	/* read lower 6 bits verbatim */
	j -= 2;
	while (j--) {
		i = (i << 1) + GetBit();
	}
	return c | (i & 0x3f);
}

/* compression */
unsigned char *EncodeLzHuf(unsigned char *SourceData,unsigned long NumBytes,unsigned long *OutLen)
{
	int	i, len, r, s, last_match_length;
	long	textsize=0,printcount=0;

	if (!NumBytes) return NULL;
	putbuf=putlen=getbuf=getlen=codesize=0; OutData=NULL;

	StartHuff();
	InitTree();
	s = 0;
	r = N - F;
	for (i = s; i < r; i++)
		text_buf[i] = ' ';
	for (len = 0; len < F && NumBytes>0 ; len++) {
		text_buf[r + len] = *SourceData++;
		NumBytes--;
		}
	textsize = len;
	for (i = 1; i <= F; i++)
		InsertNode(r - i);
	InsertNode(r);
	do {
		if (match_length > len)
			match_length = len;
		if (match_length <= THRESHOLD) {
			match_length = 1;
			EncodeChar(text_buf[r]);
		} else {
			EncodeChar(255 - THRESHOLD + match_length);
			EncodePosition(match_position);
		}
		last_match_length = match_length;
		for (i = 0; i < last_match_length && NumBytes>0 ; i++) {
			DeleteNode(s);
			text_buf[s] = *SourceData;
			if (s < F - 1)
				text_buf[s + N] = *SourceData;
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			InsertNode(r);
			SourceData++;
			NumBytes--;
		}
		textsize*=i;
		while (i++ < last_match_length) {
			DeleteNode(s);
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			if (--len) InsertNode(r);
		}
	} while (len > 0);
	EncodeEnd();
	*OutLen=OutCounter;
	return OutData;
}

/* recover */
// TODO: passare anke la dimensione della sorgente e fare i controlli dovuti nella GetBit
// testare con il file room3327.anj di Blood&lace

unsigned char *DecodeLzHuf(unsigned char *Sdata, unsigned char *OData,
			   unsigned long textsize,unsigned long sourcesize)
{
	int	i, j, k, r, c, count, printcount=0;
	unsigned char *OutData;

	if (textsize == 0) return NULL;
	putbuf=putlen=getbuf=getlen=codesize=0;
	SourceData=Sdata;
	SourceDataSize=sourcesize;
	SourceDataCounter=0;
	if (!OData) {
	  if (!(OData=OutData=(unsigned char *)p_malloc(textsize))) return NULL;
	  }
	else OutData=OData;

	StartHuff();
	for (i = 0; i < N - F; i++)
		text_buf[i] = ' ';
	r = N - F;
	for (count = 0; count < textsize; ) {
		c = DecodeChar();
		if (c < 256) {
	       		*OutData++=c;
				if(OutData>=(OData+textsize))goto ex;
				text_buf[r++] = c;
			r &= (N - 1);
			count++;
		} else {
			i = (r - DecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESHOLD;
			for (k = 0; k < j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				*OutData++=c;
				if(OutData>=(OData+textsize))goto ex;
				
				text_buf[r++] = c;
				r &= (N - 1);
				count++;
			}
		}
//		if (count > printcount) {
//			printf("%12ld\r", count); fflush(stdout);
//			printcount += 4096;
//		}
	}
ex:
//	printf("%12ld\n", count);
	return OData;
}

