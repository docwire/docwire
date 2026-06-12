#include "docwire.h"
#include "local_ai_task.h"
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
    using namespace docwire;
    std::stringstream out_stream;
    try {
        std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser() |
            PlainTextExporter() |
            ai::local::task("Write a short summary for this text:\n\n") | out_stream;
    } catch (const std::exception& e) {
        std::cerr << errors::diagnostic_message(e) << std::endl;
        return 1;
    }
    std::cout << out_stream.str() << std::endl;

    return 0;
}
