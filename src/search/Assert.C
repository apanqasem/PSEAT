#include "Assert.h" 

void Assert(int expression, char *msg) {
  if (!expression) {
    fprintf(stderr, "Error: %s\n", msg);
    assert(expression);
  }
}
