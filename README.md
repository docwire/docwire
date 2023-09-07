# DocWire DocToText SDK

DocWire DocToText is a C++ data processing library/SDK with SourceForge Community Choice award and Microsoft for Startups support. Parses DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT), PST, OST and more. Contains embedded OCR engine and CLI utiity.

## Table Of Contents
- [The Goal](#the-goal)
- [Features](#features)
- [Awards](#awards)
- [Installation](#installation)
- [License](#license)
- [Authors](#authors)
- [External links](#external-links)

## The Goal
The goal of this project is to create a data processing SDK/API/engine to drastically speed up data processing development.
This programming library/framework offers pre-made configurable components that can be joined together to create a processing
pipechain that extracts data from one place, transforms it according to configurable rules and loads it somewhere else.
It can be used by programmers in any popular language like C++, C or Python or in scripts as command line interface (CLI).
Pipechains can be also designed as text file templates (low-code/no-code solution).
All of this is delivered as an open-source solution that programmers and IT companies feel safe and secure about.

Designing data processing in applications is hard. But if it includes processing documents in multiple formats (like PDF, DOC, XLS etc),
extracting information from email boxes, databases or websites, it becomes a nightmare. In that nightmare the project specializes the most ;-)
Pre-made components give the ability to significantly reduce development time, costs and risk for companies that specialize in cybersecurity,
finances, visual aid, e-commerce and many more

Entering the artificial intelligence/neural networks area is planned to offer its advantages as part of the engine.

## Features
- Able to extract/import and export text, images, formatting and metadata along with annotations
- Data can be transformed between import and export (filtering, aggregating etc)
- Equipped with multiple importers:
    - Microsoft Office new Office Open XML (OOXML): DOCX, XLSX, PPTX files
    - Microsoft Office old binary formats: DOC, XLS, XLSB, PPT files
    - OpenOffice/LibreOffice Open Document Format (ODF): ODT, ODS, ODP files
    - Portable Document Format: PDF files
    - Webpages: HTML, HTM and CSS files
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
    - Plain text
    - HTML
    - CSV
    - XLSX and more are coming
- Equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition
- Incremental parsing returning data as soon as they are available
- Cross platform: Linux, Windows, MacOSX (and more is coming)
- Can be embeded in your application (SDK)
- Can be integrated with other data mining and data analytics applications
- Parsing process can be easily designed by connecting objects with pipe | operator into a chain
- Parsing chain elements communication based on Boost Signals
- Custom parsing chain elements can be added (importers, transformers, exporters)
- Small binaries, fast native C++ code

## Awards
- SourceForge Community Choice (2023) - project has qualified for this award out of over 500,000 open source projects on SourceForge
- Microsoft for Startups grant (2022) - project was selected by Microsoft to accelerate its grow by providing Expert Guidance, development tools, Azure and GitHub cloud infrastructure and OpenAI machine learning utilities

## Installation

### Why Choose vcpkg for DocToText Installation?
DocToText has embraced vcpkg as the preferred installation method for several compelling reasons:
- **Microsoft's Trusted Solution:** vcpkg is a package manager developed and backed by Microsoft, ensuring reliability, ongoing support, and compatibility.
- **Cross-Platform Simplicity:** With vcpkg, DocToText installation becomes a breeze on Windows, Linux, and macOS, providing a unified and hassle-free experience.
- **Effortless Dependency Management:** vcpkg takes care of resolving and installing dependencies automatically, reducing manual configuration efforts for developers.
- **Swift Binary Package Deployment:** The availability of pre-built binary packages in vcpkg accelerates installation, minimizing the need for time-consuming manual compilation.
- **Seamless CMake Integration:** vcpkg seamlessly integrates with the CMake build system, simplifying the incorporation of DocToText into CMake-based projects.
By selecting vcpkg, DocToText ensures that programmers benefit from a trusted, user-friendly, and well-supported solution that guarantees a smooth installation experience.

### Installation Steps
1. **Install vcpkg:**
If you haven't already installed vcpkg, you can do so by following the instructions in the [vcpkg documentation.](https://github.com/microsoft/vcpkg).
2. **Integrate vcpkg:**
Ensure that vcpkg is integrated with your development environment by running the following command:
```
vcpkg integrate install
```
3. **Clone the DocToText Repository:**
Clone the DocToText repository from GitHub if you haven't already:
```
git clone https://github.com/docwire/doctotext.git
```
4. **Set Up Overlay:**
To configure vcpkg to recognize the DocToText overlay, use the following command:
```
vcpkg overlay add doctotext/ports
```
This command ensures that vcpkg adds the DocToText overlay for subsequent installations.

5. **Install DocToText:**
Now that the overlay is set up, you can use vcpkg to install the DocToText library:
```
vcpkg install doctotext
```
6. **Download Binary Archives (Alternative):**
As an alternative to building from source, users can also download pre-built binary archives that contain the results of the vcpkg export command for DocToText. These archives can be found in the "Releases" section of the DocToText GitHub repository.
7. **Link with Your Project:**
After installation, configure your project settings to link with the DocToText library and its dependencies. You can use one of the following example CMake commands to build your project:
- Using vcpkg toolchain file from downloaded prebuilt binaries:
```
cmake -DCMAKE_TOOLCHAIN_FILE=doctotext/scripts/buildsystems/vcpkg.cmake ..
```
- Using vcpkg toolchain file from the vcpkg repository:
```
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

### Usage and Conclusion
You're all set! You've successfully installed the DocToText library using vcpkg. You can now use the DocToText library in your code to perform text extraction from documents. 

### Compatibility Note
Please note that DocToText is currently compatible with vcpkg tagged as version 2023.01.09. While this version is recommended for use, be aware that updates may become available in the near future. Supported triplets are: x64-linux-dynamic, x64-windows and x64-osx-dynamic.

## License
Product is double licensed:
- Can be used in open source projects using the GNU General Public License version 2
- Can be used in commercial projects using commercial license

## Authors
- [Silvercoders Ltd](https://silvercoders.com)
- [DocWire LLC](https://docwire.io)

### External links
- [Project homepage - DocWire](https://docwire.io)
- [Cloud API on RapidAPI](https://rapidapi.com/docwire-docwire-default/api/docwire-doctotext)
- [SourceForge project page](https://sourceforge.net/projects/doctotext)
- [LinkedIn page](https://www.linkedin.com/company/docwire-it-services-consulting)
- [Project homepage - Silvercoders](https://silvercoders.com/en/products/doctotext)
