## Version 2026.01.20

This release introduces a major overhaul of the SDK's core, focusing on modern C++20 features for XML parsing, robust type-safe data conversions, and configurable safety policies. Significant architectural changes have been made to enhance resilience against partial failures in parsers, improve memory management, and standardize date/time handling using `std::chrono`. The update also includes new utilities for named parameters and non-null or in-range assertions, alongside numerous fixes and documentation improvements.

> The XML stream, a new path it takes,  
> With C++20, a modern API awakes.  
> Safety policies, strict or relaxed,  
> Data conversions, precisely taxed.  
> Partial failures, now gracefully met,  
> `std::chrono` dates, no more regret.  
> A robust foundation, strong and so keen,  
> DocWire's future, brightly foreseen.  
> ✨🚀🛡️

- **Features**
  - **Modern C++20 XML Parsing API**: Introduced a new, expressive, and safe API for parsing XML documents, featuring a single-pass, forward-only reader and utilizing C++20 ranges and views for efficient node filtering and manipulation. This replaces the legacy `XmlStream` with `xml::reader`, `xml::node_ref`, `xml::children`, and `xml::attributes`.
  - **Configurable Safety Policies**: Implemented a general concept of safety policies (`strict` vs. `relaxed`) allowing developers to choose between robust error-checking (throwing exceptions on violations) and maximum performance (omitting checks). This is applied through `checked` and `not_null` wrappers, and the `enforce` utility.
  - **Type-Safe Data Conversion Framework**: Introduced a new `convert` namespace with `try_to` and `to` functions for robust, type-safe data conversions, replacing manual string-to-numeric/date conversions. This framework supports custom format tags like `with::date_format` and `with::partial_match`.
  - **C++20 `std::chrono` for Date/Time Handling**: Replaced all `struct tm` usage with `std::chrono::sys_seconds` for date and time attributes and conversions, enhancing type safety and robustness across the SDK.
  - **Partial Failure Resilience in Parsers**: Implemented `message_counters` and `make_counted_message_callbacks` in `archives_parser`, `EMLParser`, `HTMLParser`, `PDFParser`, and `PSTParser` to allow processing to continue even if some sub-items fail, and to detect total failures.
  - **Base64 Decoding**: Added a `base64::decode` function and support for parsing `data:` URLs in HTML image sources.
  - **Named Parameters and Structured Bindings**: Introduced `named::value` and `operator""_v` literal for creating named parameters, with full support for C++ structured bindings.
  - **Non-Null Assertion Utility**: Added `not_null` wrapper for pointer-like types, enforcing non-null invariants based on the configured `safety_policy`. Includes `assume_not_null` for unchecked construction when non-null is guaranteed.
  - **Ranged Numeric Types**: Introduced `ranged` template for numeric types with compile-time or runtime-checked bounds, along with fluent aliases like `at_least`, `at_most`, `exactly`, and `non_negative`.
  - **Debug-Only Assertions**: Added `debug_assert` for assertions that are only active in debug builds, providing a mechanism for internal contract checking without runtime overhead in release builds.

- **Improvements**
  - **HTML Parser Memory Management**: Improved HTML parser by using `std::unique_ptr` for `lxb_html_document_t` and validating head text before CSS parsing to prevent potential crashes.
  - **PST Parser Robustness**: Enhanced PST parser with `unique_handle` RAII wrappers for `libpff` and `libbfio` resources, ensuring exception-safe cleanup. Fixed destruction order of handles and used `LIBBFIO_OPEN_READ` to prevent accidental file truncation.
  - **Logging Enhancements**: Refactored `log_scope` macro and `scope` class for better control and zero-cost in release builds. Added `to_log_value` for `docwire::named::value` to improve structured logging.
  - **Error Reporting**: `make_error_from_tuple` and `make_error_ptr_from_tuple` functions were added for more flexible error creation.
  - **Efficient String View Access**: Added a `string_view()` method to `data_source` for efficient, allocation-free access to data as `std::string_view` where possible.

- **Refactor**
  - **XML Parsing Integration**: All XML-based parsers (`CommonXMLDocumentParser`, `ODFOOXMLParser`, `ODFXMLParser`, `XMLParser`) have been refactored to utilize the new C++20 XML parsing API.
  - **Date/Time Serialization**: Removed `serialization_time.h` as `std::chrono` types are now directly convertible to/from strings via the new `convert` framework.
  - **`rapidfuzz` Dependency**: Updated the `rapidfuzz` dependency to `rapidfuzz-cpp` to align with the upstream vcpkg port rename.
  - **`XmlStream` Removal**: The legacy `XmlStream` class and its associated files (`xml_stream.h`, `xml_stream.cpp`) have been removed, replaced by the new `xml::reader` API.

- **Fixes**
  - **`CommonXMLDocumentParser::onODFOOXMLText`**: Removed a redundant check for the "#text" node name.
  - **`ODFOOXMLParser::onOOXMLFldData`**: Added a handler to skip `fldData` content in OOXML.

- **Documentation**
  - **Updated `README.md`**: The `README.md` file has been updated to highlight the new "Modern C++20 XML Parsing" and "Configurable Safety Policies" features, including a new example demonstrating the XML parsing API.
  - **Doxygen Documentation**: Comprehensive Doxygen documentation has been added for the new XML parsing utilities and helper classes.

- **Tests**
  - **New XML API Tests**: Added `xml_tests.cpp` and `xml_parsing_example.cpp` to thoroughly test the new C++20 XML parsing API.
  - **`api_tests.cpp` Enhancements**: Updated `api_tests.cpp` with new tests for `base64::decode`, `convert` from and to `std::chrono`, `named` structured bindings, `not_null` assertions, and `PartialAndTotalFailures` scenarios for `ZIP`, `EML`, `PST`, `HTML`, and `PDF` parsers.
  - **New Test Files**: Added `embedded_images.html` and `embedded_images.html.out` for testing HTML image processing.

## Version 2025.10.31

This release introduces a comprehensive overhaul of the SDK's diagnostic and testing infrastructure, centered around a new zero-cost logging framework, a powerful serialization system, and a fluent assertion utility. The error handling mechanism has been significantly enhanced to provide richer, more structured diagnostic messages. This release also modernizes all `README.md` examples, refactors core components for improved performance and thread-safety, and enhances the CI/CD pipeline.

> The old logs fade, a silent, costly past,  
> New macros rise, expressive and fast.  
> With sinks and filters, a structured, clear design,  
> And fluent assertions, that truly align.  
> A core reforged, robust and so keen,  
> The finest framework, the best we have seen.  
> ✨🚀🔧

- **Features**
  - **Modern Logging Framework:** Replaced the legacy logger with a new zero-cost, structured logging system. It features a sink/filter architecture, JSON output, and expressive macros (`log_entry`, `log_scope`, `log_forward`) that are compiled out in release builds for zero performance overhead.
  - **Serialization Framework:** Introduced a powerful, concept-based serialization framework to convert C++ types into a structured representation, used by the new logging and error systems.
  - **Fluent Assertion Utility:** Added `docwire::ensure`, a zero-cost, exception-throwing assertion utility for writing expressive and robust release-build validations (e.g., `ensure(value) > 5`).
  - **Safe Environment Variable Access:** Introduced `docwire::environment::get`, a safe and convenient wrapper for accessing environment variables.
  - **Endian-Aware Binary Reader:** Added `binary::reader`, a new utility for safely parsing little-endian binary data from any stream-like source.

- **Improvements**
  - **Refactored `charset_converter`:** Overhauled the `charset_converter` for improved performance, correctness, and thread-safety by serializing `iconv_open` calls with a global mutex to resolve a data race in glibc.
  - **Refactored `XLSBParser`:** The `XLSBParser` now uses the new `binary::reader`, resulting in cleaner, more robust, and endian-aware parsing logic.
  - **Optimized Test Suite:** All API tests now run in a single process, significantly improving test suite performance, especially under sanitizers.
  - **CI Enhancements:**
    - The CI build matrix has been simplified and made more robust.
    - Conditional execution for OpenAI tests is now supported via the `run-tests:openai` pull request label, manual dispatch options, and repository-specific schedule triggers.

- **Refactor**
  - **Enhanced Error Handling:** The error handling framework has been refactored to use the new serialization system and support multiple context items, providing more detailed and structured diagnostic messages.
  - **Type Name Normalization:** The `type_name` utility has been refined to more robustly handle C++ standard library implementation artifacts, improving cross-platform consistency.

- **Fixes**
  - Fixed a bug where `unique_identifier` counters could be reset by moving the static member definition to a `.cpp` file, ensuring a single instance across shared libraries.
  - Corrected a lambda capture issue in `pix_cache` to capture `data_source` by reference, improving correctness and performance.
  - Prevented Leptonica error message accumulation by clearing the `stderr` buffer on each use.

- **Documentation**
  - **Modernized README:** Replaced all full code examples in `README.md` with concise snippets that use the new `ensure` API for assertions. Each snippet now links to its corresponding full, verifiable source file in the `tests/` directory.
  - **Updated Logging Section:** The logging documentation has been completely rewritten to reflect the new framework's features and usage.

- **Tests**
  - **Log Sanitization:** Automated tests now sanitize expected log output to robustly handle differences in function name formatting across compilers and platforms.
  - **README Snippet Verification:** Added a new automated test to ensure all code snippets in `README.md` are consistent with their full source examples.

## Version 2025.09.25

This release introduces a major architectural overhaul, replacing the `std::variant`-based `Tag` system with a type-erased, polymorphic `message_ptr`. This fundamental change refactors the entire data pipeline—including parsers, exporters, filters, and core components—to emit rich, structured document events, simplifying the data flow and making it easier to add new data types. Alongside this, a new high-performance `http::server` is introduced, allowing developers to expose any processing pipeline as a standalone HTTP/HTTPS microservice with TLS support.

> The pipeline's heart, a message new and bright,  
> Where docs and mail now dance in polymorphic light.  
> A server hums, where pipelines run as one,  
> This grand refactor shines, a new day has begun.  
> 🚀🔧🏗️

- **Features**
  - **HTTP Server:** Added a high-performance `http::server` class with HTTPS/TLS support. This allows any processing pipeline to be easily exposed as a standalone microservice via a path-to-pipeline mapping, enabling powerful and scalable data processing endpoints.

