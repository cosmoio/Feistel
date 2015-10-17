#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define ROUNDS 8
#define BL 8


void encrypt(unsigned int *, unsigned int *);
void decrypt(unsigned int *, unsigned int *);
unsigned int f(unsigned int ,unsigned int);
void dec_key(unsigned int *);


void encrypt(unsigned int *data, unsigned int *key)
{
	unsigned int l,r,help;
	int rnd;

   l = data[0];   /* left 32 bit block */
   r = data[1];   /* right 32 bit block */

   	for(rnd=0;rnd<ROUNDS;rnd++)     /* 4 round Feistel network (encrypt) */
	{   
   		help = l ^ f(r,key[rnd]);
    	l = r;
		r = help;
   }

   data[0] = l;
   data[1] = r;
}

void decrypt(unsigned int *data, unsigned int *key) 
{
	unsigned int l,r,help;
	int rnd;

   l = data[0];    /* left 32 bit */
   r = data[1];    /* right 32 bit */


	for(rnd=ROUNDS-1;rnd>=0;rnd--)
	{   /* 4 round Feistel network (decrypt) */
	   	help = r ^ f(l,key[rnd]);
		r = l;
		l = help;
    }

   data[0] = l;
   data[1] = r;
}


unsigned int f(unsigned int right, unsigned int key)
{
   right += key;
   return (right);
}



int main(int argc, char *argv[])
{
	int in, out;
	bool enc = true;
	char inbuf[BL];
	char key[ROUNDS*4];
	unsigned int i, *pkey, *ptr_in;


	memset(key, 0, ROUNDS*4);
	// Assumption, ROUNDS NEVER < 4
	strncpy(key,"A TRULY GENUINE PASSWORD", 25);

	pkey = (unsigned int *) key;
	if(argc == 4)      /* If a 3rd parameter is set then decrypt */
		enc = false;
	if(!(in=open(argv[1],O_RDONLY)) || !(out=open(argv[2],O_WRONLY | O_EXCL | O_CREAT)))
	{
		printf("\nUsage: Feistel input output [flag]\n");
	   	exit(0);
	}

	int n;
	while((n = read(in, inbuf, BL)) > 0)
	{
		ptr_in = (unsigned int *) inbuf;
	   	if(enc==true)
	   		encrypt(ptr_in,pkey);            /* Calls encrypt */
	   	else
	   		decrypt(ptr_in,pkey);            /* Calls decrypt */
	   	
		write(out, inbuf, BL);
	}
	return EXIT_SUCCESS;
}

