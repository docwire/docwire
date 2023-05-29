/// [example_c]
#include <stdbool.h>
#include "stdio.h"
#include "string.h"
#include "doctotext_c_api.h"

struct callbackData
{
  DocToTextWriter *writer;
};

void onNewNodeCallback(DocToTextInfo* info, void* data) // callback function for new node
{
  struct callbackData* p_callback_data = (struct callbackData*)(data); // get callback data
  doctotext_writer_write(p_callback_data->writer, info, stdout); // write the parsed text to the output stream
}

void filterMailsBySubject(DocToTextInfo* info, void* data) // callback function to filter by subject text
{
  const char * tag_name = doctotext_info_get_tag_name(info); // get the tag name of current node
  if (strcmp(tag_name, "mail-header") == 0) // if current node is mail header
  {
    const char *subject = doctotext_info_get_string_attribute(info, "subject"); // get the subject attribute
    if (strstr(subject, "Hello") != 0) // if subject contains "Hello"
    {
      doctotext_info_set_skip(info, true); // skip the current node
    }
  }
}

int main(int argc, char *argv[])
{
  DocToTextParserManager *parser_manager = doctotext_init_parser_manager("plugins/"); // create a parser manager (load parsers)
  DocToTextParser* parser = doctotext_parser_manager_get_parser_by_extension(parser_manager, argv[1]); // get parser from item
  struct callbackData callback_data; // create a callback data structure
  DocToTextWriter* writer = doctotext_create_html_writer(); // create a writer (html writer)
  callback_data.writer = writer;
  doctotext_parser_add_callback_on_new_node(parser, &filterMailsBySubject, NULL); // add callback function for filter by subject text
  doctotext_parser_add_callback_on_new_node(parser, &onNewNodeCallback, &callback_data); // add callback function to write the parsed text to the output stream
  doctotext_writer_write_header(writer, stdout); // write the header of the output file
  doctotext_parser_parse(parser); // parse the document
  doctotext_writer_write_footer(writer, stdout); // write the footer of the output file
  doctotext_free_parser(parser); // free parser
  return 0;
}
/// [example_c]