- **Improvements**
  - **HTTP Post Content-Type:** The `http::Post` component now automatically sets the `Content-Type` header for multipart form data based on the MIME type of the input `data_source`, simplifying API integrations.
  - **Enhanced vcpkg Feature Parsing:** The build system's vcpkg feature parsing has been improved to handle more complex, object-based, platform-specific feature definitions.

- **Refactor**
  - **Polymorphic Message System:** Replaced the core `std::variant`-based `Tag` system with a type-erased, polymorphic `message_ptr` across the entire data pipeline. This major architectural change affects all parsers, exporters, filters, and core components, enabling them to emit rich, structured document events and simplifying the addition of new data types.
  - **Reorganized Element Types:** Decoupled and reorganized all document, mail, and AI element types into separate, more logical header files (`document_elements.h`, `mail_elements.h`, `ai_elements.h`, `attributes.h`) for better code organization.
  - **Abstracted XML Parsing Options:** Refactored the XML parser to use a dedicated `XmlStream::no_blanks` struct for configuration, improving clarity over raw integer flags.
  - **Unified Transformer API:** Updated `TransformerFunc` and `StandardFilter` to use the new `message_transform_func`, aligning custom pipeline elements with the new message-based API.

- **Fixes**
  - **MSVC Compilation:** Resolved a compilation failure on MSVC by parenthesizing `std::min` to avoid conflicts with the Windows `min` macro and added the `/bigobj` compiler option to handle heavily templated libraries.
  - **RTTI Issues on macOS:** Added visibility attributes to core document, mail, and AI elements to fix RTTI-related issues on macOS.

- **Build**
  - **Dependency Change:** Replaced `curlpp` with `cpp-httplib` and `OpenSSL` to support the `http::Post` and `http::server` components.
  - **CI Update:** Removed the deprecated `macos-13` runner from the GitHub Actions workflow.

- **Documentation**
  - Updated `README.md` and all examples to use the new `message_ptr` API.

- **Tests**
  - **End-to-end HTTP Server Tests:** Added new integration tests to validate client-server pipeline processing for both HTTP and HTTPS using the new `http::server`.
  - **API Migration:** Migrated the entire test suite to use the new `message_ptr` API, including the addition of custom GTest matchers for easier and more readable assertions.

## Version 2025.08.13

This release marks a major milestone by integrating full support for OpenAI's newly released `gpt-5` family of models. To reflect this significant advancement in AI, `gpt_5` is now the default model for all OpenAI operations, ensuring users have immediate access to state-of-the-art capabilities. The release also includes minor code quality improvements.

> The future's here, no longer a dream,  
> GPT-5 joins the DocWire stream.  
> A new default, intelligent and vast,  
> On foundations of quality, built to last.  
> ✨🤖🚀

- **Features**
  - **New OpenAI Model Support:** Added support for the next-generation `gpt-5` family of models (`gpt-5`, `gpt_5_mini`, `gpt_5_nano`, `gpt_5_chat_latest`) and new research-focused models (`o3_deep_research`, `o4_mini_deep_research`).

- **Improvements**
  - **Updated Default AI Model:** The default model for all OpenAI operations has been updated to `gpt_5`, leveraging the new state-of-the-art capabilities.
  - **Updated XML parser initialization:** Remove call to deprecated xmlGetGlobalState() for modern libxml2 compatibility.
  - **Robust Model Handling:** The OpenAI model selection logic now throws an exception for unknown or unsupported models, improving error reporting.
  - **Code Quality:** Several single-argument constructors in AI-related chain elements (`AnalyzeData`, `ExtractEntities`, `Summarize`) have been marked `explicit` to prevent unintended implicit conversions.

- **Documentation**
  - **Updated README:** The `README.md` file has been updated to reflect the new default model and the expanded list of available OpenAI models for both chat and CLI usage. Examples were updated to use non-default models to showcase flexibility.

## Version 2025.08.05

This release introduces a major new feature: local, offline AI-powered text embeddings. By integrating the `multilingual-e5-small` model, DocWire can now generate high-quality vector representations for text in over 100 languages, enabling advanced NLP tasks like semantic search and RAG. This update also includes a significant dependency modernization, replacing `OpenNMT-Tokenizer` with Google's `SentencePiece`, and numerous build and stability fixes, particularly for MSVC and Valgrind.

> A new dimension, in vectors bright,  
> Where words find place, and meanings take flight.  
> With models sharp and logic so keen,  
> A deeper understanding, clearly seen.  
> From text to numbers, a seamless art,  
> DocWire now reads the document's heart.  
> ✨🧠🔢

- **Features**
  - **Local AI Embeddings:** Introduced a powerful new `local_ai::embed` chain element to generate high-quality, multilingual text embeddings using the `multilingual-e5-small` model. This enables advanced NLP tasks like semantic search, RAG, and text clustering to be performed entirely offline.
  - **Cosine Similarity Function:** Added a `cosine_similarity` utility function to calculate the similarity between two embedding vectors, making it easy to compare documents and queries.
  - **Public Tokenizer API:** The `local_ai::tokenizer` class, now powered by Google's SentencePiece, is exposed as a public API. It includes an `encode()` method to convert text into token IDs, supporting different tokenizer models like `T5Tokenizer` and `XLMRobertaTokenizer`.

- **Improvements**
  - **Unified Model Runner:** The `local_ai::model_runner` has been enhanced to dynamically load and manage both sequence-to-sequence (`Translator`) and encoder-only (`Encoder`) models, enabling it to handle both text generation and embedding tasks within a single class.
  - **Advanced Pooling and Normalization:** Implemented mean pooling over token outputs and L2 normalization for the embedding model to ensure high-quality, standardized vectors as required by models like E5.
  - **Simplified AI Chain Element:** Added a new convenience constructor to `local_ai::model_chain_element` that uses a default model, simplifying its usage in common scenarios.
  - **CLI Enhancements:** The command-line interface now supports embedding generation via the `--local-ai-embed` option.

- **Refactor**
  - **Dependency Modernization:** Replaced the `OpenNMT-Tokenizer` dependency with a direct integration of Google's `SentencePiece` library, reducing complexity and aligning with modern NLP tooling.

- **Fixes**
  - **Build (MSVC):** Resolved Address Sanitizer (ASan) linking errors on MSVC by adding `_DISABLE_STRING_ANNOTATION` and `_DISABLE_VECTOR_ANNOTATION` definitions.
  - **Build (CI):** Increased CI timeouts for Valgrind-based sanitizers (`memcheck`, `helgrind`, `callgrind`) to prevent premature job termination.
  - **Build (CI):** Disabled resource-intensive local AI tests when running under Callgrind to ensure CI stability.
  - **Build (Valgrind):** Added suppressions for known memory leaks in the Abseil library to clean up Valgrind reports.

- **Documentation**
  - **New Embedding Example:** Added a comprehensive example to `README.md` demonstrating how to generate embeddings for a document and multiple queries, and then calculate their cosine similarity.

- **Tests**
  - Added unit tests for the new `local_ai::tokenizer` to validate its behavior with both `flan-t5` and `multilingual-e5` models.
  - The new local AI embedding example from `README.md` is now compiled and executed as part of the automated test suite.

## Version 2025.07.14

This release introduces a significant new capability by adding support for OpenAI text embeddings. It also features a major refactoring that modularizes the HTTP client into its own library, and enhances the robustness of AI components by switching from file extension checks to MIME type-based content identification.

> A whisper of meaning, in numbers cast,  
> New dimensions open, built to last.  
> For search and reason, a powerful key,  
> Unlocking knowledge for all to see.  
> ✨🔢🧠🔍

- **Features**
  - **OpenAI Embeddings Support:** Added a new `openai::embed` chain element to generate text embeddings using OpenAI's models (`text-embedding-3-small`, `text-embedding-3-large`, `text-embedding-ada-002`). This includes corresponding CLI options (`--openai-embed`, `--openai-embed-model`) and extensive documentation with examples.
  - **MIME Type to File Extension Conversion:** Introduced a new utility function, `content_type::by_file_extension::to_extension`, to convert MIME types back to their common file extensions, improving interoperability.

- **Improvements**
  - **Robust Content Handling in AI Components:** AI components like `openai::Chat` and `local_ai::model_chain_element` now rely on MIME types (`text/plain`, `image/*`) instead of file extensions for more accurate content identification.
  - **Enhanced `http::Post` Robustness:** The `http::Post` component can now automatically determine the file extension from the MIME type if it's not explicitly provided in the `data_source`.

- **Refactor**
  - **Modular HTTP Client:** The `http::Post` class has been moved from `docwire_core` to a new, dedicated `docwire_http` library to improve modularity.

- **Fixes**
  - **Build (Windows):** Fixed a build failure on Windows by explicitly adding `libxml2` as a direct dependency in the vcpkg port.
  - **Documentation:** Corrected a minor paragraph formatting issue in `README.md`.

## Version 2025.06.29

This release introduces a major update to the integrated OpenAI functionalities, adding support for the latest models like GPT-4o and retiring older ones. It also enhances dependency management by replacing a custom library port with a standard one, and improves the intelligence of several AI-powered features through better prompting.

> The old models fade, a starlit past,  
> New constellations, brighter and vast.  
> With prompts refined and logic so keen,  
> A cleaner build, a powerful scene.  
> ✨🤖🔧

- **Features**
  - **Expanded OpenAI Model Support:** Integrated support for the latest generation of OpenAI models, including `gpt-4o`, `gpt-4o-mini`, `gpt-4.1`, and the `o3` family. This provides access to state-of-the-art AI capabilities for all relevant features.
  - **Granular Model Selection for Transcription and TTS:**
    - Users can now select specific models for audio transcription, with new options like `gpt-4o-transcribe` and `gpt-4o-mini-transcribe`, in addition to `whisper-1`.
    - Added new Text-to-Speech models, including `gpt-4o-mini-tts`, and set it as the new default for higher quality voice generation.

- **Improvements**
  - **Modernized Dependency Management:** Replaced the custom `unzip` vcpkg port with the standard `minizip` port, simplifying the build process and improving maintainability.
  - **Enhanced AI Prompting:** Significantly improved the system prompts for the `Classify` and `Find` features to yield more precise and consistently formatted results from the language models.
  - **Updated Default Model:** The default model for most OpenAI operations has been updated to `gpt-4o` to leverage its improved intelligence, speed, and cost-effectiveness.
  - **Robust Example Tests:** Updated examples in the documentation to use fuzzy string matching for assertions, making tests more resilient to minor variations in AI-generated text.

