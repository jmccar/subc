#include<stdint.h>
#include<stdlib.h>
#include"v4prefix.h"

long V4P_S = sizeof(struct v4prefix);

struct v4prefix *malloc_v4prefix(){
  struct v4prefix *prefix = malloc(V4P_S);
  fprintf(stderr, "grab %ld bytes at %p (ptr: %p)\n", V4P_S, prefix, &prefix);
  return prefix;
}

void free_v4prefix(struct v4prefix *prefix){
  fprintf(stderr, "free %ld bytes at %p (ptr: %p)\n", V4P_S, prefix, &prefix);
  free(prefix);
}

// ascii 48 -> 0, 57 -> 9
uint8_t int_from_ascii(char ascii_char){
  return ascii_char - 48;
}

uint8_t exponent(uint8_t base, uint8_t exp){
  uint8_t product = 1;
  for(uint8_t i = 0; i < exp; i++){
    product *= base;
  }
  return product;
}

uint8_t int_from_octetstr(char *octetstr){
  uint8_t sum = 0;
  for(uint8_t i = 0; i < 3; i++){
    uint8_t ascii = int_from_ascii(octetstr[i]); 
    sum += ascii * exponent(10, 2 - i);
  }
  return sum;
}


void set_tag(struct v4prefix *prefix, uint16_t tag){
  prefix->tag = tag;
}

prefix_class get_pclass(uint8_t o1){
  if(o1 < 128){
    return 'A';
  } else if(o1 >= 128 && o1 < 192){
    return 'B';
  } else if(o1 >=192 && o1 < 224){
    return 'C';
  } else if(o1 >= 224 && o1 < 240){
    return 'D';
  } else {
    return 'E';
  }
}

struct v4prefix *make_v4prefix(char *input){
  uint8_t octets[5];
  uint8_t o_cnt = 0;
  uint8_t i;
  char *walk = input;
  for(uint8_t o = 0; o < 5; o++){
    char buf[4];
    buf[3] = 0;
    i = 0;
    while(*walk != '.' && *walk != '/' && *walk != 0){
      buf[i] = *walk;
      fprintf(stderr, "buf[%u]: %c\n", i, buf[i]);
      walk++;
      i++;
    }
    octets[o_cnt] = int_from_octetstr(buf);
    fprintf(stderr, "octets[%u]: %u\n", o_cnt, octets[o_cnt]);
    o_cnt++;
    walk++;
  }

  uint32_t addr = 0;
  for(uint8_t i = 0; i < 4; i++){
    addr += octets[i] * (1 << (24 - (8 * i)));
  }

  return make_v4prefix_i(addr, octets[4]);
}

struct v4prefix *make_v4prefix_i(uint32_t net, uint8_t len){
  struct v4prefix *prefix = malloc_v4prefix();
  prefix->network = net;
  prefix->length = len;
  prefix->tag = 0;
  uint32_t o1 = get_octet(prefix, 1);
  prefix->pclass = get_pclass(o1);
  return prefix;
}

struct v4prefix *get_network(struct v4prefix *prefix){
  struct v4prefix *new = make_v4prefix_i(0, prefix->length);
  /*
   * mask /24 example
   * left shift 1: eg 100000000 (256)
   * subtract 1:   eg 011111111 (255)
   * unary flip:   eg 11111111 11111111 11111111 00000000 (/24 mask)
   */
  uint32_t mask = ~((1 << (32 - prefix->length)) - 1);
  new->network = prefix->network & mask;
  return new;
}

struct v4prefix *get_broadcast(struct v4prefix *prefix){
  struct v4prefix *new = get_network(prefix);
  /*
   * mask /24 example
   * start with network 10101010 01101101 11001111 00000000
   * find number of bits from left, 32 - 24 = 8
   * raise 2^8 and subtract 1 to get all the 1s you need
   * bitwise OR to add these 1s to the existing network
   */ 
  uint8_t from_left = 32 - prefix->length;
  new->network = prefix->network | ((1 << from_left) - 1);
  return new;
}

uint8_t get_octet(struct v4prefix *prefix, uint8_t octet){
  uint32_t mask = 255;
  mask = mask << 8 * (4-octet);
  uint32_t data = prefix->network & mask;
  return data >> 8 * (4-octet);
}

uint32_t get_usable_ip(struct v4prefix *prefix){
  uint8_t len = prefix->length;
  if(len < 31){
    return (1 << (32 - len)) - 2;
  }
  // for /32 return 1. for /31 return 2 (special cases)
  return 33 - len;
}

void to_string(struct v4prefix *prefix){
  uint8_t p[4], n[4], b[4];
  struct v4prefix *net = get_network(prefix);
  struct v4prefix *bcast = get_broadcast(prefix);
  for(uint8_t i = 0; i < 4; i++){
    p[i] = get_octet(prefix, i+1);
    n[i] = get_octet(net, i+1);
    b[i] = get_octet(bcast, i+1);
  }
  printf("%d.%d.%d.%d/%d", p[0], p[1], p[2], p[3], prefix->length);
  printf(",net: %d.%d.%d.%d", n[0], n[1], n[2], n[3]);
  //printf(",tag: %d", prefix->tag);
  printf(",bcast: %d.%d.%d.%d", b[0], b[1], b[2], b[3]);
  printf(",usable: %d\n", get_usable_ip(prefix));
  free_v4prefix(bcast);
  free_v4prefix(net);
}
