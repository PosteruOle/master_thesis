#include <stdio.h>
#include <stdlib.h>

unsigned short crcu8(unsigned char data, unsigned short crc) {
    unsigned char i = 0, x16 = 0, carry = 0;
    for (i = 0; i < 8; i++) {
      x16 = (unsigned char)((data & 1) ^ ((unsigned char)crc & 1));
      data >>= 1;
      if (x16 == 1) {
        crc ^= 0x4002;
        carry = 1;
      } else
       carry = 0;
      crc >>= 1;
     if (carry)
      crc |= 0x8000;
     else
      crc &= 0x7fff;
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

  unsigned short report=crcu8(data, crc);
  printf("report = %u\n", report);

  return 0; 
}
