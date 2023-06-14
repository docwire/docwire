/// [example_c]
#include "stdio.h"
#include "doctotext_c_api.h"

int main(int argc, char *argv[])
{
   FILE *html_file = fopen("output.html", "w"); // create a file to export html
   FILE *plain_text_file = fopen("output.txt", "w"); // create a file to export plain text

   DocToTextParserManager *manager = doctotext_init_parser_manager("plugins/"); // create a parser manager (load parsers)
   const char *file_name = argv[1];
   DocToTextInput *input = doctotext_create_input_from_file_name(file_name); // create an input and set the input file
   DocToTextImporter *importer = doctotext_create_importer(manager); // create an importer
   DocToTextExporter *plain_text_exporter = doctotext_create_plain_text_exporter(plain_text_file); // create an exporter to plain text and set the output stream
   DocToTextExporter *html_exporter = doctotext_create_html_exporter(html_file); // create an exporter to html and set the output stream
   DocToTextParsingChain *chain_1 = doctotext_connect_parsing_chain_to_exporter(doctotext_connect_input_to_importer(input, importer), plain_text_exporter); // create a parsing chain by connecting input, importer and plain text exporter (This step starts the parsing chain)
   DocToTextParsingChain *chain_2 = doctotext_connect_parsing_chain_to_exporter(doctotext_connect_input_to_importer(input, importer), html_exporter); // create a second parsing chain by connecting input, importer and html exporter (This step starts the parsing chain)
   doctotext_free_input(input); // free input
   doctotext_free_importer(importer); // free importer
   doctotext_free_exporter(plain_text_exporter); // free plain text exporter
   doctotext_free_exporter(html_exporter); // free html exporter
   doctotext_free_parsing_chain(chain_1); // free parsing chain
   doctotext_free_parsing_chain(chain_2); // free parsing chain
   return 0;
}
/// [example_c]