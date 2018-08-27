#include<stdint.h>
#include<stdlib.h>
#include"v4prefix.h"

const uint8_t ASCII_OFFSET = 48;
long V4P_S = sizeof(struct v4prefix);
uint32_t v4prefix_cnt = 0;

struct v4prefix *malloc_v4prefix(){
  struct v4prefix *prefix = malloc(V4P_S);
  v4prefix_cnt++;
  fprintf(stderr, "grab %ld bytes at %p (ptr: %p) cnt: %u\n",
    V4P_S, prefix, &prefix, v4prefix_cnt);
  return prefix;
}

void free_v4prefix(struct v4prefix *prefix){
  v4prefix_cnt--;
  fprintf(stderr, "free %ld bytes at %p (ptr: %p) cnt: %u\n",
    V4P_S, prefix, &prefix, v4prefix_cnt);
  if(v4prefix_cnt == 0){
    fprintf(stderr, "all v4prefix structs freed; no memory leak\n");
  }
  free(prefix);
}

// ascii 48 -> 0, 57 -> 9
uint8_t int_from_ascii(char ascii_char){
  return ascii_char - ASCII_OFFSET;
}

uint8_t string_len(char *str){
  uint8_t len = 0;
  for(char *i = str; *i != 0; i++){
    len++;
  }
  fprintf(stderr, "string_len(%s): %u\n", str, len);
  return len;
}

uint8_t exponent(uint8_t base, uint8_t exp){
  uint8_t product = 1;
  for(uint8_t i = 0; i < exp; i++){
    product *= base;
  }
  fprintf(stderr, "%u^%u: %u\n", base, exp, product);
  return product;
}

uint8_t int_from_octetstr(char *octetstr){
  uint8_t sum = 0;
  uint8_t len = string_len(octetstr);
  for(uint8_t i = 0; i < len; i++){
    uint8_t ascii = int_from_ascii(octetstr[i]); 
    sum += ascii * exponent(10, len - i - 1);
  }
  fprintf(stderr, "int_from_octetstr(%s): %u\n", octetstr, sum);
  return sum;
}

void set_tag(struct v4prefix *prefix, uint16_t tag){
  prefix->tag = tag;
}

prefix_class get_pclass(uint8_t o1){
  prefix_class c;
  if(o1 < 128){
    c = 'A';
  } else if(o1 >= 128 && o1 < 192){
    c = 'B';
  } else if(o1 >=192 && o1 < 224){
    c = 'C';
  } else if(o1 >= 224 && o1 < 240){
    c = 'D';
  } else {
    c = 'E';
  }
  fprintf(stderr, "get_pclass(%u): %c\n", o1, c);
  return c;
}

struct v4prefix *make_v4prefix(char *input){
  uint8_t octets[5]; // 5th octet holds the prefix length
  char *walk = input;
  for(uint8_t o = 0; o < 5; o++){
    char buf[4] = {0, 0, 0, 0}; // initialize array as all-null
    uint8_t i = 0;
    while(*walk != '.' && *walk != '/' && *walk != 0){
      buf[i] = *walk;
      fprintf(stderr, "buf[%u]: %c\n", i, buf[i]);
      i++;
      walk++;
    }
    octets[o] = int_from_octetstr(buf);
    fprintf(stderr, "octets[%u]: %u\n", o, octets[o]);
    walk++;
  }

  // convert uint8_t[4] to uint32_t
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
  /*
   * Start with eight 1s (11111111) to slide around
   * Move it to the left to get a specific octet
   * Bitwise AND to grab only that octet
   * Move it to the right to make it a uint8_t
   */
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

void print_cidr(struct v4prefix *prefix, uint32_t flags){
  uint8_t *p = (uint8_t *)&prefix->network;
  /*
   * bit 0: show prefix
   * bit 1: append comma
   * bit 2: free memory
   */
  printf("%u.%u.%u.%u", p[3], p[2], p[1], p[0]);
  if(flags & 1){
    fprintf(stdout, "/%u", prefix->length);
  }
  if(flags & 2){
    fprintf(stdout, ",");
  }
  if(flags & 4){
    free_v4prefix(prefix);
  }
}

void print_all(struct v4prefix *prefix){
  struct v4prefix *net = get_network(prefix);
  struct v4prefix *bcast = get_broadcast(prefix);
  print_cidr(prefix, 3);
  fprintf(stdout, "0x%x,", prefix->network);
  fprintf(stdout, "%c,", prefix->pclass);
  print_cidr(net, 6);
  print_cidr(bcast, 6);
  fprintf(stdout, "%u\n", get_usable_ip(prefix));
}
