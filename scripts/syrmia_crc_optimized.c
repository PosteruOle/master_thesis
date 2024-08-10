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


int main(int argc, char **argv){

  if(argc!=3){
    fprintf(stderr, "Not enough command line arguments!\n");
    exit(EXIT_FAILURE);
  }

  unsigned char data=(unsigned char)atoi(argv[1]);
  unsigned short crc=(unsigned short)atoi(argv[2]);
  
  unsigned short report=crcu8_optimized(data, crc);
  printf("report = %u\n", report);
   
  return 0; 
}
