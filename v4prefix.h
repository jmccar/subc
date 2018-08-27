#include<stdint.h>
#include<stdio.h>

typedef char prefix_class;

struct v4prefix {
  uint32_t network;
  uint16_t tag;
  uint8_t length;
  prefix_class pclass;
};

struct v4prefix *make_v4prefix(char *);

struct v4prefix *make_v4prefix_i(uint32_t, uint8_t);

void set_tag(struct v4prefix *, uint16_t);

uint8_t int_from_octetstr(char *);

void free_v4prefix(struct v4prefix *);

uint8_t get_octet(struct v4prefix *, uint8_t);

struct v4prefix *get_network(struct v4prefix *);

void print_all(struct v4prefix *);
