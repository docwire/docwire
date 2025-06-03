#include "docwire.h"
#include <cassert>
#include <fstream>

int main()
{
  using namespace docwire;
  std::stringstream out_stream;
  const std::string expected_output_1 = "Data processing refers to the activities performed on raw data to convert it into meaningful information. It involves collecting, organizing, analyzing, and interpreting data to extract useful insights and support decision-making. This can include tasks such as sorting, filtering, summarizing, and transforming data through various computational and statistical methods. Data processing is essential in various fields, including business, science, and technology, as it enables organizations to derive valuable knowledge from large datasets, make informed decisions, and improve overall efficiency.\n\n";
  const std::string expected_output_2 = "<http://www.silvercoders.com/>hyperlink test\n\n";

  try
  {
    // Perform file type determination and output the file types without also performing document conversion.
    data_source data{seekable_stream_ptr{
      std::make_shared<std::ifstream>("data_processing_definition.doc", std::ios::binary)
    }};
    content_type::detect(data);
    std::optional<mime_type> mt = data.highest_confidence_mime_type();
    assert(mt.has_value());
    assert(*mt == mime_type { "application/msword" });

    // Bypass file type determination and rely on provided mime-type
    data_source
    {
      seekable_stream_ptr{std::make_shared<std::ifstream>("data_processing_definition.doc", std::ios::binary)},
      mime_type{"application/msword"},
      confidence::high
    } |
    office_formats_parser{} | // more parsers can be added
    PlainTextExporter() | out_stream;
    assert(out_stream.str() == expected_output_1);
    out_stream.str("");

    // Perform file type determination based on provided file extension
    data_source
    {
      seekable_stream_ptr{std::make_shared<std::ifstream>("data_processing_definition.doc", std::ios::binary)},
      file_extension{".doc"}
    } |
    content_type::by_file_extension::detector{} |
    office_formats_parser{} | // more parsers can be added
    PlainTextExporter() | out_stream;
    assert(out_stream.str() == expected_output_1);
    out_stream.str("");

    // Perform file type determination based on file extension extracted from file name
    std::filesystem::path{"data_processing_definition.doc"} |
    content_type::by_file_extension::detector{} |
    office_formats_parser{} | // more parsers can be added
    PlainTextExporter() | out_stream;
    assert(out_stream.str() == expected_output_1);
    out_stream.str("");

    // Perform file type determination on a Zip file without also performing it on its contents. 
    data_source zip_data{seekable_stream_ptr{
      std::make_shared<std::ifstream>("test.zip", std::ios::binary)
    }};
    content_type::detect(zip_data);
    std::optional<mime_type> zip_data_mt = zip_data.highest_confidence_mime_type();
    assert(zip_data_mt.has_value());
    assert(*zip_data_mt == mime_type { "application/zip" });

    // Perform file type determination on all files within a zip and selectively performing document conversion based on the returned file types
    std::filesystem::path{"test.zip"} |
    content_type::detector{} |
    DecompressArchives{} |
    [](Tag&& tag, const emission_callbacks& emit_tag)
    {
      if (!std::holds_alternative<data_source>(tag))
        return emit_tag(std::move(tag));
      data_source& data = std::get<data_source>(tag);
      std::optional<mime_type> mt = data.highest_confidence_mime_type();
      if (!mt || *mt != mime_type { "application/msword" })
        return continuation::skip;
      return emit_tag(std::move(tag));
    } |
    office_formats_parser{} | // more parsers can be added
    PlainTextExporter() | out_stream;
    assert(out_stream.str() == expected_output_2);
    out_stream.str("");
    
    // ... or
    std::filesystem::path{"test.zip"} |
    content_type::detector{} |
    DecompressArchives{} |
    DOCParser{} | // other formats will be skipped
    PlainTextExporter() | out_stream;
    assert(out_stream.str() == expected_output_2);
  }
  catch (const std::exception& e)
  {
    std::cerr << "[ERROR] " << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
