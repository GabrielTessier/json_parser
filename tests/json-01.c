
#include <json.h>

int main(void) {
  json_element_t *json = json_read_file("test1.json");
  json_print(json);
  return 0;
}
