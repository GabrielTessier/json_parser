
/*
  JSON grammar : https://www.crockford.com/mckeeman.html
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <json.h>
#include <stdlib.h>

struct file_reader_s {
  FILE *file;
  char c;
};
typedef struct file_reader_s file_reader_t;

static json_element_t *parse_element(file_reader_t *file);
static bool is_wp(char c);
static void parse_wp(file_reader_t *file);
static enum json_type_e get_value_type(file_reader_t *file);
static json_object_t *parse_object(file_reader_t *file);
static json_member_t *parse_member(file_reader_t *file);
static json_array_t *parse_array(file_reader_t *file);
static char *parse_string(file_reader_t *file);
static char parse_4hex(file_reader_t *file);
static json_number_t parse_number(file_reader_t *file);
static bool parse_boolean(file_reader_t *file);

enum error_e { VALID = 0, INVALID_CHAR };
enum error_e error = 0;

static char next_char(file_reader_t *file) {
  file->c = fgetc(file->file);
  return file->c;
}
static char current_char(file_reader_t *file) { return file->c; }
static char next_n_char(file_reader_t *file, int n) {
  fseek(file->file, n-1, SEEK_CUR);
  return next_char(file);
}
static char prev_n_char(file_reader_t *file, int n) {
  fseek(file->file, -n-1, SEEK_CUR);
  return next_char(file);
}

json_element_t *json_read_file(char *filename) {
  FILE* file = fopen(filename, "r");
  file_reader_t file_reader = {.file = file, .c = fgetc(file)};
  json_element_t *element = parse_element(&file_reader);
  fclose(file);
  if (error) {
    printf("ERROR\n");
  }
  return element;
}

static json_element_t *parse_element(file_reader_t *file) {
  json_element_t *element = (json_element_t*) malloc(sizeof(json_element_t));
  parse_wp(file);

  enum json_type_e type = get_value_type(file);
  element->type = type;
  
  switch (type) {
  case JNULL:
    // "null"
    //fseek(file->file, 6, SEEK_CUR);
    next_n_char(file, 6);
    break;
  case OBJECT:
    element->object = parse_object(file);
    break;
  case ARRAY:
    element->array = parse_array(file);
    break;
  case STRING:
    element->string = parse_string(file);
    break;
  case NUMBER:
    element->number = parse_number(file);
    break;
  case BOOL:
    element->boolean = parse_boolean(file);
    break;
  }

  parse_wp(file);
  
  return element;
}

static bool is_wp(char c) {
  return c == '\n' || c == '\t' || c == '\r' || c == ' ';
}

static void parse_wp(file_reader_t *file) {
  char c = current_char(file);
  while (is_wp(c)) {
    c = next_char(file);
  }
}

static enum json_type_e get_value_type(file_reader_t *file) {
  char str[6];
  switch (current_char(file)) {
  case '{' : return OBJECT;
  case '[': return ARRAY;
  case'"' :
    str[0] = next_char(file);
    enum json_type_e type = STRING;
    if (str[0] == 't') { // true
      fgets(str, 5, file->file);
      if (strncmp(str, "rue\"", 4) == 0) type = BOOL;
      //fseek(file->file, -4, SEEK_CUR);
      prev_n_char(file, 4);
    } else if (str[0] == 'f') { // false
      fgets(str, 6, file->file);
      if (strncmp(str, "alse\"", 5) == 0) type = BOOL;
      //fseek(file->file, -5, SEEK_CUR);
      prev_n_char(file, 5);
    } else if (str[0] == 'n') { // null
      fgets(str, 5, file->file);
      if (strncmp(str, "ull\"", 4) == 0) type = JNULL;
      //fseek(file->file, -4, SEEK_CUR);
      prev_n_char(file, 4);
    }
    //fseek(file->file, -1, SEEK_CUR);
    prev_n_char(file, 1);
    return type;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '-': return NUMBER;
  default:
    // ERROR
    error = INVALID_CHAR;
    return JNULL;
  }
}

static json_object_t *parse_object(file_reader_t *file) {
  next_char(file);
  parse_wp(file);
  
  json_object_t *object = (json_object_t*) malloc(sizeof(json_object_t));
  object->members = NULL;
  object->nb_members = 0;

  while (current_char(file) != '}') {
    json_member_t *member = parse_member(file);
    object->nb_members++;
    object->members = realloc(object->members, object->nb_members * sizeof(json_member_t*));
    object->members[object->nb_members-1] = member;
    if (current_char(file) != ',' && current_char(file) != '}') {
      error = INVALID_CHAR;
      return object;
    }
    if (current_char(file) == ',') next_char(file);
  }
  next_char(file);
  return object;
}

static json_member_t *parse_member(file_reader_t *file) {
  parse_wp(file);
  json_member_t *member = (json_member_t*) malloc(sizeof(json_member_t));
  member->name = parse_string(file);
  parse_wp(file);
  if (current_char(file) != ':') {
    error = INVALID_CHAR;
    return member;
  }
  next_char(file);
  member->element = parse_element(file);
  return member;
}

static json_array_t *parse_array(file_reader_t *file) {
  next_char(file);
  parse_wp(file);

  json_array_t *array = (json_array_t*) malloc(sizeof(json_array_t));
  array->nb_elements = 0;
  array->elements = NULL;

  while (current_char(file) != ']') {
    json_element_t *element = parse_element(file);
    array->nb_elements++;
    array->elements = realloc(array->elements, array->nb_elements * sizeof(json_array_t*));
    array->elements[array->nb_elements-1] = element;
    if (current_char(file) != ',' && current_char(file) != ']') {
      error = INVALID_CHAR;
      return array;
    }
    if (current_char(file) == ',') next_char(file);
  }
  next_char(file);
  return array;
}

static char *parse_string(file_reader_t *file) {
  next_char(file);

  int size = 0;
  bool end = false;
  while (!end) {
    if (current_char(file) == '"') end = true;
    else {
      if (current_char(file) == '\\') next_char(file);
      size++;
      next_char(file);
    }
  }
  //fseek(file->file, -size, SEEK_CUR);
  prev_n_char(file, size);

  size++; // pour le \0
  char *str = (char*) malloc(sizeof(char)*size);
  end = false;
  int i = 0;
  while (!end) {
    if (current_char(file) == '"') end = true;
    else if (current_char(file) == '\\') {
      char c = next_char(file);
      switch (c) {
      case '"' : str[i] = '"'; break;
      case '\\': str[i] = '\\'; break;
      case '/' : str[i] = '/'; break;
      case 'b' : str[i] = '\b'; break;
      case 'f' : str[i] = '\f'; break;
      case 'n' : str[i] = '\n'; break;
      case 'r' : str[i] = '\r'; break;
      case 'u' :
	next_char(file);
	str[i] = parse_4hex(file);
	break;
      default  : error = INVALID_CHAR; return str;
      }
    } else {
      str[i] = current_char(file);
    }
    i++;
    next_char(file);
  }
  //next_char(file);
  str[i] = 0;
  return str;
}

static int hex_to_int(char c) {
  if (c >= '0' && c <= '9') return c-'0';
  switch (c) {
  case 'a' : return 10;
  case 'b' : return 11;
  case 'c' : return 12;
  case 'd' : return 13;
  case 'e' : return 14;
  case 'f' : return 15;
  }
  error = INVALID_CHAR;
  return 0;
}

static char parse_4hex(file_reader_t *file) {
  char c = 0;
  for (int i=0; i<4; i++) {
    c *= 16;
    c += hex_to_int(current_char(file));
    next_char(file);
  }
  return c;
}

static json_number_t parse_number(file_reader_t *file) {
  int number_int = 0;
  bool neg = current_char(file) == '-';
  enum number_type_e type = INT;
  double number_double = 0;
  double exponent = 1;  
  if (neg) next_char(file);
  while (current_char(file) >= '0' && current_char(file) <= '9') {
    number_int *= 10;
    number_int += current_char(file) - '0';
    next_char(file);
  }
  if (current_char(file) == '.') {
    next_char(file);
    type = FLOAT;
    if (neg) number_double = -number_int;
    else number_double = number_int;
    double p10 = 0.1; // puissance de 10 du prochain digit
    while (current_char(file) >= '0' && current_char(file) <= '9') {
      number_double += p10 * (current_char(file) - '0');
      p10 /= 10;
      next_char(file);
    }
  }
  if (current_char(file) == 'e' || current_char(file) == 'E') {
    next_char(file);
    int pexp = 0;
    bool exp_neg = false;
    if (current_char(file) == '+') next_char(file);
    if (current_char(file) == '-') {
      exp_neg = true;
      next_char(file);
    }
    while (current_char(file) >= '0' && current_char(file) <= '9') {
      pexp *= 10;
      pexp += current_char(file) - '0';
      next_char(file);
    }
    while (pexp != 0) {
      if (exp_neg) exponent /= 10;
      else exponent *= 10;
      pexp--;
    }
  }

  json_number_t number;
  if (type == INT) {
    if (number_int * exponent >= 1) {
      number.type = INT;
      number.integer = number_int * exponent;
    } else {
      number.type = FLOAT;
      number.floating = (double) number_int * exponent;
    }
  } else if (type == FLOAT) {
    number.type = FLOAT;
    number.floating = number_double * exponent;
  }
  return number;
}

static bool parse_boolean(file_reader_t *file) {
  next_char(file);
  if (current_char(file) == 't') {
    //fseek(file->file, 5, SEEK_CUR);
    next_n_char(file, 5);
    return true;
  } else if (current_char(file) == 'f') {
    //fseek(file->file, 6, SEEK_CUR);
    next_n_char(file, 6);
    return false;
  }
  error = INVALID_CHAR;
  return false;
}
