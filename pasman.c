#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void appendTo( uint64_t key[2], char* text, char* fname);
void retrieveFrom( uint64_t key[2], char* text, char* fname);
void createNew( uint64_t key[2], char* fname);
int checkAuth( uint64_t key[2], long code);
uint64_t* encrypt( uint64_t key[2], char* text);
char* decrypt( uint64_t key[2], uint64_t* stream, long bytes_read);
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

void createNew( uint64_t key[2], char* fname)
{
  FILE* fp;
  long code;

  fp = fopen( fname, "w");
  code = (int) *key * (int) *key;
  fwrite( &code, 4, 1, fp);
  fclose( fp);
  printf( "created!\n");
}

void appendTo( uint64_t key[2], char* text, char* fname)
{
  FILE* fp;
  long code;
  uint64_t* stream;

  fp = fopen( fname, "ra");
  fread( &code, 4, 1, fp);
  if( code == (int) *key * (int) *key)
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

void retrieveFrom( uint64_t key[2], char* text, char* fname)
{
  FILE* fp;
  long code;
  uint64_t* stream;

  fp = fopen( fname, "r");
  fread( &code, 4, 1, fp);
  if( code == (int) key * (int) key)
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

uint64_t* encrypt( uint64_t key[2], char* text)
{
  uint64_t* encoded;
  long i;
  long len;

  i = -1;
  while( text[++i]);

  encoded = (uint64_t*) malloc( i + i%8);

  memset( encoded, 0, i + i%8);
  memcpy( encoded, text, i);

  len = (i + i%8) >> 3;
  for( i = 0; i < len; i++)
  {
    encipher(64, (uint32_t*) &encoded[i], (uint32_t*) key);
  }

  //wipe out
  memset( text, 0, len << 3);
  free( text);
  return encoded;

}

char* decrypt( uint64_t key[2], uint64_t* stream, long bytes_read)
{
  char* text;
  long i;
  long len;

  text = (char*) malloc( bytes_read);

  memset( text, 0, bytes_read);
  memcpy( text, stream, bytes_read);

  for( i = 0; i < bytes_read; i++)
  {
    decipher(64, (uint32_t*) &text[i], (uint32_t*) key);
  }

  //wipe out
  memset( stream, 0, bytes_read);
  free( stream);
  return text;
}

int main( int argc, char** argv)
{
  uint64_t key[2];
  int mode;
  char* text;
  char* fname;

  memcpy( key, argv[1], 16);
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
  else if( mode == 3) //TEST ENCRYPT, may remove after test
  {
    //TODO:
  }
  else if( mode == 4) // TEST DECRYPT, may remove after test
  {
    //TODO:
  }
  else
  {
    printf( "invalid arguments!\n");
    return 1;
  }

  return 0;
}
