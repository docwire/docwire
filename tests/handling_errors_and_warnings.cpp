#define DOCWIRE_ENABLE_SHORT_MACRO_NAMES

#include "docwire.h"
#include "error_tags.h"
#include "error.h"
#include "make_error.h"
#include "contains_type.h"
#include <filesystem>
#include <iostream>
#include <sstream>

void retry()
{
  // Simulated retry logic for demonstration purposes.
}

void demonstrate_error_handling()
{
  using namespace docwire;

  // Snippet 1: categorized and tagged errors
  try
  {
    throw make_error(errors::network_failure{});
  }
  catch (const docwire::errors::base& e)
  {
    if (docwire::errors::contains_type<docwire::errors::network_failure>(e))
      retry();
  }

  // Snippet 2: embedded source location
  try
  {
    throw make_error("Demonstration error");
  }
  catch (const docwire::errors::base& e)
  {
    std::cerr << e.location.file_name() << ":" << e.location.line() << std::endl;
  }

  // Snippet 3: secure context retrieval
  try
  {
    throw make_error(std::filesystem::path{"/secret/file.txt"});
  }
  catch (docwire::errors::base& e)
  {
    std::cerr << docwire::errors::diagnostic_message(e) << std::endl;
    // or
    if (auto* impl_ptr = dynamic_cast<docwire::errors::impl<std::filesystem::path>*>(&e))
    {
      auto fn = impl_ptr->context;
      (void)fn; // avoid unused variable warning
    }
  }
}

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") |
      content_type::by_file_extension::detector{} |
      office_formats_parser{} |
      plain_text_exporter() |
      [](message_ptr msg, const message_callbacks& emit_message)
	    {
	      if (msg->is<std::exception_ptr>())
        {
		      std::clog << "[WARNING] " <<
            errors::diagnostic_message(msg->get<std::exception_ptr>()) <<
            std::endl;
        }
        return emit_message(std::move(msg));
	    } |
      out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << "[ERROR] " << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  demonstrate_error_handling();

  return 0;
}
