#pragma once

#include <stdint.h>
#include <stdbool.h>

enum json_type_e { JNULL, OBJECT, ARRAY, STRING, NUMBER, BOOL };
enum number_type_e { INT, FLOAT };

struct json_member_s;
typedef struct json_member_s json_member_t;
struct json_object_s;
typedef struct json_object_s json_object_t;
struct json_array_s;
typedef struct json_array_s json_array_t;
struct json_element_s;
typedef struct json_element_s json_element_t;
struct json_number_s;
typedef struct json_number_s json_number_t;


struct json_member_s {
  char *name;
  json_element_t *element;
};

struct json_object_s {
  int nb_members;
  json_member_t **members;
};

struct json_array_s {
  int nb_elements;
  json_element_t **elements;
};

struct json_number_s {
  enum number_type_e type;
  uint64_t integer;
  double floating;
};

struct json_element_s {
  enum json_type_e type;
  json_object_t *object;
  json_array_t *array;
  char *string;
  json_number_t number;
  bool boolean;
};

json_element_t *json_read_file(char *filename);
void json_print(json_element_t *element);
