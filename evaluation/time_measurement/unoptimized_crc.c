#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned short crcu8(unsigned char data, unsigned short crc) {
    unsigned char i = 0, x16 = 0, carry = 0;
    for (i = 0; i < 8; i++) {
      x16 = (unsigned char)((data & 1) ^ ((unsigned char)crc & 1));
      data >>= 1;
      if (x16 == 1) {
        carry = 1;
        crc ^= 0x4002;
      } else {
       carry = 0;
      } 
      crc >>= 1;
     if (carry)
      crc |= 0x8000;
     else
      crc &= 0x7fff;
     }
    return crc;
}

int main(){

  int data;
  int crc;
  unsigned short report;

  FILE *fin=fopen("inputs.txt", "r");
  if(fin==NULL){
    return -1;
  }

  // Start measuring time
  clock_t start_time = clock();

  while(fscanf(fin, "%d %d", &data, &crc)!=EOF){
    report=crcu8(data, crc);
  }

  // Stop measuring time
  clock_t end_time = clock();

  // Calculate the elapsed time in milliseconds
  double elapsed_time = (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC;
  printf("Execution time: %.3f ms\n", elapsed_time);

  fclose(fin);
   
  return 0; 
}

