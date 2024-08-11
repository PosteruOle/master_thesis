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
  FILE *fin=fopen("inputs.txt", "r");
  if(fin==NULL){
    return -1;
  }

  FILE *fout=fopen("output2.txt", "w");
  unsigned short report;

  while(fscanf(fin, "%d %d", &data, &crc)!=EOF){
    report=crcu8(data, crc);
    fprintf(fout, "report = %u\n", report);
  }

  fclose(fin);
  fclose(fout);

  return 0; 
}

