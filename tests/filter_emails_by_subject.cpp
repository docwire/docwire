#include "docwire.h"
#include <iostream>
#include <filesystem>
#include <stdexcept>

int main(int argc, char* argv[])
{
  using namespace docwire;
  try
  {
    std::filesystem::path("1.pst") | content_type::detector{} | mail_parser{} | office_formats_parser{}
      |  // Create an input from file path, parser and connect them to transformer
        [](message_ptr msg, const message_callbacks& emit_message)
        {
          if (msg->is<mail::mail>()) // if current node is mail
          {
            auto subject = msg->get<mail::mail>().subject; // get the subject attribute
            if (subject && subject->find("Hello") != std::string::npos) // if subject contains "Hello"
            {
              return continuation::skip; // skip the current node
            }
          }
          return emit_message(std::move(msg));
        }
      | plain_text_exporter() | std::cout;
  }
  catch (const std::exception& e) { std::cerr << errors::diagnostic_message(e) << std::endl; return 1; }
  return 0;
}
