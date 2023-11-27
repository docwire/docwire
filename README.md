# DocWire SDK

DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing. Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining, document analysis. Offline processing possible for security and confidentiality.

**Embark on an exciting journey with DocWire SDK, where the philosophies of C++ converge with the power of data processing innovation. Explore the limitless possibilities today and witness the substantial enhancements that set it apart from its predecessor, DocToText.**

Explore the latest updates, contribute to the community, and find the most up-to-date information on our [GitHub project page](https://github.com/docwire/docwire).

## Table Of Contents
- [The Goal](#the-goal)
- [Features](#features)
- [Examples](#examples)
- [Awards](#awards)
- [Installation](#installation)
- [API documentation](#api-documentation)
- [License](#license)
- [Authors](#authors)
- [External links](#external-links)

<a name="the-goal"></a>
## The Goal

Introducing DocWire SDK: Revolutionizing Data Processing in C++17/20

Unlock the future of data processing with the DocWire SDK, an award-winning, cutting-edge framework that builds upon the legacy of the acclaimed DocToText library. Committed to pushing the boundaries of modern C++ dialects, we are dedicated to enhancing productivity, simplifying development, and delivering a seamless experience for both developers and businesses.

### The Power of C++

In the dynamic landscape of data processing, the selection of a programming language is pivotal. We proudly embrace C++ as the backbone of DocWire SDK for several compelling reasons:
- **C++ is renowned for its high performance**, making it the language of choice for applications where speed is paramount. DocWire SDK leverages this performance to ensure swift and efficient data processing, even with large datasets.
- **C++ offers a fine balance between low-level memory manipulation and high-level abstractions**. This versatility allows us to create a robust and flexible SDK that caters to a wide range of data processing needs, from simple tasks to complex algorithms.
- **Efficiency Through "You Only Pay for What You Use"**: C++ adheres to the philosophy of "you only pay for what you use," emphasizing efficiency by enabling developers to have fine-grained control over resources. This philosophy translates into a lean and optimized SDK, ensuring that users can tailor their data processing solutions without unnecessary overhead.
- **Static Typing for Robustness**: The static typing inherent in C++ contributes to the robustness of DocWire SDK. By catching potential errors at compile-time, developers can build more reliable and maintainable data processing applications.
- **Multiparadigm Approach**: C++ is celebrated for its multiparadigm nature, allowing developers to seamlessly blend procedural, object-oriented, and generic programming styles. This flexibility aligns perfectly with the diverse requirements of data processing, ensuring that DocWire SDK accommodates a broad spectrum of development needs.
- **Industry Proven**: C++ is the language of choice for well-known companies in the development of serious, mission-critical, and performance-sensitive applications. By leveraging the robustness and scalability of C++, DocWire SDK positions itself as a trusted solution for demanding data processing tasks, ensuring reliability in mission-critical scenarios.

### For Developers: Elevate Your Capabilities

If you've experienced the power of DocToText, brace yourself for a substantial upgrade. Staying ahead with C++17 and soon C++20, we integrate advanced features such as ranges and concurrency. DocWire SDK evolves to handle a spectrum of data processing tasks, including robust HTTP capabilities, comprehensive network APIs, and efficient web data extraction.

We understand the need for comprehensive solutions in today's development landscape. DocWire SDK seamlessly integrates into C++ applications, providing solutions for text extraction, data conversion, data scraping, and data mining.

### For Businesses: A Quantum Leap in Data Processing

DocWire SDK transcends being a mere successor; it's a quantum leap in addressing complex data processing tasks. Our support for diverse document formats is expanding, including PDF, DOC, XLS, and more. Extracting insights from email boxes, databases, websites, and harnessing the latest AI features are pivotal components of our commitment.

### Project Plans for the Future

- **Comprehensive Data Import/Export**: DocWire SDK empowers users to seamlessly import/export data from/to all known sources and destinations, ensuring flexibility and compatibility.
- **Advanced Data Processing**: Harness the power of standard algorithms and AI models within DocWire SDK, offering a spectrum of data processing capabilities for diverse needs.
- **Online Integrations and Offline Processing**: Enjoy the best of both worlds. Access advanced, resource-hungry AI models online, or opt for intranet/offline processing with locally operating AI models, providing flexibility without compromising performance.
- **Multiplatform Support**: DocWire SDK seamlessly integrates across servers, personal computers, and mobile devices, offering a consistent experience across diverse environments.
- **Embedded Opensource Libraries**: We understand the power of existing opensource libraries in speeding up development. DocWire SDK strategically embeds high-quality opensource data processing libraries, ensuring a consistent API, mitigating potential issues and safeguarding license compliance for SDK users.
- **Enhanced Cloud Integration**: Develop features to seamlessly integrate with various cloud platforms, allowing users to leverage cloud resources for scalable and efficient data processing.
- **Improved NLP Integration**: Building on our existing OpenAI integration, we aim to enhance natural language processing capabilities for even more accurate and insightful data extraction.
- **Powerhouse for data processing**: With continued support for various import formats and expanded export options, DocWire SDK is evolving into a powerhouse for data processing.
- **Top-notch Documentation and Support**: Expect nothing but the best. We are committed to providing high-quality documentation and fanatical customer support, ensuring a smooth and efficient experience for every user.

### Examples of Input and Output Sources
- **Email Boxes**: Effortlessly extract valuable information from email boxes, streamlining the integration of email data into your processing workflows.
- **Cloud Drives**: Seamlessly import/export data from popular cloud drives, ensuring that your data processing isn't bound by storage location.
- **Local Filesystem**: Process data directly from local filesystems, providing flexibility in handling data stored locally.
- **ERP Systems**: Integrate with ERP systems for streamlined data processing in enterprise resource planning.
- **Databases**: Connect seamlessly with databases, enabling efficient extraction and manipulation of structured data.
- **Web Services**: Extract data from web services, providing real-time integration capabilities.

### Research and Development Goals

In our pursuit of excellence, DocWire SDK is committed to ongoing Research and Development. Some of our R&D goals include:
- **Enhanced AI Capabilities**: Pushing the boundaries of AI integration, exploring new models, and refining existing ones to provide state-of-the-art data processing.
- **Vision AI Models**: Expand our capabilities to include vision AI models, enabling advanced image and video data processing.
- **Quantum Computing Integration**: Investigate the potential benefits of quantum computing techniques for accelerating data processing tasks and staying at the forefront of technological advancements.
- **Blockchain Integration**: Explore the utilization of blockchain technology for secure and transparent data processing.
- **IoT Device Support**: Explore the integration of DocWire SDK with IoT devices, extending its capabilities to process data generated by the Internet of Things.

**Embark on an exciting journey with DocWire SDK, where the philosophies of C++ converge with the power of data processing innovation. Explore the limitless possibilities today and witness the substantial enhancements that set it apart from its predecessor, DocToText.**

<a name="features"></a>
## Features

- Able to extract/import and export text, images, formatting, and metadata along with annotations
- Data can be transformed between import and export (filtering, aggregating, etc)
- Equipped with multiple importers:
    - Microsoft Office new Office Open XML (OOXML): DOCX, XLSX, PPTX files
    - Microsoft Office old binary formats: DOC, XLS, XLSB, PPT files
    - OpenOffice/LibreOffice Open Document Format (ODF): ODT, ODS, ODP files
    - Portable Document Format: PDF files
    - Webpages: HTML, HTM, and CSS files
    - Rich Text Format: RTF files
    - Email formats with attachments: EML files, MS Outlook PST, OST files
    - Image formats: JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP with OCR capabilities
    - Apple iWork: PAGES, NUMBERS, KEYNOTE files
    - ODFXML (FODP, FODS, FODT)
    - Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)
    - Scripts and source codes: ASM, ASP, ASPX, BAS, BAT, C, CC, CMAKE, CS, CPP, CXX, D, F, FPP, FS, GO, H, HPP, HXX, JAVA, JS, JSP, LUA, PAS, PHP, PL, PERL, PY, R, SH, TCL, VB, VBS, WS files
    - XML format family: XML, XSD, XSL files
    - Comma-Separated Values: CSV files
    - Other structured text formats: JSON, YML, YAML, RSS, CONF files
    - Other unstructured text formats: MD, LOG files
    - DICOM (DCM) as an additional commercial plugin
- Equipped with multiple exporters:
    - Plain text: Easily extract and export text content.
    - HTML: Export content in HTML format for web use.
    - CSV: Export data to Comma-Separated Values format.
    - XLSX and more are coming: Additional export formats for diverse use cases.
- HTTP::Post transformer: Facilitate seamless communication with external HTTP APIs or services, enabling data exchange and integration with external systems.
- Integration with OpenAI API, including transformers like:
    - TranslateTo: Translate text to different languages.
    - Summarize: Generate summarized content from longer texts.
    - ExtractEntities: Extract entities and key information from text.
    - Classify: Perform text classification and categorization.
    - ExtractKeywords: Identify and extract keywords from text.
    - DetectSentiment: Analyze and detect sentiment in text.
    - AnalyzeData: Perform data analysis on text content.
    - Chat: Conduct chat-based interactions and conversations.
- Supports multiple LLM models: gpt-3.5-turbo, gpt-3.5-turbo-16k, gpt-3.5-turbo-1106, gpt-4, gpt-4-32k and gpt-4-1106-preview (world events up to April 2023, 128k context window that can fit more than 300 pages of text in a single prompt). More are coming.
- Equipped with a high-grade, scriptable, and trainable OCR that has LSTM neural networks-based character recognition
- Incremental parsing returning data as soon as they are available
- Cross-platform: Linux, Windows, MacOSX, and more to come
- Can be embedded in your application (SDK)
- Can be integrated with other data mining and data analytics applications
- Parsing process can be easily designed by connecting objects with the pipe `|` operator into a chain
- Parsing chain elements communicate based on Boost Signals
- Custom parsing chain elements can be added (importers, transformers, exporters)
- Small binaries, fast native C++ code

<a name="examples"></a>
## Examples

Parse file in any format (Office, PDF, mail, etc) having its path, export to plain text and print to standard output:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) | Importer() | PlainTextExporter() | Output(std::cout);
  }
  return 0;
}
```

Parse file in any format (Office, PDF, mail, etc) having stream, export to HTML and save to file stream:

```cpp
#include "docwire.h"
#include <fstream>

int main(int argc, char* argv[])
{
  using namespace docwire;

  Input(std::ifstream(argv[1], std::ios_base::binary)) | Importer() | HtmlExporter() | Output(std::ofstream("output.html"));

  return 0;
}
```

Parse all files in any format inside archives (ZIP, TAR, RAR, GZ, BZ2, XZ) recursively:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) | DecompressArchives() | Importer() | PlainTextExporter() | Output(std::cout);
  }
  return 0;
}
```

Classify file in any format (Office, PDF, mail, etc) to any categories:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) | Importer() | PlainTextExporter() | openai::Classify({ "agreement", "invoice", "report", "legal", "other"}, "api-key-1234") | Output(std::cout);
  }
  return 0;
}
```

Translate document in any format (Office, PDF, mail, etc) to other language:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) | Importer() | PlainTextExporter() | openai::TranslateTo("french", "api-key-1234") | Output(std::cout);
  }
  return 0;
}
```

Detect sentiment of document in any format (Office, PDF, mail, etc) using newest GPT-4 Turbo model with 128K context:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) | Importer() | PlainTextExporter() | openai::DetectSentiment("api-key-1234", openai::Model::gpt4_1106_preview) | Output(std::cout);
  }
  return 0;
}
```

Reusing single parsing chain to parse multiple input files:

```cpp
#include "docwire.h"
#include <fstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  auto chain = Importer() | PlainTextExporter() | Output(std::cout);  // create a chain of steps to parse a file
  for (int i = 1; i < argc; ++i)
    Input(std::ifstream(argv[i], std::ios_base::binary)) | chain; // set the input file as an input stream
  return 0;
}
```

Using transformer to filter out emails (eg. from Outlook PST mailbox) with subject containing "Hello":

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  Input(argv[1]) |
  Importer()
    | TransformerFunc([](Info &info) // Create an importer from file name and connect it to transformer
      {
        if (info.tag_name == StandardTag::TAG_MAIL) // if current node is mail
        {
          auto subject = info.getAttributeValue<std::string>("subject"); // get the subject attribute
          if (subject) // if subject attribute exists
          {
            if (subject->find("Hello") != std::string::npos) // if subject contains "Hello"
            {
              info.skip = true; // skip the current node
            }
          }
        }
      })
    | PlainTextExporter() // sets exporter to plain text
    | Output(std::cout);
  return 0;
}
```

