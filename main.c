#include<stdio.h>
#include<stdint.h>
#include"v4prefix.h"


int main(int argc, char *argv[]){
  fprintf(stdout, "input,class,net,bcast,usable\n");
  for(uint8_t i = 1; i < argc; i++){
    fprintf(stderr, "\nargv[%d]: %s\n", i, argv[i]);
    struct v4prefix *p = make_v4prefix(argv[i]);
    print_all(p);
    free_v4prefix(p);
  }
/*
  // parse IP into uint32_t
  uint8_t octets[4];
  uint8_t o_cnt = 0;
  uint8_t i;
  char *input = "203.000.013.255/23";
  char *walk = input;

  for(uint8_t o = 0; o < 4; o++){
    char buf[4];
    i = 0;
    while(*walk != '.' && *walk != '/'){
      buf[i] = *walk;
      walk++;
      i++;
    }
    buf[3] = 0;
    octets[o_cnt] = int_from_octetstr(buf);
    o_cnt++;
    walk++;
  }

  printf("remaining %s", walk);
  uint32_t addr = 0;
  for(uint8_t i = 0; i < 4; i++){
    uint32_t e = (1 << (24 - (8 * i)));
    printf("num %u\n", octets[i] * e);
    addr += octets[i] * (1 << (24 - (8 * i)));
  }
  printf("addr %u\n", addr);
  */
  return 0;
}
