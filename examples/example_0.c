/// [example_c]
#include "stdio.h"
#include "doctotext_c_api.h"

int main(int argc, char *argv[])
{
  DocToTextParserManager *manager = doctotext_init_parser_manager(); // create a parser manager (load parsers)
  const char *file_name = argv[1];
  DocToTextImporter *importer = doctotext_create_importer_from_file_name(manager, file_name); // create an importer and set the input file
  DocToTextExporter *exporter = doctotext_create_plain_text_exporter(stdout); // create an exporter to plain text and set the output stream
  DocToTextParsingChain *chain = doctotext_connect_importer_to_exporter(importer, exporter); // create a parsing chain by connecting importer and exporter (This step starts the parsing chain)
  doctotext_free_importer(importer); // free importer
  doctotext_free_exporter(exporter); // free exporter
  doctotext_free_parsing_chain(chain); // free parsing chain
  return 0;
}
/// [example_c]
