#include <stdio.h>
#include <stdlib.h>

unsigned short crcu8_optimized(unsigned char data, unsigned short _crc)  {
    unsigned char i = 0, x16 = 0, carry = 0;
    long crc = _crc;
    crc ^= data;
    for (i = 0; i < 8; i++) {
      x16 = (unsigned char)crc & 1;
      data >>= 1;
      crc >>= 1;
      crc ^= (x16 & 1) ? 0xa001 : 0; // Conditional XOR
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

  FILE *fout=fopen("output1.txt", "w");
  unsigned short report;

  while(fscanf(fin, "%d %d", &data, &crc)!=EOF){
    report=crcu8_optimized(data, crc);
    fprintf(fout, "report = %u\n", report);
  }

  fclose(fin);
  fclose(fout);
   
  return 0; 
}
