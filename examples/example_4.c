/// [example_c]
#include "stdio.h"
#include <string.h>
#include "doctotext_c_api.h"

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

struct callbackData
{
  int mail_counter;
  int max_mails_number;
};

void mailsLimitation(DocToTextInfo* info, void* data)
{
  struct callbackData* p_callback_data = (struct callbackData*)(data);
  const char * tag_name = doctotext_info_get_tag_name(info); // get the tag name of current node
  if (strcmp(tag_name, "mail-header") == 0)
  {
    if (p_callback_data->mail_counter >= p_callback_data->max_mails_number)
    {
      doctotext_info_set_cancel_parser(info, true);
    }
    else
    {
      p_callback_data->mail_counter++;
    }
  }
}

int main(int argc, char *argv[])
{
  DocToTextParserManager *manager = doctotext_init_parser_manager("plugins/"); // create a parser manager (load parsers)
  const char *file_name = argv[1];
  DocToTextImporter *importer = doctotext_create_importer_from_file_name(manager, file_name); // create an importer from file name
  DocToTextExporter *exporter = doctotext_create_plain_text_exporter(stdout); // create an exporter to plain text and set the output stream
  DocToTextInfo *transformer = doctotext_create_transfomer(filterMailsBySubject, NULL); // create a transformer and set the callback function
  DocToTextParsingChain *chain = doctotext_connect_importer_to_transformer(importer, transformer); // create a parsing chain by connecting importer and transformer
  struct callbackData callback_data; // create a callback data structure
  callback_data.mail_counter = 0; // initialize the mail counter
  callback_data.max_mails_number = 10; // set the maximum number of mails to 10
  DocToTextInfo *transformer2 = doctotext_create_transfomer(mailsLimitation, &callback_data); // create a transformer and set the callback function and callback data
  chain = doctotext_connect_parsing_chain_to_transformer(chain, transformer2); // connect the parsing chain to the transformer
  chain = doctotext_connect_parsing_chain_to_exporter(chain, exporter); // connect the parsing chain to exporter (This step starts the parsing)
  doctotext_free_importer(importer); // free importer
  doctotext_free_transformer(transformer); // free transformer
  doctotext_free_exporter(exporter); // free exporter
  doctotext_free_parsing_chain(chain); // free parsing chain
  return 0;
}
/// [example_c]