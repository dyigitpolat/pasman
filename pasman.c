#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void appendTo( long key, char* text, char* fname);
void retrieveFrom( long key, char* text, char* fname);
void createNew( long key, char* fname);
int checkAuth( long key, long code);
int* encrypt( long key, char* text);
char* decrypt( long key, int* stream);
void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);
void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);


/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */
void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}

/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */
void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}

void createNew( long key, char* fname)
{
  FILE* fp;
  long code;

  fp = fopen( fname, "w");
  code = key * key;
  fwrite( &code, 4, 1, fp);
  fclose( fp);
  printf( "created!\n");
}

void appendTo( long key, char* text, char* fname)
{
  FILE* fp;
  long code;
  int* stream;

  fp = fopen( fname, "ra");
  fread( &code, 4, 1, fp);
  if( code == key * key)
  {
    printf( "small auth success!\n");
  }
  else
  {
    printf( "small auth failure!\n");
    return;
  }

  code = -1;
  while( text[++code]);

  stream = encrypt( key, text);
  fwrite( stream, 2, code, fp);
  fclose( fp);
}

void retrieveFrom( long key, char* text, char* fname)
{
  FILE* fp;
  long code;
  int* stream;

  fp = fopen( fname, "r");
  fread( &code, 4, 1, fp);
  if( code == key * key)
  {
    printf( "small auth success!\n");
  }
  else
  {
    printf( "small auth failure!\n");
    return;
  }

  code = -1;
  while( text[++code]);


}

int* encrypt( long key, char* text)
{
  //TODO:
}

char* decrypt( long key, int* stream)
{
  //TODO:
}

int main( int argc, char** argv)
{
  long key, mode;
  char* text;
  char* fname;

  key = atoi(argv[1]);
  mode = atoi(argv[2]);
  text = argv[3];
  fname = argv[4];

  if( argc != 5)
  {
    printf( "invalid arguments!\n");
    return 1;
  }

  if( mode == 1) //APPEND
  {
    appendTo( key, text, fname);
  }
  else if( mode == 0) //RETRIEVE
  {
    retrieveFrom( key, text, fname);
  }
  else if( mode == 2) //CREATE
  {
    createNew( key, fname);
  }
  else
  {
    printf( "invalid arguments!\n");
    return 1;
  }

  return 0;
}