- **Refactor**
  - **API Modernization:** Removed support for deprecated and legacy OpenAI models (e.g., `gpt-3.5-turbo`, `gpt-4-turbo-preview`) to streamline the API and focus on current-generation models.
  - **Transcription Component:** Refactored the `openai::Transcribe` component to allow model selection, making it more flexible and future-proof.

## Version 2025.06.19

Version 2025.06.19 introduces notable advancements to DocWire SDK, featuring significant upgrades to OCR capabilities, greatly improved precision in PDF document handling including richer positional metadata, and a modernized approach to archive processing. This release also includes expanded content type support, refined text output, and strengthened build and testing infrastructure, providing developers with enhanced data extraction tools, more reliable document analysis, and an improved development workflow. **Please note: Support for the deprecated windows-2019 GitHub runners has been removed from our CI/CD pipeline. If this change impacts your workflow, please contact us for assistance.**


> OCR's vision, sharp and newly bright,  
> PDF layouts, now a clearer sight.  
> Archives rebuilt, with structure firm and new,  
> DocWire advances, steady, strong, and true!  
> ✨📄🔬🏗️

- **Features**
  - **Significant OCR Enhancements:**
	- **Structured OCR Output with Positional Data:** The `OCRParser` now provides a more detailed, structured output. It recognizes and emits not only text with its positional attributes (x, y, width, height) but also identifies document elements like paragraphs, sections, and lines. This significantly enhances data extraction capabilities, allowing for better preservation of the original document layout and enabling more sophisticated content analysis.
	- **Configurable OCR Confidence:** Users can now set a custom confidence threshold (0-100) for OCR results, allowing for a better balance between accuracy and the volume of extracted text by filtering out low-confidence words.
  - **Enhanced PDF Parsing & Positional Metadata:**
    - The `PDFParser` has been refactored for **position-based element sorting**. This significantly improves the accuracy of text flow and element placement, providing more precise positional metadata (x, y, width, height) for extracted text and images within PDF documents. This leads to a more faithful representation of the original layout.
  - **Expanded Format Support:**
	- **ASP & ASP.NET Content Type Detection:** DocWire now includes specialized detection for Active Server Pages (ASP) and ASP.NET content, increasing its versatility with web-based file formats.
  - **Modernized Archive Handling:**
	- **New `docwire_archives` Library:** Archive processing has been comprehensively refactored and moved into a new, dedicated `docwire_archives` library. This architectural improvement enhances modularity, maintainability, and performance for archive-related operations.

- **Improvements**
  - **Improved Archive Detection:**
	- Archive format identification is now more robust and standardized by leveraging **MIME types for detection**.
  - **Refined Plain Text Output:** Improved handling of page breaks in the plain text exporter for clearer document separation and better readability.
  - **Updated CI & Testing Environment:**
	- The continuous integration pipeline now utilizes **windows-2025 GitHub runners** (replacing the deprecated `windows-2019` runner) and has **restored ASAN sanitizer tests on Windows**, contributing to code reliability.
    - Automated tests (including `http::Post`, document parsing, and CLI OCR tests) have been updated to further enhance test coverage and consistency.
  - **Documentation Enhancements:** Updated project documentation to reflect new features, API changes, and supported platforms, alongside specific corrections to module dependencies in 3rdparty components.

- **Fixes**
  - **Build (Windows):** Added the `NOMINMAX` preprocessor definition for Windows builds. This resolves macro conflicts between standard Windows headers (e.g., `windows.h`) and the PDFium library, preventing compilation issues.
  - **Output Formatting:** Corrected spacing and line break logic in OCR and PDF outputs for improved readability and layout fidelity.

## Version 2025.05.22

This release introduces significant enhancements to PDF processing, including image extraction and OCR integration, alongside major internal refactorings that modernize the core data flow and parser architecture. It also includes several fixes related to thread safety, library linking, and testing infrastructure, particularly improving test discovery and execution on Windows.

> From PDFs, images now take flight,  
> Through refined chains, data flows bright.  
> With steadier tests and safer threads,  
> Docwire advances, new paths it treads.  
> 🖼️🔗⚙️

- **Features**
  - **PDF Processing:** Added extraction of images from PDF files. Extracted images can now be processed by subsequent chain elements, including content type detection and OCR.
  - **Writers:** Updated HTML and plain text writers to support image tags, including data URLs for embedded images and text derived from OCR.
  - **Testing:** Implemented automatic tests for the new PDF image extraction and OCRing capabilities.

- **Improvements**
  - **Core Architecture:** Significantly refactored the data processing mechanism within chain elements. This modernizes the core data flow, enhances clarity on processing progression (continue, skip, stop), and allows more flexible tag emission, including parsers sending data back for reprocessing.
  - **Parser Architecture:** Refactored parsers to directly implement `ChainElement` and utilize enhanced `data_source` checks, eliminating the `Parser` base class.
  - **Testing & CI:** Enhanced CI by adding explicit runs of automatic test discovery to catch issues that `ctest` might silently ignore.
  - **Testing & CI:** Improved error reporting in CI by separating the execution of API automatic tests from example runs.
  - **Code Organization:** Moved `HTMLWriter` and `HTMLExporter` classes from `docwire_core` to the `docwire_html` library.

- **Fixes**
  - **Core:** Ensured thread-safe initialization of parser MIME type vectors, including a specific fix for `PSTParser`.
  - **Testing:** Resolved test discovery issues on Windows by implementing a custom `main()` function for automatic tests instead of linking `gtest_main`.
  - **Build:** Addressed linking issues with the `docwire_html` library.
  - **Build:** Fixed `mailio` library linking to ensure compatibility with version 0.25.1 following a vcpkg upgrade.

## Version 2025.04.29

This release marks a significant upgrade to PDF handling by replacing the PoDoFo library with Google's PDFium engine. This change aims to improve compatibility and resolve parsing issues encountered with various PDF documents. A substantial part of this release involved extensive stabilization work to ensure PDFium builds and integrates correctly across Windows (MSVC), MacOS, ARM64, and with Clang compilers. Subsequent fixes address runtime issues discovered in the PDF parser after the integration.

> The old PDF engine, a source of some strife,  
> Replaced by PDFium, bringing new life.  
> Its powerful core resolves the plight,  
> Parsing PDFs now shines so bright.  
> 📄✨🔧

- **Features**
  - Replaced the PoDoFo library with PDFium in PDF parser to improve PDF parsing reliability and compatibility with problematic files.

- **Fixes**
  - **PDFium Integration & Build Stability:** Resolved numerous build, linking, and integration issues with the new PDFium backend across multiple platforms and toolchains:
    - Addressed compilation errors, warnings, and configuration specifics for MSVC on Windows.
    - Corrected linking problems and build issues on MacOS (including versions 13/14).
    - Ensured compatibility with the ARM64 architecture.
    - Fixed build issues encountered when using the Clang compiler.
    - Corrected installation paths for PDFium library files on Windows.
  - **PDFParser Runtime:**
    - Fixed threading issues within the PDF parser.
    - Addressed memory leaks identified in the PDF parser.
  - **PDFium Headers:** Fixed incorrect include paths in PDFium public header files.

## Version 2025.04.13

This release significantly overhauls the classic Word Document (.doc) parsing by integrating, updating, and stabilizing the underlying parsing engine. Numerous fixes, including several backported from related projects, address stability and memory safety issues.

> The classic `.doc`, a format old,  
> Its secrets now DocWire unfolds.  
> With steadier hand and sharper sight,  
> It reads the past, and makes it right.  
> 📜🔧✨

- **Improvements**
  - **DOC Parser:** Incorporated the `wv2` library directly as a subproject and rebased it to the latest available version (SVN r45, 2009-12-17) for significantly improved classic `.doc` file handling.
  - **DOC Parser Stability:** Improved list (`ilfo`) handling and validation

- **Bug Fixes**
  - **DOC Parser Stability:** Prevented potential endless loops during style unwrapping in the `wv2` library.
  - **DOC Parser Memory Safety:** Fixed invalid memory reads/writes in the `wv2` library reported by Valgrind by applying a fix adapted from the Calligra project
  - **DOC Parser Memory Safety:** Fixed potential use of invalid data pointers within the `wv2` library by applying a fix from the Calligra project.
  - **DOC Parser Security:** Prevented potential buffer overflows when handling specific structures within the `wv2` library by applying a fix from the Calligra project.
  - **Dependencies:** Updated the vcpkg patch for `libmagic` to resolve a regression affecting zip file detection introduced in libmagic 5.46.

- **Refactor**
  - **Code Compatibility:** Adjusted internal class names (`OLEStorage`, `OLEStreamReader`) and constants (`SEEK_*`) to align better with the original `wv2` codebase, simplifying the integration and future updates.

## Version 2025.03.24

This release introduces a major overhaul of the HTML parsing component within the DocWire SDK, significantly enhancing the accuracy, robustness, and efficiency of HTML processing.  Alongside this core update, we've modernized the build system and improved overall project structure. **Please note that we have removed support for Ubuntu 20.04 in our CI/CD pipeline. If this change impacts your workflow, please contact us for assistance.**

> I'm a coding bunny on a merry sprint,  
> Hopping through workflows without a hint,  
> Matrix trimmed and targets set to shine,  
> Parsing HTML with a twist so fine.  
> With tests and docs in cheerful array,  
> I nibble bugs and brighten the day!  
> 🐇💻✨

- **New Features**
  - **Enhanced HTML Parsing Capabilities:** The HTML parsing engine has been significantly upgraded, featuring:
    - **Improved Encoding Detection:**  More accurate and reliable detection of various character encodings, ensuring correct handling of diverse web content.
    - **DOM Correction:** Enhanced ability to automatically correct common HTML structure errors, leading to more robust and reliable parsing results.

