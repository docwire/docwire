#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  try
  {
    std::filesystem::path("1.pst") | content_type::detector{} | mail_parser{} | office_formats_parser{} |
      [](message_ptr msg, const message_callbacks& emit_message) // Create an input from file path, parser and connect them to transformer
      {
        if (msg->is<mail::mail>()) // if current node is mail
        {
          auto subject = msg->get<mail::mail>().subject; // get the subject attribute
          if (subject) // if subject attribute exists
          {
            if (subject->find("Hello") != std::string::npos) // if subject contains "Hello"
            {
              return continuation::skip; // skip the current node
            }
          }
        }
        return emit_message(std::move(msg));
      } |
      [counter = 0, max_mails = 3](message_ptr msg, const message_callbacks& emit_message) mutable // Create a transformer and connect it to previous transformer
      {
        if (msg->is<mail::mail>() && ++counter > max_mails) // if counter is greater than max_mails
            return continuation::stop; // cancel the parsing process
        return emit_message(std::move(msg));
      } |
      plain_text_exporter() | // sets exporter to plain text
      std::cout;
  } catch (const std::exception& e) { std::cerr << errors::diagnostic_message(e) << std::endl; return 1; }
 return 0;
}
