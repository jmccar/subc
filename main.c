#include<stdio.h>
#include<stdint.h>
#include"v4prefix.h"
#include"endian.h"

int main(int argc, char *argv[]){
  endian();
  fprintf(stdout, "input,hex,class,net,bcast,usable\n");
  for(uint8_t i = 1; i < argc; i++){
    fprintf(stderr, "\nargv[%d]: %s\n", i, argv[i]);
    struct v4prefix *p = make_v4prefix(argv[i]);
    print_all(p);
    free_v4prefix(p);
  }
  return 0;
}