- **Improvements**
  - **Modernized HTML Parser:** The HTML parser has been replaced with a modern, high-performance alternative, providing significant improvements in parsing speed, accuracy, and stability.
  - **Modernized Build Workflow:** Streamlined the CI/CD workflow with a more modern environment and adjustments to build and test steps.
  - **Expanded Logging:** Increased log uploads to provide more detailed insights into the build and test processes.
  - **CMake Integration:** Upgraded package configuration and dependency management for smoother integration with CMake-based projects.
  - **Target Installation:** Numerous CMake files now install targets with an export specification, improving the overall build process.

- **Documentation**
  - **Practical Integration Guidance:** Enriched documentation with practical integration examples, highlighting the new HTML parsing capabilities, to help developers quickly get started.

- **Refactor**
    - **HTML Parser Replacement:** Replaced the previous HTML parser and removed all related configuration files.
    - Numerous CMake files now install targets with an export specification, and a new package configuration template is added.

## Version 2025.03.02

This release brings a multitude of improvements, focusing on library build enhancements, error handling, charset conversion, and HTML parsing.

> As functions refine and errors cease,  
> Charset conversions bring their peace.  
> HTML parsing, a seamless release,  
> In the realm of code, our skills increase.  
> Progress marches on, in harmony's lease.  

### Improvements
- Extracted all OpenAI-related features to `docwire_openai` library.
- Added table caption support to `HTMLParser`, `HTMLExporter`, and `PlainTextExporter`.
- Introduced `charset_converter` class and used it instead of `htmlcxx::CharsetConverter` to prevent crashes on errors and avoid linking `htmlcxx` library.
- Added support for `std::exception_ptr` in `errors::make_nested` and used `std::current_exception()` to avoid exception object slicing.
- Made `errors::base` struct and all error tag structs non-inline to avoid ODR violations.

### Refactoring
- Replaced `defines.h` with standard CMake-generated `*_export.h` files.

### Bug Fixes
- Added patch to vcpkg libmagic port to fix 5.46 zip regression introduced by microsoft/vcpkg#43565.
- Fixed automatic test that was disabled by mistake.
- Fixed incorrect empty `Text` tags emitting in `HtmlParser`.

### Documentation
- Added file type determination example to the documentation.

## Version 2025.01.22

This release brings enhancements and fixes, focusing on content type detection and code refactoring for improved
performance, stability and maintainability.

> A new dawn breaks, a new release unfolds,  
> With features that shine, and performance that's told.  
> The code is clean, the heart is light,  
> In this version, all is made right.  
> The file signatures, a secret they hold,  
> The content type, no longer to be cold.  
> The parsing chain, a mighty stream,  
> With operators that make the heart beam.  
> The code is new, the code is grand,  
> In this release, a new world to expand.  
> So here's to the future, and all that's to come,  
> With this release, a new journey has just begun.  

- **New features**
	- Added support for content type detection based on file signatures.
	- Improved file format detection performance and robustness.
	- Added operator|= for easy parsing chain extension.

- **Refactor**
	- New API for file format detection and content type detection
	- All file format detection features moved to separate namespace and library
	- Highly refactored base parsing chain classes and operators
	- Introduced a general-purpose pimpl mechanism and made all parsing chain elements movable
	- A lot of other refactoring and code cleanup

## Version 2024.12.04

This release introduces significant improvements to error handling and code organization, enhancing the overall stability and usability of our software.

> Errors gracefully caught and tamed,  
> XmlStream and OCRParser, their handling acclaimed.  
> Stringification features, now standing alone,  
> Non-fatal parsing errors, their presence known.  
> Documentation clearer than before,  
> Our error handling reaching new shore.  

- **Improvements**
	- Improved error handling in the XmlStream and OCRParser classes, ensuring more robust and reliable parsing.
	- Extracted stringification features from error.h into a separate file, stringification.h, and improved them for better code organization and reusability.
	- Added emitting error from the first round of parsing as non-fatal if second round of parsing starts, allowing for more flexible error handling.

- **Bug Fixes**
	- Disabled printing of libxml2 errors and warnings to stderr, preventing unnecessary output and improving user experience.
	- Fixed a small issue with the main page of the API documentation, ensuring accurate and up-to-date information.

- **Documentation**
	- Extracted example of handling errors and warnings into a separate source file and added a Doxygen documentation page with that example, providing clear guidance for developers.
	- Linked the example to the docwire::errors namespace, errors::base class, and errors::impl template descriptions, enhancing documentation completeness and usability.

## Version 2024.11.23

This release introduces improvements to the DocWire SDK's codebase, focusing on enhancing code organization, modernizing features, and optimizing compilation times.

> Functions moved, headers light,  
> Logs decoupled, code shines bright.  
> 'Override' guides, exports renew,  
> Refactored code, we start anew.  

- **Refactor**
	- Extracted implementations of data_source methods from data_source.h into data_source.cpp and model_chain_element methods from model_chain_element.h into model_chain_element.cpp, to improve compilation times and code organization.
	- Separated logging functionalities from various classes and moved them into dedicated files (log_data_source.h, log_file_extension.h, log_tags.h), enhancing modularity and maintainability.
	- Removed unnecessary include directives from multiple files to reduce dependencies and improve build performance.
	- Ensured consistent use of modern C++ features like override for better code practices.

## Version 2024.11.19

This release introduces updates to the build workflow configuration, adding new macOS 15 runner and removing deprecated macOS 12 runner. VCPKG port for the docwire package was modified to fix automatic testing when multiple cmake instances are available. Various source files have been updated to adapt code to the new version of magic_enum library.

> 🐇 In the land of code where the rabbits play,  
> New paths are forged, come what may.  
> With macOS 15, our builds take flight,  
> Testing and logging, all feel just right.  
> So hop along, let’s celebrate,  
> With every change, we elevate! 🐇✨

- **New Features**
	- Added new macOS 15 runner

- **Bug Fixes**
	- VCPKG port for the docwire package was modified to fix automatic testing when multiple cmake instances are available.

- **Chores**
	- Updated source files to adapt code for the new version of `magic_enum` library.
	- Removed deprecated macOS 12 runner.

## Version 2024.11.10

This release introduces significant enhancements to error handling across various components of the DocWire SDK. A new section in the documentation outlines the comprehensive error handling framework, detailing features such as chained exceptions, type-safe context values, and secure error messages. Additionally, multiple source files have been updated to implement more structured error reporting, including the introduction of specific error types and improved context for exceptions. The changes aim to provide clearer diagnostics and facilitate better debugging practices throughout the SDK.

> 🐇 In the land of code, where errors do roam,  
> A new framework shines, guiding us home.  
> With tags and types, our troubles untwine,  
> Robust handling now, in each line we define.  
> So hop with joy, let the debugging commence,  
> For clarity reigns, and we leap with confidence! 🐇

- **New Features**
	- Introduced new error tag types such as `uninterpretable_data` and `program_corrupted` to enhance error categorization.
	- Added hashing functionalities of error types

- **Refactor**
	- Enhanced error handling mechanisms throughout the codebase, including updates to existing error types and the introduction of new ones.
	- Improved error messages for various parsing methods across multiple file types, enhancing clarity for debugging.
	
- **Documentation**
	- Updated `README.md` to include detailed descriptions of the error handling framework.

## Version 2024.10.15

The release introduces several significant changes across various files, enhancing configuration, documentation, and functionality. Key updates include improved error handling in various components, providing clearer context for exceptions, and the introduction of new functionality for chaining function calls in C++20, enabling more flexible and composable operations. Other notable changes include modifications to the .gitignore to ignore the .cache directory and updates to the CMake configuration to enhance build process integration with tools like VSCode.

> 🐇 "In the code where changes abound,  
> A rabbit hops with joy profound.  
> New features bloom, errors refined,  
> Documentation clear, knowledge aligned.  
> With CMake's might and README's grace,  
> Our project thrives, a joyful space!" 🐇

- **New Features**
	- Enhanced error handling in various components, improving security, clarity and context for exceptions.
	- Introduced new functionality for chaining function calls in C++20, allowing for more flexible and composable operations.
	- Improved handling of w:binData tag in XML parser

- **Documentation**
	- Updated README.md to clarify error handling, enhancing user-friendliness.

- **Chores**
	- Updated `.gitignore` to include `.cache` directory.
	- Modified CMake configuration to enhance build process integration with tools like VSCode.

## Version 2024.08.30

The changes involve enhancements to the XML parsing and reading logic. The parseXmlData function in xml_parser.cpp now includes improved handling for specific XML tags, while xml_stream.cpp has been refactored to encapsulate XML reading logic within a new method. Additionally, a new processing instruction has been added to the test XML document, enriching its structure.

> In the meadow, I hop with glee,  
> New tags and methods, oh what a spree!  
> Parsing XML, so swift and bright,  
> With each little change, the code feels just right.  
> A dance of data, a joyful tune,  
> Hooray for the updates, we’ll celebrate soon! 🐇✨

- **New Features**
  - Enhanced XML parsing capabilities with specific handling for `w:p` and `w:tab` tags.

- **Refactor**
  - Streamlined XML reading logic by encapsulating it in a new method, improving code readability and maintainability.

- **Bug Fixes**
  - Adjusted logic for handling XML node reading to ensure accurate processing of "processing instructions" elements.

## Version 2024.08.25

The changes encompass updates across various components in the project, including parsing improvements, modifications to build configurations, and adjustments to testing mechanisms. Key revisions include  the transition of sanitizer options, simplifications in build scripts, and improvements in resource handling.

> 🐰 In fields of code, a rabbit hops,  
> With changes fresh, it never stops.  
> From sanitizers bright to scripts that gleam,  
> Oh joyful leaps, let coding dream!  
> With hyperlinks in documents it now plays,  
> And encodes binary data in clever ways.  
> Improved tool management, with latest versions in tow,  
> It's a codebase that's efficient, and in the flow!  
> Each build a dance, each test a cheer,  
> In tech we trust, with love sincere! 🌼

- **New Features**
	- Enhanced installation instructions for the DocWire SDK, emphasizing vcpkg integration.
	- Added base64 encoding capabilities to public API to allow for encoding of binary data.
	- Introduced support for additional document formats (specialized XML parser) and parsing strategies.
	- Implemented a mechanism for handling hyperlinks in RTF and OOXML documents.

- **Documentation**
	- Updated README with clearer integration and setup instructions, including platform support.
	- Refactored comments and code snippets to reflect recent changes in the API.

