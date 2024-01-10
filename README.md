# DocWire SDK

DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing. Supports nearly 100 data formats, including email boxes and OCR in more than 100 languages. Boost efficiency in text extraction, web data extraction, data mining, document analysis. Offline processing possible for security and confidentiality.

**Embark on an exciting journey with DocWire SDK, where the philosophies of C++ converge with the power of data processing innovation. Explore the limitless possibilities today and witness the substantial enhancements that set it apart from its predecessor, DocToText.**

Explore the latest updates, contribute to the community, and find the most up-to-date information on our [GitHub project page](https://github.com/docwire/docwire).

## Table Of Contents
- [The Goal](#the-goal)
- [Features](#features)
- [Revolutionary API Concept](#api-concept)
- [Examples](#examples)
- [Awards](#awards)
- [Installation](#installation)
- [Versioning](#versioning)
- [Logging](#logging)
- [API documentation](#api-documentation)
- [Console application (CLI)](#console-application)
- [License](#license)
- [Authors](#authors)
- [External links](#external-links)

<a name="the-goal"></a>
## The Goal

Introducing DocWire SDK: Revolutionizing Data Processing in C++20

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

If you've experienced the power of DocToText, brace yourself for a substantial upgrade. Staying ahead with C++20, we integrate advanced features such as ranges and concurrency. DocWire SDK evolves to handle a spectrum of data processing tasks, including robust HTTP capabilities, comprehensive network APIs, and efficient web data extraction.

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
    - Image formats: JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP with OCR capabilities in more than 100 languages and multiple languages in single picture.
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
    - TextToSpeech: Perform written text into spoken words (voice) conversion (TTS).
    - Transcribe: Convert spoken language (voice) into written text (transcription, Automatic Speech Recognition).
- Supports multiple LLM models: gpt-3.5-turbo, gpt-3.5-turbo-16k, gpt-3.5-turbo-1106, gpt-4, gpt-4-32k and gpt-4-1106-preview (world events up to April 2023, 128k context window that can fit more than 300 pages of text in a single prompt), whisper-1, tts-1. More are coming.
- Equipped with a high-grade, scriptable, and trainable OCR that has LSTM neural networks-based character recognition OCR capabilities in more than 100 languages and multiple languages in single picture.
- Incremental parsing returning data as soon as they are available
- Cross-platform: Linux, Windows, MacOSX, and more to come
- Can be embedded in your application (SDK)
- Can be integrated with other data mining and data analytics applications
- Parsing process can be easily designed by connecting objects with the pipe `|` operator into a chain
- Parsing chain elements communicate based on Boost Signals
- Custom parsing chain elements can be added (importers, transformers, exporters)
- Small binaries, fast native C++ code

<a name="api-concept"></a>
## Revolutionary API concept

### Seamless Integration of PipeChain and DataTree

Welcome to the DocWire SDK, where we redefine document processing through groundbreaking API concepts. In this chapter, let's delve into the two pillars shaping our approach: PipeChain and DataTree. Revolutionize your document processing with an SDK that seamlessly blends familiar development practices like C++ ranges, DOM tree and SAX parser-inspired processing model providing an unparalleled and dynamic coding experience. Explore the revolutionary synergy between PipeChain and DataTree.

### PipeChain: Streamlining Data Flow

**Expressive Code Flow**:
Unlock the power of PipeChain, a concept inspired by C++ ranges that brings a familiar and expressive coding experience.

**Versatile Chain Elements**:
Break free from rigidity. Our chain elements transcend traditional boundaries, accommodating everything from document parsers to custom exporters. Just as manipulating DOM elements, you'll find these versatile elements provide unparalleled flexibility.

**Extensible Functionality**:
Expand your capabilities effortlessly. Our chain elements support a variety of functionalities, allowing you to seamlessly integrate new components, akin to extending SAX parser or DOM tree iterator behavior.

**Examples of Information Flowing Through PipeChain**:
- **Emails**: Process and analyze email content seamlessly within the chain, extracting key information like subject, sender, and attachments.
- **Email Attachments**: Dive into attachments effortlessly, extracting details such as file types, sizes, and embedded content.
- **Folders**: Navigate through folders, organizing and processing documents or emails stored within them.
- **Archives**: Unpack archives with ease, whether they contain documents, images, or nested folders.
- **Mailboxes**: Manage entire mailboxes as a cohesive unit, transforming and exporting data efficiently.
- **Pages**: Navigate through pages of documents, extracting text, styles, and metadata.
- **Paragraphs**: Process paragraphs individually, extracting and transforming text, styles, and more.
- **Tables**: Efficiently handle tabular data, extracting, transforming, and exporting information seamlessly.
- **Links**: Navigate through hyperlinks, capturing URLs, anchor text, and associated metadata.
- **Images**: Process images effortlessly, extracting details like format, dimensions, and embedded text.

### DataTree: Navigating the Information Landscape

**Tree of Information Flow**:
Dive into the structured information universe with DataTree, reminiscent of navigating the DOM tree in HTML. Beyond the document tree, our API handles the intricate tree of documents within archives, attachments within emails, and more.

**Transformative Chain Elements**:
Explore the transformative power of DataTree with document parsers, custom transformers, and dynamic exporters. Each chain element acts as a SAX parser, providing a dynamic and efficient approach to processing. Like SAX parsers, our chain elements efficiently traverse and process data as it flows through the pipeline.

**Unified Transformation**:
Harmonize your processing across different levels of the information tree, similar to traversing the DOM tree. Whether it's a document page or an attachment within an email, our unified transformation approach ensures coherence throughout.

### Uniting PipeChain and DataTree: Elevate Your Processing

**Comprehensive Structure**:
Merge the structured flow of PipeChain with the depth of information in DataTree, creating a revolutionary approach that is as comfortable as working with the DOM tree in HTML. Embrace a comprehensive system that navigates, transforms, and exports data seamlessly.

**Effortless Adaptability**:
Experience the synergy of PipeChain and DataTree, effortlessly adapting to diverse document structures. Elevate your document processing game with a harmonious blend of expressive code, structured information, and an efficient SAX parser-inspired processing model.

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

Make a voice summary of document in any format (Office, PDF, mail, etc) in two steps: summarize using GPT model and convert the summary to speech using text to speech model. Result is saved to mp3 file:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) | Importer() | PlainTextExporter() | openai::Summarize("api-key-1234") | openai::TextToSpeech("api-key-1234") | Output(std::ofstream("summary.mp3"));
  }
  return 0;
}
```

Make a text summary of voice recording (e.g. mp3 file with meeting recording) in two steps: convert voice to text using Whisper-1 model and summarize text using GPT model:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) | openai::Transcribe("api-key-1234") | PlainTextExporter() | openai::Summarize("api-key-1234") | Output(std::cout);
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

<a name="versioning"></a>
## Versioning

DocWire SDK introduces a distinctive versioning methodology, deviating from conventional semantic versioning (SemVer) in favor of a dynamic, date-centric system. This chapter elucidates the versioning strategy of DocWire SDK, shedding light on its unique attributes and the benefits derived from this innovative approach.

### Versioning Scheme

#### Date-Based Versioning

DocWire SDK's versioning system integrates release dates as a pivotal element. Each release is identified by a specific date, providing users with a transparent timeline of updates and enhancements. This approach ensures clarity and enables users to comprehend the evolution of the SDK over time.

#### "Live at the Head" Concept

Adhering to the "live at the head" philosophy, DocWire SDK always grants immediate access to the latest version, representing the current state of the codebase. This model allows users to benefit from the most recent features, improvements, and bug fixes without waiting for formal releases, enhancing the overall user experience.

#### Planned Release Cycle

To maintain a balance between regular updates and stability, DocWire SDK plans one release or tagging per quarter. This structured release cycle incorporates new features and improvements. In the event of discovering serious bugs or multiple smaller issues, additional releases may occur to ensure the continual delivery of a high-quality SDK.

#### Inspired by VCPKG Project Versioning

DocWire SDK draws inspiration from the versioning approach of the VCPKG project. As VCPKG serves as the preferred method for installing the SDK, users familiar with VCPKG will find a similar versioning philosophy in DocWire SDK, reinforcing a cohesive and familiar experience.

### Advantages of Date-Based Versioning

#### Continuous Evolution

The date-based versioning strategy fosters a model of continuous evolution, allowing for swift development and deployment of new features. This ensures users have timely access to the latest advancements and can choose to stay current or opt for specific versions tailored to their requirements.

#### Improved Collaboration

Transparent and chronological versioning facilitates collaboration among developers, contributors, and users. This inclusive model enables everyone to track the SDK's progress, understand the sequence of changes, and contribute to discussions around specific releases, fostering a sense of community and shared ownership.

#### Rapid Issue Resolution

The "live at the head" concept accelerates issue resolution by providing prompt access to bug fixes and improvements. This agile approach ensures that users encounter fewer obstacles, leading to a more responsive and satisfying experience.

#### User-Centric Updates

DocWire SDK's versioning approach prioritizes user-centric updates, enabling developers to make informed decisions about when to integrate the latest changes into their projects. This flexibility empowers users to tailor their SDK experience based on specific features or fixes introduced in each release.

### Codebase Strategy

DocWire SDK maintains a single code branch, focusing on continuous improvement of the API and staying up-to-date with integrated external services. This strategy prioritizes innovation over stability, ensuring that resources are directed towards enhancing the SDK rather than maintaining outdated code branches. This approach aligns with industry best practices and optimizes the SDK for evolving technology landscapes.

### Long-Term Support (LTS) Agreements

Recognizing the diverse needs of our users, DocWire SDK offers Long-Term Support (LTS) agreements, providing a tailored solution for those seeking sustained stability and reliability. The LTS agreement is designed to address specific concerns related to feature stability, API consistency, and platform support, offering peace of mind as projects progress and evolve.

#### Customized Stability

With an LTS agreement, customers can select specific features or aspects of the API that are crucial to their projects. This ensures that the chosen features remain stable and will not be subject to unexpected changes or removals as the SDK continues to evolve. Customized stability empowers users to build and maintain applications with confidence, knowing that the core functionalities they rely on will remain consistent over time.

#### API Consistency

For projects that demand a consistent API, the LTS agreement provides assurance that the API's core elements will remain unchanged throughout the agreed-upon support period. This commitment to API consistency enables developers to build and maintain applications with minimal disruptions, fostering a stable and reliable development environment.

#### Platform Support Assurance

The LTS agreement extends to platform support, offering a guarantee that the SDK will continue to support selected platforms. This is particularly beneficial for projects with specific platform dependencies, ensuring that compatibility is maintained even as new SDK updates are introduced. Customers can negotiate the details of platform support to align with their project requirements.

#### Tailored Support Duration

DocWire SDK understands that the definition of "long-term" can vary based on individual project timelines and needs. Therefore, LTS agreements come with the flexibility to negotiate the support duration, allowing customers to align the agreement with their project's lifecycle and development roadmap.

#### Ongoing Collaboration

An LTS agreement signifies an ongoing collaboration between DocWire SDK and the customer. It establishes a dedicated channel for communication, ensuring that any concerns or specific requirements related to stability, API, or platform support are addressed promptly. This collaborative approach reflects our commitment to supporting our users throughout their development journey.

#### How to Enquire about LTS

To explore the possibilities of an LTS agreement or to discuss specific requirements, please reach out to our dedicated support team. We are committed to working closely with our users to create customized LTS agreements that cater to the unique demands of their projects, providing a solid foundation for long-term success.

<a name="logging"></a>
## Logging

DocWire SDK generate extensive logs that provide insights into the current processing status, warnings, and errors. In the latest version of the SDK, the logging mechanism has been enhanced to output logs in JSON format, offering several advantages.

The enhanced logging mechanism in the DocWire SDK provides developers with powerful tools for monitoring and debugging data processing. Whether redirecting logs to a custom stream or leveraging the flexibility of JSON formatting, the logging system is designed to meet the diverse needs of users.

### JSON Format for Logging

The logs are now formatted in JSON, providing a structured and machine-readable representation of the information. This format is advantageous for various reasons:

- **Structured Data**: JSON allows for a clear and organized representation of log data, making it easy to extract specific information.

- **Compatibility**: JSON is widely supported by various tools and platforms, ensuring compatibility and ease of integration into existing workflows.

- **Readability**: The human-readable nature of JSON logs facilitates manual inspection and troubleshooting when needed.

- **Flexibility**: JSON's key-value pair structure accommodates a wide range of log information, enhancing the flexibility of the logging system.

### Configuring Logging

To configure the logging parameters, the SDK provides a set of functions. Users can set the log verbosity level, customize the log stream, and create log record streams with specific severity levels and source locations.

### Log Macros

The SDK includes convenient macros for logging, such as:

- docwire_log(severity): Conditionally logs based on the specified severity level.

- docwire_log_vars(...): Logs variables with associated values.

- docwire_log_func_with_args(...): Logs function entry with associated arguments.

### Additional Logging Features

The SDK introduces new features like logging source locations, custom streamable types, and handling of iterable and dereferenceable objects.

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

<a name="console-application"></a>
## Console Application (CLI)

Welcome to the DocWire Console Application (DocWire CLI). This versatile command-line tool empowers users to extract content from documents, including text, document structure, and metadata. Whether you're processing documents for analysis, summarization, sentiment detection, or translation, DocWire CLI has you covered.

### Usage

To run the program and process a document, use the following command:

```bash
docwire [options] file_name
```

### Basic options
- **&ndash;&ndash;help**: Display the help message.
- **&ndash;&ndash;version**: Display the DocWire version.
- **&ndash;&ndash;verbose**: Enable verbose logging.
- **&ndash;&ndash;input-file <file_path>**: Specify the path to the file to process (or you can provide filename without --input-file).
- **&ndash;&ndash;output_type <type>** (default: plain_text): Set the output type. Available types include plain_text, html (preserving document structure), csv (structured data), and metadata (document information).

### OpenAI Integration (New Features!)

Unlock the power of OpenAI with the following options:

- **&ndash;&ndash;openai-chat <prompt>**: Initiate a chat prompt for processing exported data via OpenAI.
- **&ndash;&ndash;openai-extract-entities**: Extract entities from exported data via OpenAI.
- **&ndash;&ndash;openai-extract-keywords <N>**: Extract N keywords/key phrases from exported data via OpenAI.
- **&ndash;&ndash;openai-summarize**: Summarize exported data via OpenAI.
- **&ndash;&ndash;openai-detect-sentiment**: Detect sentiment of exported data via OpenAI.
- **&ndash;&ndash;openai-analyze-data**: Analyze exported data for important insights and generate conclusions via OpenAI.
- **&ndash;&ndash;openai-classify <category>**: Classify exported data via OpenAI to one of the specified categories.
- **&ndash;&ndash;openai-translate-to <language>**: Language to translate exported data to via OpenAI.
- **&ndash;&ndash;openai-text-to-speech**: Convert text to speech via OpenAI
- **&ndash;&ndash;openai-transcribe**: Convert speech to text (transcribe) via OpenAI
- **&ndash;&ndash;openai-key <key>**: OpenAI API key.
- **&ndash;&ndash;openai-model <model>** (default: gpt35_turbo): Choose the OpenAI model. Available models are: gpt35_turbo, gpt35_turbo_16k, gpt35_turbo_1106, gpt4, gpt4_32k and gpt4_1106_preview.
- **&ndash;&ndash;openai-temperature <temp>**: Force specified temperature for OpenAI prompts.
- **&ndash;&ndash;openai-tts-model <model>** (default: tts1): Choose the TTS model. Available models are: tts1, tts1_hd.
- **&ndash;&ndash;openai-voice <voice>** (default: alloy): Choose voice for text to speech conversion. Available voices are: alloy, echo, fable, onyx, nova, shimmer.

### Additional Options

- **&ndash;&ndash;language <lang> (default: eng)**: Set the document language(s) for OCR as ISO 639-3 identifiers like: spa, fra, deu, rus, chi_sim, chi_tra etc. More than 100 languages are supported. Multiple languages can be enabled.
- **&ndash;&ndash;use-stream <yes|no> (default: 0)**: Pass the file stream to the SDK instead of the filename.
- **&ndash;&ndash;min_creation_time <timestamp>**: Filter emails by minimum creation time (currently applicable only to emails in PST/OST files).
- **&ndash;&ndash;max_creation_time <timestamp>**: Filter emails by maximum creation time (currently applicable only to emails in PST/OST files).
- **&ndash;&ndash;max_nodes_number <number>**: Filter by the maximum number of nodes.
- **&ndash;&ndash;folder_name <name>**: Filter emails by folder name.
- **&ndash;&ndash;attachment_extension <type>**: Filter by attachment type.
- **&ndash;&ndash;table-style <style> (default: table_look, deprecated)**: Set the table style. Available styles include table_look, one_row, and one_col.
- **&ndash;&ndash;url-style <style> (default: extended, deprecated)**: Set the URL style. Available styles include text_only, extended, and underscored.
- **&ndash;&ndash;list-style-prefix <prefix> (default: " * ", deprecated)**: Set the output list prefix.
- **&ndash;&ndash;log_file <file_path>**: Set the path to the log file.
- **&ndash;&ndash;plugins_path <path>**: Set a non-standard path to DocWire plugins.

Note: The "min_creation_time" and "max_creation_time" options currently work only for emails within PST/OST files.

### Example Usage

#### Extracting Structured Content in HTML Format

To extract structured content in HTML format, use the following command:

```bash
docwire --output_type html document.docx
```

#### Leveraging OpenAI for Intelligent Document Analysis

Harness the power of OpenAI to analyze and extract valuable insights from your document. For example, initiate a chat prompt to interact with the document's content:

```bash
docwire --openai-chat "What are the key points in the document provided?" document.docx
```

#### Sentiment Analysis for Document Understanding

Gain a deeper understanding of the document's sentiment using OpenAI. Detect the sentiment of the exported data with the following command:

```bash
docwire --openai-detect-sentiment document.docx
```

#### Dynamic Language Translation for Multilingual Documents

Translate your document into another language using OpenAI. Specify the target language with the following command:

```bash
docwire --openai-translate-to spanish document.docx
```

#### Summarization and text to speech conversion

Summarize your document using GPT model, convert summary to audio using TTS model and read it loud:

```bash
docwire document.doc --openai-summarize --openai-text-to-speech | ffplay -nodisp -autoexit -
```

Happy Document Processing with DocWire CLI!

<a name="license"></a>
## License

Product is double licensed:

- It can be used in open source projects under the terms of the GNU General Public License, version 2
- It can be used in commercial projects under the terms of commercial license agreement

Please contact SILVERCODERS (http://silvercoders.com) for a commercial license for DocWire SDK.

In addition DocWire SDK incorporates code that falls under licenses separate from the GNU General Public License or the DocWire Commercial License;
instead, it operates under specific opensource licenses granted by the original authors.
Details are described in "3rdparty components used" documentation chapter.

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
- [SourceForge project page](https://sourceforge.net/projects/doctotext)
- [LinkedIn page](https://www.linkedin.com/company/docwire-it-services-consulting)
- [Project homepage - Silvercoders](https://silvercoders.com/en/products/doctotext)