![Example flow](doc/images/example_flow.png)

Joining transformers to filter out emails (eg. from Outlook PST mailbox) with subject "Hello" and limit the number of mails to 10:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  Input(argv[1]) |
  Importer() |
    TransformerFunc([](Info &info) // Create an input from file name, importer and connect them to transformer
    {
      if (info.tag_name == StandardTag::TAG_MAIL) // if current node is mail
      {
        auto subject = info.getAttributeValue<std::string>("subject"); // get the subject attribute
        if (subject) // if subject attribute exists
        {
          if (subject->find("Hello") != std::string::npos) // if subject contains "Hello"
          {
            info.skip = true; // skip the current node
          }
        }
      }
    }) |
    TransformerFunc([counter = 0, max_mails = 1](Info &info) mutable // Create a transformer and connect it to previous transformer
    {
      if (info.tag_name == StandardTag::TAG_MAIL) // if current node is mail
      {
        if (++counter > max_mails) // if counter is greater than max_mails
        {
          info.cancel = true; // cancel the parsing process
        }
      }
    }) |
    PlainTextExporter() | // sets exporter to plain text
    Output(std::cout);
 return 0;
}
```

<a name="awards"></a>
## Awards

- SourceForge Community Choice (2023) - project has qualified for this award out of over 500,000 open source projects on SourceForge
- Microsoft for Startups grant (2022) - project was selected by Microsoft to accelerate its grow by providing Expert Guidance, development tools, Azure and GitHub cloud infrastructure and OpenAI machine learning utilities

<a name="installation"></a>
## Installation

### Why Choose vcpkg for DocWire SDK Installation?
DocWire has embraced vcpkg as the preferred installation method for several compelling reasons:
- **Microsoft's Trusted Solution:** vcpkg is a package manager developed and backed by Microsoft, ensuring reliability, ongoing support, and compatibility.
- **Cross-Platform Simplicity:** With vcpkg, DocWire installation becomes a breeze on Windows, Linux, and macOS, providing a unified and hassle-free experience.
- **Effortless Dependency Management:** vcpkg takes care of resolving and installing dependencies automatically, reducing manual configuration efforts for developers.
- **Swift Binary Package Deployment:** The availability of pre-built binary packages in vcpkg accelerates installation, minimizing the need for time-consuming manual compilation.
- **Seamless CMake Integration:** vcpkg seamlessly integrates with the CMake build system, simplifying the incorporation of DocWire into CMake-based projects.
By selecting vcpkg, DocWire ensures that programmers benefit from a trusted, user-friendly, and well-supported solution that guarantees a smooth installation experience.

### Installation Steps
1. **Install vcpkg:**
If you haven't already installed vcpkg, you can do so by following the instructions in the [vcpkg documentation.](https://github.com/microsoft/vcpkg).
2. **Integrate vcpkg:**
Ensure that vcpkg is integrated with your development environment by running the following command:
```
vcpkg integrate install
```
3. **Clone the DocWire Repository:**
Clone the DocWire repository from GitHub if you haven't already:
```
git clone https://github.com/docwire/docwire.git
```
4. **Set Up Overlay:**
To configure vcpkg to recognize the DocWire overlay, use the following command:
```
vcpkg overlay add docwire/ports
```
This command ensures that vcpkg adds the DocWire overlay for subsequent installations.

5. **Install DocWire:**
Now that the overlay is set up, you can use vcpkg to install the DocWire library:
```
vcpkg install docwire
```
6. **Download Binary Archives (Alternative):**
As an alternative to building from source, users can also download pre-built binary archives that contain the results of the vcpkg export command for DocWire. These archives can be found in the "Releases" section of the DocWire GitHub repository.
7. **Link with Your Project:**
After installation, configure your project settings to link with the DocWire library and its dependencies. You can use one of the following example CMake commands to build your project:
- Using vcpkg toolchain file from downloaded prebuilt binaries:
```
cmake -DCMAKE_TOOLCHAIN_FILE=docwire/scripts/buildsystems/vcpkg.cmake ..
```
- Using vcpkg toolchain file from the vcpkg repository:
```
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