- **Refactor**
	- Streamlined multiple build scripts and configurations for improved maintainability.
	- Improved tool management and resource handling.
	- Replaced several class definitions with type aliases to enhance modularity.
	- Upgraded 3rdparty components to the latest available versions
	- Removed vcpkg pinning to single version, now using the "latest" version on every build.
	- Splitted SDK into more libraries, improving modularity.

- **Chores**
	- Cleaned up unnecessary conditional logic in build scripts, simplifying the setup process.
	- Removed obsolete function declarations to enhance code clarity and organization.

- **Style**
	- Refined workflow configurations to improve clarity and organization of environment settings.

## Version 2024.07.31

Recent enhancements to the DocWire SDK significantly boost its natural language processing capabilities by integrating local AI models. Developers can now perform tasks like text classification and sentiment analysis directly within C++ applications, improving performance and safeguarding data privacy. Additionally, refined dependency management and build configurations provide a comprehensive toolkit for developing AI-driven applications. Overall, the updates significantly enhance the SDK's utility for developers working on NLP and AI projects, providing them with powerful tools to process and analyze data effectively while maintaining control over their data privacy.

> 🐰 In the burrow where ideas sprout,  
> Local models bring cheer, there's no doubt!  
> Processing swift, with privacy tight,  
> DocWire now shines, a true delight!  
> With text and models, we hop and play,  
> Celebrating changes—hip, hip, hooray! 🌟

- **New Features**
  - Local AI models execution for text classification, summarization, translation, sentiment analysis, named entity recognition and more directly within applications.
  - Added command-line options for local AI model processing, enhancing user interaction.
  - Introduced a build-in powerful flan-t5-large model for various NLP tasks
  - Added support for fuzzy matching capabilities with a new function for string similarity assessments.
  - Streamlined integration of third-party libraries, enhancing dependency management.

- **Documentation**
  - Updated documentation to reflect new local AI capabilities, including detailed usage instructions.

## Version 2024.06.24

Recent updates enhance cross-platform compatibility, introduce new input data handling approaches, and refine testing. OS versions added to the CI configuration include macOS 13, macOS 14, and Ubuntu 24.04. The data_source class now better supports various data types (std::vector, std::span, std::string_view), expanding its flexibility. New tests validate data source processing, while refinements prevent table errors.

> In realms of bytes and streams so vast,  
> Data flows from first to last.  
> CI matrix now more grand with Mac,  
> Ubuntu stands, there's no lack.  
> Tests abound, they check with care,  
> Ensuring code beyond compare.  
> A rabbit's cheer for changes new,  
> With bytes and tests, our world renew!  

- **New Features**
  - Added support for new OS versions `macos-13` and `macos-14` in workflows.
  - Enhanced `data_source` class to handle new data types like `std::vector<std::byte>`, `std::span<const std::byte>`, and `std::string_view`.

- **Bug Fixes**
  - Improved error checks in table structures to prevent invalid nesting in `plain_text_writer`.

- **Tests**
  - Added comprehensive test cases for various input data sources in `api_tests.cpp`.

- **Chores**
  - Updated OS configurations in `.github/workflows/build.yml` to include new OS versions and remove outdated ones.

## Version 2024.06.19

This release introduces significant modernizations and optimizations across the codebase. Key changes include the adoption of modern C++ practices such as move semantics and smart pointers, simplification of the code by removing outdated patterns, and structural enhancements that decentralize parsing responsibilities. Performance is notably optimized through new caching mechanisms and memory management strategies. Additionally, several bug fixes and new features enhance the functionality and reliability of data parsing and exporting components.

> In the realm of code, where logic reigns,  
> A modern touch, the old constrains.  
> Smart pointers weave, with move semantics,  
> A tapestry free from old panics.  
>
> Locks and guards, a vigilant dance,  
> Safeguarding flow, they advance.  
> Gone are wrappers, templates flee,  
> Simplicity wins, the code is free.  
>
> From parsers to import, a journey bold,  
> A narrative of efficiency told.  
> Obsolete no more, the styles old cast,  
> Lambdas in light, shadows they outlast.  
>
> Bugs in chains, limits set free,  
> PST's breath deeper, as it should be.  
> Metadata whispers in HTML's ear,  
> EML listens close, clarity near.  
>
> A cache of memories, seldom forget,  
> Performance tunes, on better we bet.  
> Through tests and logs, comparisons drawn,  
> A script to tell, which faster, which gone.  

- **Modernization of Parsing Chain Elements**: Initially, the usage of `clone()` methods was replaced with modern C++ design strategies like move semantics and smart pointers. This enhances both the safety and performance of the code by avoiding unnecessary copying and providing better resource management.

- **Enhancements in PDFParser**: The `std::mutex lock()` and `unlock()` methods were replaced with `std::lock_guard`. This change simplifies the code and ensures exception safety by automatically managing the lock's lifecycle.

- **Code Simplification**: The removal of `ParserWrapper`, `wrapper_parser_creator`, and `parser_creator` class templates simplifies the codebase, making it easier to maintain and understand.

- **Structural Changes in Parsing Logic**: Recursive parsing responsibilities were moved from individual parsers to the Importer class. This makes the parsers independent of the Importer class and simplifies the overall parsing architecture.

- **Removal of Obsolete Code**: The obsolete `FormattingStyle` class was removed. Additionally, `std::bind` was replaced with modern C++ lambda expressions, which are more straightforward and performant.

- **Bug Fix in PST Parser**: An incorrectly hardcoded limit on the number of mails processed was removed, potentially preventing data loss and improving the parser's reliability.

- **Enhancements in Exporters**: Support for document metadata was added in the HTML exporter, along with a workaround for incorrect date values in DOC format. Metadata support in the EML parser was also fixed, improving the accuracy and usefulness of the exported data.

- **Performance Optimization and Refactoring**: Introduction of several new classes such as `data_source`, `lru_memory_cache`, `memory_buffer`, `imemorystream`, `unique_identifier`, and `file_extension`. These changes aim to optimize performance, reduce reliance on temporary files, and improve support for nested documents in exporters.

- **Performance Comparison Script**: A new script was added to compare the performance between two versions of the SDK using callgrind logs from automatic tests, aiding in performance evaluation and regression testing.

## Version 2024.06.17

The recent changes include updates to the libbfio library, adjustments in test files, and fixes related to address sanitizer CI tests. Overall, these modifications improve compatibility, security, and build robustness.

> In code's quiet night, the rabbit hops,  
> URLs shift, new versions pop,  
> Headers dance in JSON's cheer,  
> And builds grow strong, without a fear.  
> With sanitizer's careful might,  
> We perfect our software’s flight. 🌙  

- **Updates**
	- Updated libbfio library to the latest release.
- **Bug Fixes**
	- Fixed headers in HTTP POST request tests.
	- Fixed address sanitizer tests.

## Version 2024.04.04

The DocWire SDK is embracing a dynamic development approach with its new "Release Early, Release Often" strategy and date-based versioning. This shift aims to enhance user experience by ensuring rapid releases, allowing users to benefit from the latest advancements with greater transparency and efficiency. While this release focuses on adopting this new versioning strategy and updating documentation, it's important to note that the significant improvements in memory management, thread safety, document processing, and build configurations were introduced in earlier versions. These enhancements are documented in the ChangeLog, highlighting the SDK's ongoing evolution.

> 🐰✨  
> In the world of code, where changes abound,  
> A rabbit hopped in, documentation found.  
> With every leap, a clearer path in sight,  
> Guiding through versions, making the future bright.  
> "Hop along," it cheered, with a joyful sound,  
> For in the realm of DocWire, clarity is crowned.  
> 🌟📚  

- **Documentation**
	- Introduced a "Release Early, Release Often" strategy with date-based versioning for the DocWire SDK, enhancing transparency and ease of tracking updates.
	- Updated the `ChangeLog` with details on significant enhancements including memory management, thread safety, document processing capabilities, and build configurations across various versions.
- **Refactor**
	- Modified the project's versioning approach in the `vcpkg.json` file to use "latest" as the version string, simplifying version management.
	- Adjusted the `portfile.cmake` to reference the `master` branch directly, removing specific commit references for easier updates.

## Version 2024.04.01

This update enhances support for Valgrind tools (memcheck, helgrind, callgrind) across various components, improving memory checking and thread safety. It includes changes to build configurations, scripts, and source code to address memory leaks, data races, and simplify XML parsing management. Testing procedures have been updated to incorporate Valgrind tools, and patches have been applied for better compatibility and performance.

> "In the realm of code, where the bytes do hop,  
> A rabbit worked hard, in the garden non-stop.  
> 🌱 With Valgrind in hand, and bugs in its sight,  
> It leaped through the lines, from morning to night.  
> 🐾 No leak too small, no race too fast,  
> Ensuring the garden's safety, vast.  
> 🥕 A cheer for the changes, so wisely sown,  
> For a healthier codebase, robustly grown."

- **New Features**
	- Added support for additional sanitizers (`memcheck`, `helgrind`, `callgrind`) including Valgrind tools in build configurations and automatic tests.
	- Introduced mutexes in various parsers for thread safety.
	- Enhanced XML handling to prevent memory leaks.
- **Bug Fixes**
	- Fixed threading data-race issues in parsers and logging.
	- Addressed memory leaks in XML processing.
- **Refactor**
	- Simplified error logging and XML parsing logic.
	- Updated memory management for XML streams to use `std::unique_ptr`.
- **Chores**
	- Updated build scripts and configurations for improved testing and compatibility.
	- Applied patches to address external library issues and Python incompatibilities in the build tool.

## Version 2024.03.26

This comprehensive update streamlines the codebase by enhancing file path handling, operation chaining, and streamlining output processes. It refactors document processing classes, introduces more structured and typed document element handling, and improves stream management. The changes aim to boost code readability, maintainability, and efficiency by leveraging modern C++ features and design patterns, significantly refining the development experience and output quality of the software.

> In the realm of code where rabbits dare to hop,  
> Changes vast and wide, improvements non-stop.  
> With structured tags and streams so clear,  
> The code now runs, without a fear.  
> 🐰💻✨  
> Through paths and chains, it finds its way,  
> A brighter, sleeker, brand new day.

- **New Features**
    - Introduced structures for document elements to facilitate structured document creation and manipulation.
    - Added common logging operators for enhanced debugging and error handling.
    - Streamlined stream management in chat functionalities.
    - Enhanced document processing with updated class refactoring and tagging system.
