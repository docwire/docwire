# DocWire DocToText SDK

DocWire DocToText is a C++ data processing library/SDK with SourceForge Community Choice award and Microsoft for Startups support. Parses DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT), PST, OST and more. Contains embedded OCR engine and CLI utiity.

## Features
- Able to extract/import and export text, images, formatting and metadata along with annotations
- Data can be transformed between import and export (filtering, aggregating etc)
- Equipped with multiple importers:
    - Microsoft Office new Office Open XML (OOXML): DOCX, XLSX, PPTX files
    - Microsoft Office old binary formats: DOC, XLS, XLSB, PPT files
    - OpenOffice/LibreOffice Open Document Format (ODF): ODT, ODS, ODP files
    - Portable Document Format: PDF files
    - HTML files
    - Rich Text Format: RTF files
    - Email formats with attachments: EML files, MS Outlook PST, OST files
    - Image formats: JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP with OCR capabilities
    - Apple iWork: PAGES, NUMBERS, KEYNOTE files
    - ODFXML (FODP, FODS, FODT)
    - DICOM (DCM) as an additional commercial plugin
- Equipped with multiple exporters:
    - Plain text
    - HTML
    - CSV and XLSX are coming
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

## License
Product is double licensed:
- Can be used in open source projects using the GNU General Public License version 2
- Can be used in commercial projects using commercial license

## Authors
- [Silvercoders Ltd](https://silvercoders.com)
- [DocWire LLC](https://docwire.io)