### Usage and Conclusion
You're all set! You've successfully installed the DocWire library using vcpkg. You can now use the DocWire library in your code to perform text extraction from documents. 

### Compatibility Note
Please note that DocWire is currently compatible with vcpkg tagged as version 2023.11.20. While this version is recommended for use, be aware that updates may become available in the near future. Supported triplets are: x64-linux-dynamic, x64-windows and x64-osx-dynamic.

<a name="api-documentation"></a>
## API Documentation
The API documentation for the DocWire SDK/library is readily available in various formats to assist you in seamlessly integrating it into your projects. Whether you prefer reading detailed doxygen-style documentation, accessing it through binary packages, or installing it via package managers like Vcpkg, we've got you covered.

### ReadTheDocs - Doxygen Format
Our API documentation is hosted on [ReadTheDocs](https://docwire.readthedocs.io/), presented in the widely recognized and developer-friendly Doxygen format. This comprehensive documentation provides insights into the functionality, usage, and features of the DocWire library. You can explore it at your own pace to better understand how to harness the power of DocWire within your applications.

### GitHub Releases

If you're looking for a more direct way to access the documentation, you can find it bundled with our binary packages in the [GitHub Releases](https://github.com/docwire/docwire/releases) section. Simply download the appropriate release for your platform, and you'll have the API documentation readily available alongside the library itself.

### Vcpkg Package Manager

For users who prefer to manage their dependencies using Vcpkg, we've made sure that our API documentation is included with the packages you install. This means you can access the same doxygen-style documentation seamlessly as you manage and integrate DocWire into your C++ projects.

### Consistency Across Platforms

It's worth noting that no matter where you choose to access our API documentation—whether through ReadTheDocs, GitHub Releases, or Vcpkg—you will find the same comprehensive doxygen-style documentation. This ensures a consistent and reliable resource for understanding and utilizing the DocWire library.

### Why ReadTheDocs?

You might wonder why we chose ReadTheDocs to host our documentation. While some might see this as a marketing signal, we believe it's a practical choice for several reasons:

1. **Accessibility**: ReadTheDocs provides an easy-to-navigate platform that ensures our documentation is readily accessible to all users.

2. **Versioning**: We can maintain multiple versions of our documentation, ensuring that you can always find the information relevant to your specific library version.

3. **Automation**: ReadTheDocs allows us to automate the documentation publishing process, ensuring that you have the latest documentation whenever you need it without delay.

We believe in making the integration of DocWire as smooth as possible, and providing our documentation through ReadTheDocs is just one way we're committed to simplifying your experience.

Explore the documentation, experiment with the library, and feel free to reach out if you have any questions or feedback. We're here to support you on your journey with DocWire.

<a name="license"></a>
## License

Product is double licensed:

- It can be used in open source projects under the terms of the GNU General Public License, version 2
- It can be used in commercial projects under the terms of commercial license agreement

Please contact SILVERCODERS (http://silvercoders.com) for a commercial license for DocWire SDK.

Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,
client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license
other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.
Simply stop using the product if you disagree with this viewpoint.

<a name="authors"></a>
## Authors

- [Silvercoders Ltd](https://silvercoders.com)
- [DocWire LLC](https://docwire.io)

<a name="external-links"></a>
## External links

- [Project homepage - DocWire](https://docwire.io)
- [Cloud API on RapidAPI](https://rapidapi.com/docwire-docwire-default/api/docwire-doctotext)
- [SourceForge project page](https://sourceforge.net/projects/doctotext)
- [LinkedIn page](https://www.linkedin.com/company/docwire-it-services-consulting)
- [Project homepage - Silvercoders](https://silvercoders.com/en/products/doctotext)