- **Refactor**
    - Simplified operation chaining and output handling in core functionalities.
    - Updated document processing classes for better maintainability and readability.
    - Refactored and renamed Input and Output classes for clearer code semantics.
    - Enhanced parsing and output processes with modern C++ features.
	- Updated CSS styling support in HTML documents for improved presentation.
- **Bug Fixes**
    - Fixed exception handling during command line argument parsing.
- **Tests**
    - Updated API tests to reflect new parsing and output stream handling methods.
    - Modified HTML output tests to test new CSS styling changes.
- **Chores**
    - Removed deprecated code and files, improving codebase cleanliness.
    - Added new utility function in build script for robust external command execution.

## Version 2024.03.16

The overarching theme of these changes involves enhancing thread safety and modernizing the codebase by transitioning from `pthread` to `std::mutex` for mutex management across various components. This includes adjustments in library linking, replacing `pthread_mutex_t` with `std::mutex`, and updating locking mechanisms to use `std::lock_guard`. These updates aim for better integration, dynamic configuration, and leveraging modern C++ standards for improved code maintainability and performance.

> 🐰✨  
> To the code we hop and bound,  
> Where `pthread` once was found.  
> Now `std::mutex` takes its place,  
> With `std::lock_guard` to embrace.  
> Thread safety, our newfound grace,  
> In our codebase's every space.  
> 🌟🐾

- **Refactor**
	- Enhanced thread safety across various parsers and components by standardizing the use of `std::mutex` for synchronization.
	- Optimized library dependencies and linking for improved integration.
	- Adjusted OCR timeout settings for better performance based on configuration values.
- **Bug Fixes**
	- Resolved potential threading issues by replacing outdated synchronization mechanisms with modern, safer alternatives.

## Version 2024.03.14

The project has undergone significant refactoring to enhance document processing capabilities by updating exception handling, refactoring class structures, and improving parser management. These changes aim to streamline document parsing, making the system more robust and easier to maintain.

> 🐇✨📜  
> In the realm of code, where parsers play,  
> A rabbit hopped, refactoring away.  
> "Out with the old, in with the new,  
> Let's parse documents, efficient and true."  
> With a twitch of its nose, and a flick of its ear,  
> The code now runs smooth, oh so clear.  
> 🌟🐾

- **New Features**
    - Enhanced document processing capabilities with improved format parsing.
    - Introduced a new functionality for importing and parsing files based on file extension or content.
- **Refactor**
    - Replaced runtime loading of parser providers with compile-time templates for static linking, enhancing performance and reliability.
    - Updated exception handling across various parsers for better clarity and error reporting.
    - Transitioned to using `Importer` class across the application, removing dependencies on deprecated classes.
- **Bug Fixes**
    - Fixed inconsistencies in error handling by standardizing exceptions.
- **Documentation**
    - Updated documentation to reflect changes in parsing and error handling mechanisms.

## Version 2024.03.01

The recent updates focus on enhancing the GitHub Actions workflow and build scripts for better integration with GitHub Packages as a binary cache. It introduces a permissions adjustment, shifts from local caching strategies to leveraging GitHub Packages for NuGet, and emphasizes debugging capabilities. These changes aim to streamline development processes, improve cache utilization, and facilitate easier debugging.

> 🐇✨  
> In the realm of code where binaries dwell,  
> A rabbit hopped, casting a magical spell.  
> "Away with the old, in with the new,  
> Let packages flow, and caches renew."  
> With a twitch of its nose and a flick of its tail,  
> Debugging turned simpler, and builds set to sail.  
> 🌟📦

- **New Features**
	- Introduced GitHub Packages nuget cache for public readonly cache usage, enhancing build efficiency.
- **Chores**
	- Updated build configurations for both Windows and Linux environments to support the new caching mechanism.
	- Adjusted environment variables and logging settings for improved debugging capabilities.

## Version 2024.02.27

The recent updates introduce an expansion and renaming of the supported large language models (LLMs), enhance OCR capabilities, refine text processing logic, and adjust sentiment detection to use a different LLM model. Additionally, there's an update in the documentation regarding available GPT models and minor modifications in the codebase, including a new instruction in the Classify constructor and an update in header file inclusion.

> 🐰✨  
> Changes are afoot, both big and small,  
> In the world of code, where we heed the call.  
> Models grow wise, and OCR sees clear,  
> Sentiment's grasp, now draws near.  
> Through files and docs, our journey we trace,  
> A hop, a skip, in the digital space.  
> 🥕🌟  

- **New Features**
	- Expanded and updated the list of supported LLM (Language Learning Models) for enhanced functionality.
	- Enhanced OCR (Optical Character Recognition) capabilities for better text recognition.
	- Refined text processing logic for improved efficiency.
	- Adjusted sentiment detection to utilize a different LLM model for more accurate analysis.
- **Documentation**
	- Updated the documentation to reflect the new list of available OpenAI GPT models.
- **Refactor**
	- Modified the `Classify` constructor to include a new instruction for category name responses.
	- Added a new header file inclusion for improved logging capabilities.

## Version 2024.02.25

The recent updates enhance the project's compatibility, performance, and user experience. Key improvements include expanding operating system support, refining build processes, and enhancing error handling. Additionally, the project's dependency management and testing capabilities have been significantly improved, ensuring a more robust and efficient development cycle. These changes collectively contribute to a more versatile and user-friendly software, catering to a wider audience and facilitating smoother project contributions.

> 🐇✨
> To the code that now flies on wings of change,  
> A hop, a skip, in binary plains so wide and strange.  
> Through windows new, and paths untrod,  
> We weave our spells, in the realm of code, we prod.  
> Let's cheer for the craft, so deftly enhanced,  
> By paws that typed, and at problems glanced.  
> 🚀🎉

- **New Features**
    - Added support for Windows 2022 in the build process.
    - Introduced conditional cloning of a GitHub repository in build scripts.
    - Enhanced testing and archiving logic in build scripts.
    - Added export functionality for `docwire` and included version and ABI information in archives.
    - Improved error handling and specific error messages in `docwire.cpp`.
    - Updated environment setup scripts for better dependency handling and setup efficiency.

- **Refactor**
    - Adjusted package installations and environment variables based on the operating system in build configurations.
    - Simplified compiler information handling and installation.
    - Enhanced dynamic configuration capabilities in log verbosity settings.

- **Bug Fixes**
    - Fixed MSVC 2022 build errors and improved dependency handling in GitHub Actions.
    - Updated conditional file renaming logic in `portfile.cmake` for better accuracy.

- **Chores**
    - Updated actions versions and modified the naming convention for job runs in GitHub workflows.
    - Adjusted artifact upload action version and naming convention.

## Version 2024.02.02

This update brings significant improvements to document processing, with a focus on enhancing parsing capabilities and exporting functionalities. Key upgrades include incremental parsing results, better handling of tables, hyperlinks, paragraphs, and support for headers and footers in exports. Additionally, there's a notable change in the C++ standard for OSX targets, alongside comprehensive enhancements across parsing and writing components to streamline document handling and presentation.

> In the realm of code, where documents roam,  
> A rabbit hopped, making it home.  
> Parsing tables, links, and more,  
> With headers and footers, it did adore.  
> 🐇💻✨  
> "To improve, to enhance," it cheerfully sung,  
> A leap for docs, a job well done.

- **New Features**
	- Added incremental parsing for DOC format files, enhancing the handling of tables, hyperlinks, and paragraphs.
	- Introduced support for headers and footers in plain text and HTML exports.
	- Implemented `<header>` and `<footer>` tags in HTML output for better document structure.
- **Refactor**
	- Modified code to use `enum class` for table state tracking, improving readability and maintainability.
	- Updated text handling to process and tag content more efficiently.
	- Adjusted HTML and plain text writers to handle new header and footer functionality.
- **Tests**
	- Updated HTML output in tests to reflect new parsing capabilities, including structured tables, paragraphs, and properly formatted hyperlinks.

## Version 2024.01.25

This update enhances the project's support for macOS, particularly for Apple Silicon (ARM64) platforms, by updating continuous integration configurations, documentation, and script adjustments. It ensures broader compatibility and efficiency in the build process, addressing the evolving landscape of macOS development environments.

> In the realm of code, where the bits do hop,  
> A rabbit worked, never wishing to stop.  
> 🍏 For macOS, it leaped, with a bound so light,  
> Ensuring the builds, in the night, were right.  
> "To ARM64, we go!" it cheerfully said,  
> With every line of code, it merrily led.  
> 🐾 Through fields of green, where the data does flow,  
> Our CodeRabbit thrives, making the CI glow.  

- **New Features**
	- Expanded continuous integration support for Apple Silicon platforms.
	- Updated documentation for installing on different platforms with specific configurations.
- **Documentation**
	- Added platform-specific installation instructions in the README.
- **Bug Fixes**
	- Fixed a formatting issue in log normalization.
- **Tests**
	- Updated a test log entry for improved clarity.

## Version 2024.01.22

The recent modifications enhance the project's robustness and adaptability across various environments. By refining the build process, these changes incorporate dynamic feature selection and bolster code safety with the introduction of Address and Thread sanitizers. This comprehensive update ensures a more reliable and secure codebase, catering to diverse platforms and configurations. In addition efficiency boost in HTML parsing is introduced by optimizing whitespace management.

> 🐇✨  
> In the code's weave, under the moon's glow,  
> A rabbit hops, leaving trails in the snow.  
> With each bound and leap, sanitizers in tow,  
> Ensuring the code, safe and sound, will grow.  
> Oh, how the digital garden does flourish and flow!  
> 🌟🐾

- **New Features**
	- Enabled Address and Thread sanitizers when specific feature flags are enabled, enhancing security and stability.
- **Refactor**
	- Improved efficiency and readability in HTML parser whitespace handling.
- **Chores**
	- Updated build configurations to support a wider range of operating systems and improve code safety with sanitizers.

## Version 2024.01.21

The recent modifications introduce a significant enhancement to the project's continuous integration (CI) process. By incorporating a matrix strategy within the GitHub Actions workflow, the build process now efficiently runs across different operating systems and utilizes various sanitizers. This update not only streamlines the job structure and artifact handling but also marks an improvement in the project's documentation to reflect these changes.

