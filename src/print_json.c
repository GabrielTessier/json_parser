
#include <string.h>
#include <stdio.h>
#include <json.h>

static void print_tab(int tab);
static void new_line(int tab);

static void print_element(json_element_t *element, int tab);
static void print_object(json_object_t *object, int tab);
static void print_member(json_member_t *member, int tab);
static void print_array(json_array_t *array, int tab);
static void print_string(char *str, int tab);
static void print_number(json_number_t number, int tab);
static void print_bool(bool b, int tab);
static void print_null(int tab);

void json_print(json_element_t *element) { print_element(element, 0); }

static void print_tab(int tab) {
  for (int i=0; i<tab; i++) printf(" ");
}

static void new_line(int tab) {
  printf("\n");
  print_tab(tab);
}

static void print_element(json_element_t *element, int tab) {
  switch (element->type) {
  case OBJECT: print_object(element->object, tab); break;
  case ARRAY : print_array(element->array, tab); break;
  case STRING: print_string(element->string, tab); break;
  case NUMBER: print_number(element->number, tab); break;
  case BOOL  : print_bool(element->boolean, tab); break;
  case JNULL : print_null(tab); break;
  }
}

static void print_object(json_object_t *object, int tab) {
  if (object->nb_members == 0) {
    printf("{ }");
  } else {
    printf("{");
    for (int i=0; i<object->nb_members; i++) {
      new_line(tab+2);
      print_member(object->members[i], tab+2);
      if (i != object->nb_members-1) printf(",");
    }
    new_line(tab);
    printf("}");
  }
}

static void print_member(json_member_t *member, int tab) {
  print_string(member->name, tab);
  printf(" : ");
  print_element(member->element, tab+3+strlen(member->name)+2);
}

static void print_array(json_array_t *array, int tab) {
  if (array->nb_elements == 0) {
    printf("[ ]");
  } else {
    printf("[");
    for (int i=0; i<array->nb_elements; i++) {
      new_line(tab+2);
      print_element(array->elements[i], tab+2);
      if (i != array->nb_elements-1) printf(",");
    }
    new_line(tab);
    printf("]");
  }
}

static void print_string(char *str, int tab) {
  printf("\"%s\"", str);
}

static void print_number(json_number_t number, int tab) {
  if (number.type == INT) printf("%ld", number.integer);
  else if (number.type == FLOAT) printf("%f", number.floating);
}

static void print_bool(bool b, int tab) {
  printf("\"%s\"", b?"true":"false");
}

static void print_null(int tab) {
  printf("\"null\"");
}
