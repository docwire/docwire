#include "docwire/docwire.h"
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
    using namespace docwire;
    std::stringstream out_stream;
    try {
        std::filesystem::path file_path = "../data_processing_definition.doc";
        if (!std::filesystem::exists(file_path)) {
            std::cerr << "Error: File not found at " << file_path << std::endl;
            return 1;
        }
        file_path | content_type::detector{} | office_formats_parser() |
            PlainTextExporter() |
            local_ai::model_chain_element("Write a short summary for this text:\n\n") | out_stream;
    } catch (const std::exception& e) {
        std::cerr << errors::diagnostic_message(e) << std::endl;
        return 1;
    }
    std::cout << out_stream.str() << std::endl;

    return 0;
}