> 🐇✨  
> In the land of code and byte,  
> A rabbit hopped into the night.  
> With a matrix wide and vast,  
> It refactored builds to run so fast.  
> Across the OSs, it did dance,  
> Giving every change a chance.  
> 🌟📜 "To improve is to change," it squeaked with glee,  
> "Behold the new CI strategy!"  

- **Refactor**
	- Improved the CI build process with a matrix strategy for various operating systems and sanitizers.

## Version 2024.01.20

Significantly enhances the capabilities of the DocWire SDK by integrating it with OpenAI Vision. The integration introduces advanced features and optimizations, including improved AI capabilities. The changes also include updates to environment variable handling, particularly the OPENAI_API_KEY, and the introduction of new classes and methods to support image processing and analysis. The upgrade to C++20 and the addition of functionalities such as the openai::find transformer, extended CLI options, and enhanced openai transformers mark a significant leap in the SDK's capabilities. Additionally, the new version includes improvements to testing configurations and dependencies, aiming for a robust and reliable development environment.

> In the garden of code, where ideas take flight,  
> DocWire blooms, a beacon of light.  
> With AI's touch, and vision so keen,  
> It sees the unseen, in the digital scene.  
> Quantum dreams, in documentation lie,  
> IoT whispers, beneath the digital sky.  
> Though not in code, their promise we hold,  
> A future of tech, daring and bold.  
> Through the wires, data takes its form,  
> In DocWire's embrace, it's transformed.  
> A digital garden, ever so bright,  
> Where bytes and bits, take their flight.  

- **New Features**
	- Enhanced DocWire SDK with AI capabilities
	- Integrated C++20 and OpenAI Vision for advanced data processing.
	- Added `openai-find` option for text and image search.
	- Implemented base64 encoding and various image detail handling in chat functionalities.
	- Expanded CLI options and made README examples testable.

- **Documentation**
	- Updated README with new capabilities and integration details.

- **Refactor**
	- Improved handling of environment variables and test configurations.

- **Tests**
	- Conditionally skipped tests based on the availability of `OPENAI_API_KEY`.
	- Updated test configurations for enhanced testing capabilities.

- **Bug Fixes**
	- Fixed handling of `OPENAI_API_KEY` across various scripts and configurations.

## Version 2024.01.11

This upgrade enhances DocWire SDK's capabilities in handling a wide range of data processing tasks, including robust HTTP capabilities, comprehensive network APIs, and efficient web data extraction. Upgrade to the C++20 standard is a significant part of the changes in the release.

> In the digital forge, where code takes flight,  
> DocWire embarks on a journey, bold and bright.  
> With C++20, it weaves its tale,  
> A modern saga, where efficiency prevails.  
> From text to web, it parses with grace,  
> Unlocking data, setting the pace.  
> A beacon of innovation, in the code sea,  
> DocWire shines, with C++20.  

- **Build and Documentation Updates:** Updated CMake configurations and improved project documentation. The project now utilizes the C++20 standard, integrating advanced features for more efficient data processing.
- **Core Enhancements:** Introduced and refined functionalities for data analysis, attachment processing, and basic parsing.
- **Parsing and Exporting:** Expanded capabilities for document parsing (including EML, HTML, iWork, ODF, OOXML, and PDF formats) and exporting data in CSV, HTML, and plain text formats.
- **Utility Improvements:** Enhanced logging, metadata handling, and miscellaneous utility functions for better performance and usability.

## Version 2024.01.05

### Archives Support

- Unveiling the DecompressArchives chain element for seamless handling of ZIP, TAR, RAR, GZ, BZ2, and XZ formats. This feature is now the default in the CLI utility, enhancing document processing efficiency.

### HTML Support Improvements

- Significantly enhancing HTML parsing with incremental tagging for document elements and a refined table layout algorithm, leading to more accurate HTML to plain text conversion. The improved whitespace support in the inline formatting context ensures precise rendering.

### OpenAI Models

- Integrating with the OpenAI API, introducing transformers like openai::Chat, openai::TranslateTo, openai::Summarize, openai::ExtractEntities, openai::Classify, openai::ExtractKeywords, openai::DetectSentiment, openai::TextToSpeech, and openai::Transcribe. Additionally, extending support for models such as gpt-3.5-turbo, gpt-4, and gpt-4-1106-preview broadens the spectrum of available options for users.

### Other SDK Improvements

- Introducing the CsvWriter class for efficient CSV handling.
- Separating pst/ost and eml parsers into a dedicated plugin for improved modularity.
- Incorporating the Input class (a wrapper for stream or filename) and the ChainElement class (the base class for importer, exporter, and transformer) to streamline workflows.
- Enhancing support for subparsers tag signals in the EML parser.
- Upgrading OCR support to include all Tesseract OCR languages and enabling processing in multiple languages in a single OCR process.

### Build System and Infrastructure

- Abandoning Docker for GitHub CI, and adopting improved caching of build environment images in GitHub Packages, resulting in faster build times.
- Implementing GitHub Actions Cache to store compiled dependencies for increased efficiency.
- Upgrading GoogleTest to version 1.12.1 for robust testing.
- Dynamically linking MSVC runtime for all libraries and executables, enhancing runtime efficiency.
- Introducing Address and Thread sanitizers in the CMake options, VCPKG features, and GitHub CI to detect and fix memory and thread errors.
- Refactoring building process to use VCPKG.
- Upgrading VCPKG and SDK dependencies to version 2023.11.20 for enhanced stability.
- Incorporating VCPKG for managing dependencies and using VCPKG triplet names as platform suffixes.
- Utilizing VCPKG export command to create binary archives, streamlining the build process.
- Introducing the http::Post transformer for efficient HTTP POST operations.

### Documentation

- Strategic improvements in documentation, with README.md serving as the primary page in Doxygen API documentation.
- Consolidating examples in README.md and removing obsolete documentation chapters for a more focused and user-friendly experience.
- Adding automatic generation of the 3rd-party components licensing documentation chapter for transparency.

### Other Changes

- Renaming DocToText to DocWire SDK for a more impactful brand representation.
- Streamlining the search strategy for plugins, resources, and test files.
- Introducing logic-specific error exception classes (LogicError and RuntimeError) for improved code readability and management.
- Refactoring the CLI to use Boost.Program_options for easy extensibility.
- Seamless extraction of the version number from ChangeLog for source archive builds, enhancing versioning practices.
- Addressing EML parser crashes for unknown attachment file name extensions.
- Resolving PDF parser infinite loop issues when the font is unknown.
- Fixing HTML parser memory write errors.

### Summary

These updates collectively contribute to a more robust, feature-rich, and user-friendly DocWire SDK experience, with enhanced OCR capabilities, advanced sanitization, improved dependency management, and versatile OpenAI model integration.

Version 5.0.9:
    * Replace README file with more detailed README.md prepared for public GitHub repository
    * Added 48 filename extensions/formats as recognized by TXTParser: asm, asp, aspx, bas, bat, c, cc, cmake, cs, conf, cpp, css, csv, cxx, d, f, fpp, fs, go, h, hpp, htm, html, hxx, java, js, json, jsp, log, lua, md, pas, php, pl, perl, py, r, rss, sh, tcl, vb, vbs, xml, xsd, xsl, yml, yaml, ws
    * Added TXTParser end of line characters normalization
    * Added TXTParser ability to extract printable character sentences from binary files (similar to Linux "strings" utility) that is enabled if character encoding cannot be detected

Version 5.0.8:
    * Update libpff to newest version in Linux, Mingw/Win32 and MacOSX builds to fix compilation error
    * DICOM parser plugin sources removed as this is an additional commercial plugin and will be developed in separate repository
    * Fixed libtiff install issue in MacOSX build

Version 5.0.7:
    * Added exception if proper parser was not found
    * Fixed recursive splitting debug symbols
    * 2-times nested tables support in plain text writer fixed
    * Unused update_version.sh script removed
    * Fixed get_self_path() function
    * Removed unnecessary scripts

Version 5.0.6:
    * Fixed wrong plugin path for OSX
    * Nested tables support in plain text writer fixed
    * MS Word Templates parsing (DOT files) enabled with a simple fix
    * Added missing suffix in version string

Version 5.0.5:
    * Changed attribute type from cont char* to std::string for TAG_LIST
    * fixed OSX build scripts
    * removed unnecessary files
    * added description for console application
    * fixed bad_alloc exception handling
    * fixed header of html output
    * removed TAG_SPACE and TAG_TAB

Version 5.0.4:
    * Building with CMake for full version numbers fixed

Version 5.0.3:
    * Ordered and unordered lists API redesigned.
    * Fixed memory problem for OSX
    * PST parser API redesigned (tags)
    * Page close tag renamed

Version 5.0.2:
    * Fixed pdf parsing if CMap is unavailable
    * Fixed namespaces
    * tableStyleId tag support added to fix crash with some PPTX documents
    * Ignore all XML text nodes that contains only whitespaces to fix crashes with some ODF and OOXML documents
    * Add support for T tag in OOXML formats with whitespace preserve attribute

Version 5.0.1:
    * Empty table row problem fixed in plain text exporter
    * Fixed problem with char width calculation in pdf parser
    * Added exception if file doesn't exist
    * Added debug version for MSVC
    * Added sendTag method in Parser class
    * Added TAG_PAGE tag in pdf parser
    * Added section about tags to documentation
    * Added section about parser parameters to documentation
    * Added Info structure constructor
    * Changed extension from hpp to h for several files
    * Fixed location of temporary pst files to system defined temporary directory
    * Fixed problem with incorrect support for chars in html parser
    * Fixes problem with sending tags in pst parser

Version 5.0.0:
    * Introduced boost signals mechanism
    * Added examples for doctotext api
    * Updated documentation
    * Added next level api (Importer, Exporter, Transformer)
    * Refactoring
    * Added plugins mechanism for parser providers
    * Added support for hyperlinks in new api
    * Added support for msvc
    * 3rdparty directory removed (moved to build environment)
    * Made CMake script platform independent
    * Added support for lists in new api (ODF)
    * Fixed file validation
    * support table for Open Document Formats
    * Added support for hyperlinks in new api
    * Fixed support for odp files
    * Support attachments in eml in new api
    * Fixed xlsx parser (table formatting)
    * Fixed logs for encrypted files
    * Fixed comment support
    * Removed old api
    * Fixed formatting table - multiple line in cell

