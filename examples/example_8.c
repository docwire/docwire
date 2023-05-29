/// [example_c]
#include <stdbool.h>
#include "stdio.h"
#include "string.h"
#include "doctotext_c_api.h"

void filterMailsByDate(DocToTextInfo* info, void* data) // callback function to filter by date
{
  const char * tag_name = doctotext_info_get_tag_name(info); // get the tag name of current node
  if (strcmp(tag_name, "mail-header") == 0) // if current node is mail header
  {
    unsigned int date = doctotext_info_get_uint_attribute(info, "date"); // get the date attribute
    if (date < 1651437232) // if date is less than 01.05.2022 (1651437232 is the unix timestamp of 01.05.2022)
    {
      doctotext_info_set_skip(info, true); // skip the current node
    }
  }
}

int main(int argc, char *argv[])
{
  DocToTextSimpleExtractor* extractor = doctotext_create_simple_extractor(argv[1]); // create a simple extractor
  if (extractor) // if extractor exists
  {
     doctotext_simple_extractor_add_callback_function(extractor, filterMailsByDate, NULL); // set the filter function
     const char* text = doctotext_simple_extractor_get_plain_text(extractor); // get the plain text of the document. Call this function cause starts the parsing process.
     printf("%s", text); // print the plain text
  }
  return 0;
}
/// [example_c]