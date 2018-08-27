#include<stdio.h>
#include<stdint.h>

uint8_t endian(){
  uint16_t endian = 0x1020; // 16 and 32
  uint8_t *p = (uint8_t *)&endian;
  fprintf(stderr, "%p:%u\t", p, p[0]);
  fprintf(stderr, "%p:%u\t", p+1, p[1]);
  if(p[0] == 0x10 && p[1] == 0x20){
    fprintf(stderr, "--> big\n");
    return 1;
  } else if(p[0] == 0x20 && p[1] == 0x10){
    fprintf(stderr, "--> little\n");
    return 2;
  } else {
    fprintf(stderr, "--> unknown\n");
    return 0;
  }
}