Version 4.6.1:
	* Fixed problem with unsupported exceptions in c api
	* Switch to DCMTK version 3.6.7 on windows version
	* Build 3rd party libraries with C++17 standard flags

Version 4.6.0:
	*  Updated tesseract library to version 5
	*  Implemented color inversion from white to black text

Version 4.5.0:
	* Added callbacks for style tags in odfooxml parser
	* Added HtmlWriter
	* Added tags with attributes for pst parser (mail-header, folder-header, attachment-header, mail-body, attachment-body)
	* Added filter mechanism for callbacks
	* Added set of standard filters (filterByFolderName, filterByAttachmentType, filterByMailMinCreationTime, filterByMailMaxCreationTime)
	* Fixed bug in pdf parser (missing check if object is valid string or hex string)
	* Added missing api headers: parser_builder.h parser_parameters.h, parser_provider.hpp, parser_wrapper.h, standard_filter.h, writer.h plain_text_writer.h html_writer.h

Version 4.4.2:
	* Added functions in old APIs to choose different "tessdata" directory for
	OCR parsing

Version 4.4.1:
	* Significant binary files size optimization

Version 4.4.0:
	* Significant OCR parser speed improvements
	* Filters support added to PST/OST parser
	* Possibility to cancel OCR parser process added to new API
	* Build environment updated to Debian Bullseye
	* Formatting style support for PST/OST and HTML parser in new API
	* Hardcoded OCR process timeout added to old API
	* CI environment separated from build environment
	* Multithreaded build and automatic tests

Version 4.3.0:
	* DICOM (Digital Imaging and Communications in Medicine) file format parser added
	* Small improvements in new API
	* Missing support for reading data buffer in OCR parser added
	* Unitialized formatting value error in HTML parser in new API fixed
	* Documentation and copyrights headers updated with new file formats

Version 4.2.2:
	* Remove CMake cache from binary packages
	* Add missing ChangeLog file to binary packages
	* Update documentation with new API and new file formats
	* Fix library file name after CMake introduction
	* Fix automatic tests after CTest introduction

Version 4.2.1:
	* Multithreading problems fixed: libmimetic, getpwuid, gmtime
	* CMake and CTest introduced instead of handwritten Makefiles
	* ODF_OOXML parser, image (OCR) parser, TXT parser and RTF parser added to new experimental API
	* PST/OST parser added to old API (with hardcoded limit for number of e-mails processed)
	* OCR parser support in old C API fixed
	* Automatic tests for old C API fixed and improved

Version 4.2.0:
	* Image parser (OCR) added with support for tiff, jpeg, bmp, png, pnm, jfif, jpg and webp file formats
	* Personal Storage Table (.pst) and Off-line Storage Table (.ost) parser added with support for Microsoft Outlook and Microsoft Exchange files
	* New experimental API added with callback support for quick handling of partial results and preserving the structure of documents

Version 4.1.2:
	* PDF parser improved significally (documents that did not open, word spacing, etc.)
	* PDF parser refactoring started.
	* Added support for EML and ODF_OOXML parsers in C API.
	* Fixed problem with duplicated content in EML parser.

Version 4.1.1:
	* Debug symbols separated to addditional files.

Version 4.1.0:
	* PlainTextExtractor can throw exceptions (doctotext::Exception)
	* Improved whitespace support in OpenDocument formats.
	* Debug symbols enabled by default.
	* Building Linux version process cleanups.
	* Building Win32 version process cleanups.
	* Memory error in EML parser fixed.

Version 4.0:
	* fixed bug in utf8 encoding for RT_TextBytesAtom record in PPT parser
	* Initial Open Document Flat XML Parser (fodt, fods, fodg, fodp)
	* Initial EML Parser (with ability to extract attachments)
	* Capabilities of C API has been expanded.
	* Better charset detection in HTML parser.
	* New TXT parser (Can be used to change encoding to UTF8)
	* ListStyle is a class now (not an enum).
	* Initial PDF parser.
	* Whole public interface of DocToText (PlainTextExctractor, Metadata, Link, Exception, FormattingStyle) is available under doctotext namespace.
	* DocToText supports exceptions now (Exception class).
	* Reorganization of url handling. PlainTextExtractor can now return list of links. Supported parsers: HTML/EML/ODF_OOXML/ODFXML.
	* PlainTextExtractor allows now for parsing files from memory buffer.
	* Independence from glib, libgsf, gettext. Pthreads are used instead of gthreads, build-in OLEReader is used instead of libgsf.
	* New iWork parser.
	* New XLSB parser.
	* Extracting number of pages from ODG files fixed.
	* ODG files added to automatic tests.
	* Support for Object Linking and Embedding (OLE) in ODF formats added.
	* Managing libxml2 (initialization and cleanup) can be disabled.
	* Thread-safety fixed in ODF, OOXML and DOC parsers.
	* Better handling of fields in DOC and DOT files.
	* Better handling of headings in ODF documents.
	* Fixes for x86_64 architecture.
	* Improved stability in multithreaded environments.
	* Embedded XLS Workbooks in DOC files supported without creating temporary files.
	* Cleanups in ODF and OOXML parsers.
	* Memory consumption of ODF and OOXML parsers significantly reduced.
	* Better handling of fields in DOCX files.
	* Fixed crash in RTF parser for invalid files.
	* Fixes in XLS parser.
	* Initial port to win64 architecture.
	* Function enter and exit tracking feature useful for debugging.

Version 0.14.0:
	* Initial HTML format support.
	* Initial implementation of metadata extractor (author, creation time, last modified by, last modification time, page count, word count
	* Font commands in XLSX header or footer do not corrupt output any more.
	* Estimate not existing meta values using different techniques.
	* Initial implementation of annotations support (ODT, DOC, DOCX, RTF formats).
	* Download and use precompiled wv2 binaries
	* Initial API documentation (Doxygen)
	* Fallback to other parsers if specified parser fails
	* Fixes and improvements in XLS parser.
	* C++ API extended to allow communicating without STL objects (to mix two STL implementations for example).
	* Line break handling in ODF and OOXML formats fixed.

Version 0.13.0
	* Initial Mac OS X port
	* Initial implementation of shared library
	* C language API added to allow using C++ library from C and other languages
	* Universal binary (i386, x86_64) for Mac OS X
	* Static linking of 3rdparty libraries option for Mac OS X
	* Cleanups
	* First parser selection is according to file extension
	* First parser used can be choosen via command line
	* Embedded XLS Workbooks support in DOC files
	* Table cells encoding problems in DOC files fixed.
	* Case insensitive file extension matching.
	* Do not try other parsers if parser selected by file extension fails.
	* New XLS parser.
	* Small fix in XLS numbers formatting.
	* Small fix in parsing XLS shared string table.
	* Locating files in zip archives (ODF, OOXML) optimized.
	* Initial PPT format support. ODP, PPT, PPTX files added to automatic tests.
	* Fixes in DOC and XLS parsers.
	* Verbose logging can be turned on and off.
	* Logs can be redirected to other C++ stream than standard cerr.
	* Redesigned C++ API.
	* Debug and release versions separated.
	* End of paragraph special symbols support fixed.
	* Download and use more precompiled libraries
	* Small and big compilation fixes
	* Headers and footers support in DOC files added

Version 0.12.0:
	* Formatting tables optimized
	* Build-in libxml2 library upgraded to version 2.7.7
	* Initial xls format support
	* Better default table formatting
	* Support for DOC format moved to DOCParser class
	* RTFParser API uses standard string class only
	* Cleanups
	* Error message is not displayed when checking for ODF or OOXML format fails
	* Table parsing in DOC files improved
	* Link parsing in DOC files improved
	* Cleanups
	* Copyright headers updated
	* Packages in bzip2 format instead of gzip
	* ODS (OpenOffice Calc) files added to automatic tests
	* VERSION file added to binary packages
	* Download and use precompiled libiconv binaries
	* Encoding problems in XLS format support on win32 fixed
	* Use MAKE variable executing make recursively

Version 0.11.0:
	* zip archives (ODF, OOXML) support moved to DocToTextUnzip class
	* --unzip-cmd option implemented to use specified command to unzip files from archives (ODF, OOXML) instead of build-in decompression library
	* --unzip-cmd fixed on win32 - replacing slashes with backslashes in filename
	* --fix-xml option implemented to try to fix corrupted xml files (ODF, OOXML) using custom recursive descent parser before processing
	* some ODF and OOXML formatting issues fixed
	* UTF-8 support in corrupted xml files (ODF, OOXML) fixing routines
	* Makefile dependencies fixed
	* --strip-xml option implemented to strip xml tags instead of parsing them (ODF, OOXML)
	* Entities support in corrupted xml files (ODF, OOXML) fixing routines
	* max tag depth security limit of libxml supported in corrupted xml files (ODF, OOXML) fixing routines
	* a lot of compilation warnings fixed
	* small fix for encoding support in RTF parser
	* --strip-xml option removes duplicated attributes
	* ChangeLog file added to binary releases
	* copyright headers updated

Version 0.10.0:
	* Command line arguments added to change tables, lists and links formatting (for odt files only)
	* glib upgraded to 2.14.5
	* initial OOXML formats support
	* Parsing tables in DOC fixed
	* support for mixed character encodings RTF files created using MS WordPad or MS Word (thanks to John Estrada)
	* ioapi of unzip library changed from winapi to fopen api in win32 version - winapi io in unzip library caused some problems, for example xlsx file could not be open when is open in MS Word
	* allocating memory using expotential grow in UString - big performance boost in rtf parser
	* copyright headers updated


Version 0.9.0:
	* patch for gettext fixing duplicate case value errors
	* glib upgraded to 2.12.11 - duplicate case value errors
	* wstring changed to UString in rtf parser - problems on mingw
	* initial support of charset in rtf parser
	* initial odt format support
	* performing automatic tests after the source code is compiled
	* memory error fixed - unzReadCurrentFile() does not put NULL at the end of the buffer
	* counting function added
	* number of characters read from file was changed to corect value


Version 0.2.0:
	* rtf format support

Version 0.1.0:
	* initial implementation
	* output in utf-8 encoding
	* help, copyright headers, README
	* paragraphs support
