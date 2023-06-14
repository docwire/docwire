/// [example_c]
#include "stdio.h"
#include "doctotext_c_api.h"

int main(int argc, char *argv[])
{
   DocToTextParserManager *manager = doctotext_init_parser_manager("plugins/"); // create a parser manager (load parsers)
   const char *file_name = argv[1];
   DocToTextInput *input = doctotext_create_input_from_file_name(file_name);
   DocToTextImporter *importer = doctotext_create_importer(manager);
   DocToTextExporter *exporter = doctotext_create_plain_text_exporter(stdout); // create an exporter to plain text and set the output stream
   DocToTextParsingChain *parsing_chain = doctotext_connect_parsing_chain_to_exporter(doctotext_connect_input_to_importer(input, importer), exporter); // create a parsing chain by connecting input, importer and exporter
                                                                                                          // (This step doesn't start the parsing chain because the input is not set)
   for (int i = 1; i < argc; i++) // iterate over all files
   {
     FILE *file = fopen(argv[i], "r"); // open the file
     doctotext_parsing_chain_set_input(parsing_chain, file); // set the input file (This step starts the parsing chain for the current file)
     fclose(file); // close the file
   }

   doctotext_free_input(input); // free input
   doctotext_free_importer(importer); // free importer
   doctotext_free_exporter(exporter); // free exporter
   doctotext_free_parsing_chain(parsing_chain); // free parsing chain
   return 0;
}
/// [example_c]