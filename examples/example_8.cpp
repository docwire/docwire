/// [example_cpp]
#include <iostream>

#include "parser.h"
#include "simple_extractor.h"

/**
 * @example example_8.cpp
 */
int main(int argc, char* argv[])
{
   doctotext::SimpleExtractor simple_extractor(argv[1]); // create a simple extractor
   simple_extractor.addCallbackFunction([](doctotext::Info &info)
                                       {
                                         if (info.tag_name == doctotext::StandardTag::TAG_MAIL) // if current node is mail
                                         {
                                           auto date = info.getAttributeValue<int>("date"); // get the date attribute
                                           if (date) // if date attribute exists
                                           {
                                             if (*date < 1651437232) // if date is less than 01.05.2022 (1651437232 is the unix timestamp of 01.05.2022)
                                             {
                                               info.skip = true; // skip the current node
                                             }
                                           }
                                         }
                                       });
   std::cout << simple_extractor.getPlainText(); // print the plain text of the document
}
/// [example_cpp]
