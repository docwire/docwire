/// [example_c]
#include "stdio.h"
#include "doctotext_c_api.h"

int main(int argc, char *argv[])
{
   FILE *html_file = fopen("output.html", "w"); // create a file to export html
   FILE *plain_text_file = fopen("output.txt", "w"); // create a file to export plain text

   DocToTextParserManager *manager = doctotext_init_parser_manager("plugins/"); // create a parser manager (load parsers)
   const char *file_name = argv[1];
   DocToTextImporter *importer = doctotext_create_importer_from_file_name(manager, file_name); // create an importer and set the input file
   DocToTextExporter *plain_text_exporter = doctotext_create_plain_text_exporter(plain_text_file); // create an exporter to plain text and set the output stream
   DocToTextExporter *html_exporter = doctotext_create_html_exporter(html_file); // create an exporter to html and set the output stream
   DocToTextParsingChain *chain_1 = doctotext_connect_importer_to_exporter(importer, plain_text_exporter); // create a parsing chain by connecting importer and plain text exporter (This step starts the parsing chain)
   DocToTextParsingChain *chain_2 = doctotext_connect_importer_to_exporter(importer, html_exporter); // create a second parsing chain by connecting importer and html exporter (This step starts the parsing chain)
   doctotext_free_importer(importer); // free importer
   doctotext_free_exporter(plain_text_exporter); // free plain text exporter
   doctotext_free_exporter(html_exporter); // free html exporter
   doctotext_free_parsing_chain(chain_1); // free parsing chain
   doctotext_free_parsing_chain(chain_2); // free parsing chain
   return 0;
}
/// [example_c]