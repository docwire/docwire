/// [example_c]
#include <stdbool.h>
#include "stdio.h"
#include "string.h"
#include "doctotext_c_api.h"

int main(int argc, char *argv[])
{
  DocToTextSimpleExtractor* extractor = doctotext_create_simple_extractor(argv[1]); // create a simple extractor
  if (extractor) // if extractor exists
  {
     const char* text = doctotext_simple_extractor_get_plain_text(extractor); // get the plain text of the document. Call this function cause starts the parsing process.
     printf("%s", text); // print the plain text
  }
  return 0;
}
/// [example_c]