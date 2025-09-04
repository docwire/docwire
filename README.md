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
- [Error handling: robust and secure](#error-handling)
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

If you've experienced the power of DocToText, brace yourself for a substantial upgrade. Staying ahead with C++20, we integrate advanced features such as ranges and concurrency. DocWire SDK evolves to handle a spectrum of data processing tasks, including robust HTTP capabilities, comprehensive network APIs, efficient web data extraction, and running local AI models.

With the addition of local LLM models, DocWire SDK now enables developers to leverage state-of-the-art natural language processing capabilities right in their C++ applications. This feature allows developers to perform tasks such as text classification, sentiment analysis, named entity recognition, and many more, directly on their data without the need for remote API calls.

We understand the need for comprehensive solutions in today's development landscape. DocWire SDK seamlessly integrates into C++ applications, providing solutions for text extraction, data conversion, data scraping, data mining, and more.

### Optimized for NLP and AI Projects

DocWire SDK is engineered to be the cornerstone of your NLP and AI projects. With its advanced data preparation capabilities, it ensures that your input data is of the highest quality, which is crucial for the performance of your models.

- **Data Cleaning and Preprocessing**: Before feeding data into your NLP models, it's essential to clean and preprocess it. DocWire SDK provides powerful tools to remove noise, correct formatting issues, and standardize data, ensuring that your models are trained on clean and relevant datasets.

- **Content Filtering and Removal**: Unwanted content can skew the results of your NLP models. DocWire SDK allows you to define rules to filter out irrelevant sections, such as boilerplate text, disclaimers, or headers and footers, ensuring that only pertinent content is passed to your models.

- **Local LLM Model**: DocWire SDK integrates support for local LLM models, allowing you to leverage state-of-the-art natural language processing capabilities right in your C++ applications. This feature allows developers to perform tasks such as text classification, sentiment analysis, named entity recognition, and many more, directly on their data without the need for remote API calls.

- **Tokenization and Detokenization**: DocWire SDK provides a convenient interface for tokenizing and detokenizing your data. Tokenization breaks down text into individual words or tokens, which can then be fed into your NLP models. After processing, the SDK's detokenization feature allows you to reconstruct the original text from the tokenized output, ensuring that the integrity and context of the document are preserved.

- **Output Sanitization**: When integrating NLP models into production systems, it's vital to sanitize outputs to prevent the leakage of sensitive information. DocWire SDK offers features to detect and redact personal data, confidential information, and other sensitive content from your outputs.

- **Downstream Application Readiness**: The quality of data not only affects model training but also the performance of downstream applications. DocWire SDK prepares and sanitizes your data to ensure that it meets the requirements of subsequent systems, whether they are analytics platforms, customer relationship management tools, or content management systems.

By incorporating DocWire SDK into your workflow, you can significantly enhance the quality of your NLP and AI applications, leading to more accurate insights and better decision-making.

### Enhanced Support for Embeddings and AI/NLP Integration

DocWire SDK provides a robust foundation for AI/NLP developers to build upon. By utilizing the SDK's capabilities in partitioning and chunking document elements, developers can create embeddings for each element, which are essential for advanced AI applications such as Retrieval Augmented Generation (RAG), semantic search, and more.

- **Partitioning for Embeddings**: DocWire SDK's partitioning feature allows for the detection of distinct document elements, which can then be fed into embedding models to generate vector representations. These embeddings can be used to understand the semantic meaning of each element, facilitating tasks like document clustering and similarity analysis.

- **Chunking for Contextual Embeddings**: The SDK's chunking functionality groups related document elements, enabling the generation of contextual embeddings. This is particularly useful for applications that require an understanding of the document's structure and thematic content, such as summarization and topic modeling.

- **Embeddings in AI Workflows**: By integrating DocWire SDK with popular machine learning frameworks, developers can streamline the process of obtaining embeddings for their NLP models. This integration simplifies the workflow, from data extraction to embedding generation, making it more efficient and developer-friendly.

- **Use Cases and Applications**: The ability to obtain embeddings from partitioned or chunked document elements opens up a plethora of use cases. For instance, in Retrieval Augmented Generation (RAG), embeddings can be used to retrieve relevant document segments to augment the generation process. Similarly, in semantic search, embeddings enable the matching of query intent with document content, improving search relevance and accuracy.

By enhancing DocWire SDK with these capabilities, we aim to provide a comprehensive toolkit for AI/NLP developers, enabling them to leverage the full potential of embeddings in their projects.

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
- **Enhanced Structured Content Extraction for AI Applications**: We are committed to advancing the capabilities of DocWire SDK to meet the growing demands of AI and NLP developers. Our roadmap includes the development of more sophisticated algorithms for structured content extraction, enabling the SDK to identify and categorize document elements with greater precision. This will include improved recognition of semantic structures within the text, such as headings, subheadings, and thematic breaks, which are crucial for training AI models in tasks like document summarization, topic modeling, and content classification.
- **Customizable Content Selection for Model Training**: Recognizing the diverse needs of AI applications, we plan to introduce customizable content selection features within the SDK. This will empower developers to specify which document elements should be included or excluded during the data extraction process, tailoring the dataset to the specific requirements of their AI models. Whether it's extracting dialogue from scripts, narrative passages from articles, or key points from reports, DocWire SDK will provide the flexibility needed for targeted model training.

- **Future Enhancements for Chunking and Semantic Analysis**: Looking ahead, DocWire SDK will continue to evolve its chunking and semantic analysis capabilities. We plan to introduce more nuanced algorithms that can detect and utilize specific knowledge about each document format, further refining the partitioning of documents into semantic units. This will enable developers to work with even more granular and contextually relevant data segments, tailored to the unique demands of their AI and NLP projects.

- **Intelligent Partitioning for Diverse Document Formats**: As part of our commitment to supporting nearly 100 data formats, future iterations of DocWire SDK will feature enhanced partitioning logic that is acutely aware of the idiosyncrasies of each document type. Whether dealing with structured spreadsheets, narrative-rich articles, or hybrid formats, the SDK will offer tailored chunking strategies that respect the inherent structure and intended use of the content.

- **Seamless Integration with LLMs and NLP Libraries**: We are dedicated to ensuring that DocWire SDK remains at the forefront of compatibility with leading Large Language Models (LLMs) and NLP libraries. Future updates will focus on streamlining the integration process, allowing developers to effortlessly incorporate the SDK's chunking and semantic analysis features into their AI-driven workflows.

- **Integration with Advanced NLP Libraries**: To further support the development of AI and NLP applications, DocWire SDK will integrate with advanced NLP libraries, providing out-of-the-box support for common NLP tasks such as tokenization, part-of-speech tagging, named entity recognition, and sentiment analysis. This integration will streamline the workflow for developers, allowing them to preprocess and analyze text within the same framework used for data extraction.
- **Support for Custom AI Model Deployment**: As AI models become increasingly specialized, there is a need for frameworks that can accommodate custom model deployment. DocWire SDK will offer support for integrating custom-trained AI models, enabling developers to leverage their proprietary algorithms within the data processing pipeline. This will facilitate a seamless transition from model training to deployment, ensuring that developers can maximize the performance and accuracy of their AI applications.

- **Embeddings Extraction Interface**: Extend existing interface within DocWire SDK that simplifies the process of extracting embeddings from document elements. This interface will allow developers to plug in their preferred embedding models and obtain embeddings directly from the SDK's output.

- **Embeddings-Based Retrieval**: Develop functionalities that leverage embeddings for document retrieval, enabling applications such as RAG to efficiently find and utilize relevant document segments based on semantic similarity.

- **Embeddings Storage and Management**: Implement storage solutions within DocWire SDK for managing and querying embeddings, making it easier for developers to handle large volumes of embedding data.

- **Embeddings Visualization Tools**: Provide visualization tools that help developers understand and analyze the embeddings generated from document elements, aiding in model tuning and data exploration.

- **Top-notch Documentation and Support**: Expect nothing but the best. We are committed to providing high-quality documentation and fanatical customer support, ensuring a smooth and efficient experience for every user.

### Examples of Input and Output Sources
- **Email Boxes**: Effortlessly extract valuable information from email boxes, streamlining the integration of email data into your processing workflows.
- **Cloud Drives**: Seamlessly import/export data from popular cloud drives, ensuring that your data processing isn't bound by storage location.
- **Local Filesystem**: Process data directly from local filesystems, providing flexibility in handling data stored locally.
- **ERP Systems**: Integrate with ERP systems for streamlined data processing in enterprise resource planning.
- **Databases**: Connect seamlessly with databases, enabling efficient extraction and manipulation of structured data.
- **Web Services**: Extract data from web services, providing real-time integration capabilities.

### Research and Development Goals

In our pursuit of excellence, DocWire SDK is committed to ongoing Research and Development. Our R&D goals are designed to address the evolving challenges in data processing and to leverage cutting-edge technologies to provide our users with a robust and efficient toolkit. Our current R&D goals include:

- **Enhanced AI Capabilities**: We are continuously pushing the boundaries of AI integration by exploring new models and refining existing ones to provide state-of-the-art data processing. This includes running large AI models locally, which presents challenges such as managing model size, optimizing speed, and ensuring high-quality outputs. Addressing these issues is crucial for enabling powerful on-device AI without the need for constant cloud connectivity.

- **Advanced Data Sanitization Techniques**: We are developing sophisticated data sanitization techniques to further enhance the safety and privacy of AI models. These techniques will enable the SDK to automatically detect and redact sensitive information, ensuring that data used for training and inference is free from personal identifiers and confidential content.

- **Customizable Data Preparation Workflows**: Recognizing the diverse needs of AI and NLP applications, we are working on customizable data preparation workflows. These workflows will allow developers to define and automate the steps required to clean, preprocess, and format data for their specific use cases, streamlining the path from raw data to actionable insights.

- **Enhanced Support for Multilingual Data Processing**: As AI applications become increasingly global, the need for multilingual data processing grows. We are enhancing DocWire SDK's capabilities to handle a wide range of languages and scripts, ensuring that developers can prepare data for multilingual models with ease and accuracy.

- **Long-Term Memory for AI Models**: To enhance the performance of AI models, we are researching the integration of vector databases and other innovative solutions that can provide long-term memory capabilities. This will allow AI models to retain and access vast amounts of data efficiently, improving their predictive accuracy and contextual understanding.

- **AI Document Layout Analysis**: Recognizing the importance of extracting information from unstructured data, we are developing AI-driven document layout analysis tools. These tools will enable the SDK to understand and process various document formats, making it easier to extract and utilize information from diverse data sources.

- **Advanced Semantic Partitioning**: Our R&D team is exploring cutting-edge techniques for advanced semantic partitioning of documents. This research aims to enhance the SDK's ability to understand and delineate semantic units within a wide array of document types, facilitating more effective data segmentation for AI and NLP applications.

- **Embeddings Optimization**: Research techniques to optimize the generation of embeddings from document elements, focusing on speed and accuracy to support real-time NLP applications.

- **Contextual Embeddings Research**: Investigate advanced methods for generating contextual embeddings that capture the document's narrative flow and thematic elements, providing richer inputs for AI models.

- **Embeddings for Cross-Lingual Applications**: Explore the development of embeddings that support cross-lingual NLP tasks, enabling applications to process and understand documents in multiple languages.

- **Embeddings in Unsupervised Learning**: Study the use of embeddings in unsupervised learning scenarios, such as clustering and anomaly detection, to uncover hidden patterns and insights within large document corpora.

- **Context-Aware Chunking Algorithms**: We are developing context-aware chunking algorithms that can intelligently segment documents while preserving the narrative flow and logical connections between sections. This will enable AI models to process and generate content with a higher degree of coherence and relevance.

- **Document Element Detection and Classification**: To further improve the SDK's chunking capabilities, we are working on algorithms for the detection and classification of document elements. This will allow the SDK to identify headers, footers, sidebars, and other structural components, enabling more precise and meaningful data extraction for AI applications.

- **Optimization with C++ Meta-Programming**: Utilizing C++ meta-programming techniques, we aim to optimize data processing speed by enabling compile-time optimizations and reducing runtime overhead. This can lead to significant performance gains in data-intensive applications.

- **Caching Techniques**: We are exploring advanced caching techniques for time-consuming and costly operations. By intelligently storing and reusing data, we can reduce latency and improve the efficiency of repeated processes, which is especially beneficial in scenarios with limited computational resources.

- **Processing of Encrypted Data**: To address the growing need for privacy and security, we are developing algorithms that enable the processing of encrypted data, allowing for zero-knowledge processing. This will ensure that sensitive data can be analyzed and processed without compromising its confidentiality.

- **Quantum Computing Integration**: We continue to investigate the potential benefits of quantum computing techniques for accelerating data processing tasks. By staying abreast of developments in quantum algorithms, we aim to position DocWire SDK at the forefront of technological advancements.

- **Blockchain Integration**: We are exploring the utilization of blockchain technology for secure and transparent data processing. Blockchain can provide a decentralized and tamper-proof ledger, which is particularly useful for applications requiring high levels of data integrity and traceability.

- **IoT Device Support**: We are expanding the integration of DocWire SDK with IoT devices, enabling it to process data generated by the Internet of Things. This will facilitate real-time data analysis and decision-making in IoT ecosystems, unlocking new possibilities for smart applications.

By focusing on these R&D goals, DocWire SDK aims to solve significant problems faced by developers, such as handling large-scale data, ensuring data privacy, and optimizing processing speeds. Our commitment to innovation will help users harness the full potential of modern data processing technologies.

**Embark on an exciting journey with DocWire SDK, where the philosophies of C++ converge with the power of data processing innovation. Explore the limitless possibilities today and witness the substantial enhancements that set it apart from its predecessor, DocToText.**

<a name="features"></a>
## Features

- **Secure and privacy-preserving data processing** - all operations can be done locally without sending data to the cloud
- Able to extract/import and export **text, images, formatting, and metadata along with annotations**
- **Data can be transformed** between import and export (filtering, aggregating, translation, text classification, sentiment analysis, named entity recognition etc).

- **Equipped with multiple parsers**:
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
    - DICOM (DCM) as an additional commercial library

- **Equipped with multiple exporters**:
    - Plain text: Easily extract and export text content.
    - HTML: Export content in HTML format for web use.
    - CSV: Export data to Comma-Separated Values format.
    - XLSX and more are coming: Additional export formats for diverse use cases.

- **Fast and accurate file format detection**: Leveraging file signatures, file name extensions, content analysis, and MIME type recognition, the DocWire SDK automatically detects the format of any file. This ensures that the appropriate parser is selected for processing. With its ability to identify various formats, DocWire SDK serves as a versatile tool for numerous file processing tasks. For more information, see the following functions and classes: [content_type::detector](https://docwire.readthedocs.io/en/latest/classdocwire_1_1content__type_1_1detector.html) and [content_type::detect](https://docwire.readthedocs.io/en/latest/namespacedocwire_1_1content__type.html#ab8fcce329158e74aed1c402df93fa4e4). [You can find example how to perform file type detection (with or without document processing) here](https://docwire.readthedocs.io/en/latest/file_type_determination_8cpp-example.html).
Additionally, the SDK provides functionality to convert a MIME type back to a file extension via `content_type::by_file_extension::to_extension`, which can be useful in scenarios where a file name is needed.

- **Build-in powerful flan-t5-large model**: This state-of-the-art transformer-based model is designed for a wide range of natural language processing tasks, including text translation, question answering, summarization, text generation, and more. It has been trained on a diverse range of data sources and can handle complex linguistic phenomena such as word order, syntax, and semantics. The model's versatility and ability to perform multiple tasks make it a valuable addition to the DocWire SDK, allowing developers to leverage its capabilities for a variety of NLP tasks within their applications. **The build-in model is optimized to run with descent speed on lower-end desktops and mobile devices without GPU acceleration and consuming less than 1 GB of memory**

- **Build-in powerful multilingual-e5-small model**: This powerful and efficient multilingual text embedding model is designed to generate high-quality vector representations (embeddings) for text in over 100 languages. These embeddings are crucial for a wide range of NLP tasks, including semantic search, retrieval-augmented generation (RAG), text clustering, and similarity comparison. The `multilingual-e5-small` model is optimized for performance, making it suitable for applications where both speed and accuracy are important.

- **Support for running locally more powerful AI models using ctranslate2 technology**: This technology is designed to be highly scalable and efficient on CPU and GPU, enabling efficient deployment of language models on resource-constrained devices. Quantization allows maintaining accuracy with smaller model sizes making it suitable for desktop and mobile applications. Parallel and asynchronous execution enables efficient utilization of hardware resources.

- **Equipped with a high-grade, scriptable, and trainable local OCR** that has LSTM neural networks-based character recognition OCR capabilities in more than 100 languages and multiple languages in single picture.

- HTTP::Post transformer: Facilitate **seamless communication with external HTTP APIs or services**, enabling data exchange and integration with external systems.

- **Integration with OpenAI API for most advanced NLP tasks**, including transformers like:
    - TranslateTo: Translate text or image to different languages.
    - Summarize: Generate summarized content from longer texts or description of images.
    - ExtractEntities: Extract entities and key information from text and image.
    - Classify: Perform text and image classification and categorization.
    - ExtractKeywords: Identify and extract keywords from text and image.
    - DetectSentiment: Analyze and detect sentiment in text and image.
    - AnalyzeData: Perform data analysis on text content and images.
    - Find: Search for phrases, objects or events in text and images (object detection, event detection).
    - Chat: Conduct chat-based interactions and conversations with text input and image input.
    - TextToSpeech: Perform written text into spoken words (voice) conversion (TTS).
    - Transcribe: Convert spoken language (voice) into written text (transcription, Automatic Speech Recognition).
    - embed: Generate embedding of text via OpenAI.
   
   Supports multiple Open AI LLM models: gpt-5, gpt-5-mini, gpt-5-nano, gpt-5-chat-latest, gpt-4.1, gpt-4.1-mini, gpt-4.1-nano, gpt-4o, gpt-4o-mini, o3, o3-pro, o3-deep-research, o3-mini, o4-mini, o4-mini-deep-research, gpt-4o-transcribe, gpt-4o-mini-transcribe, whisper-1, gpt-4o-transcribe, tts-1, tts-1-hd, text-embedding-3-small, text-embedding-3-large, text-embedding-ada-002. More are coming.

- **Incremental parsing** returning data as soon as they are available

- **Structured Content Extraction**: DocWire SDK excels in transforming unstructured documents into structured content. By breaking down documents into discernible elements such as titles, paragraphs, list items, tables, and metadata, the SDK facilitates a granular level of control over the data extraction process. This feature is particularly beneficial for developers working with NLP and LLM models, as it allows them to selectively retain content that is most relevant to their application. For instance, when training a summarization model, users may opt to focus solely on narrative text, excluding headers, footers, and other non-essential elements. The SDK's structured content extraction capability ensures that the integrity and context of the document are preserved, enabling more accurate and meaningful interactions with AI-driven projects.

- **Preprocessing for Embeddings**: Use DocWire SDK to clean and preprocess text data, ensuring that the input to your embedding models is of high quality and free from noise.

- **Document Element Extraction**: Extract specific document elements such as paragraphs, headings, and lists, which can then be used to generate embeddings for each element.

- **Semantic Chunking**: Group related content using the SDK's chunking feature to create contextually rich embeddings that capture the nuances of the document's structure.

- **Integration with Embedding Models**: Once the data is preprocessed and structured, you can use [openai::embed](https://docwire.readthedocs.io/en/latest/classdocwire_1_1openai_1_1embed.html) or [local_ai::embed](https://docwire.readthedocs.io/en/latest/classdocwire_1_1local__ai_1_1embed.html) or integrate with your choice of embedding models, such as word2vec, GloVe, or BERT, to generate embeddings that can be used in various NLP tasks.

- **Enhancing AI/NLP Pipelines**: Embeddings obtained from DocWire SDK can be used to enhance AI/NLP pipelines, enabling more accurate and context-aware applications such as document classification, sentiment analysis, and information retrieval.

- **Advanced Document Chunking**: DocWire SDK introduces sophisticated chunking capabilities, leveraging metadata and document elements to partition documents into smaller, semantically coherent parts. This feature is invaluable for applications such as Retrieval Augmented Generation (RAG), where the ability to process and retrieve information from specific document segments can significantly enhance the performance of AI models. By understanding the structure and semantics of each document, DocWire SDK can intelligently divide content into meaningful units, preserving the context and coherence necessary for high-quality AI interactions.

- **Semantic Unit Coherence**: The SDK's chunking mechanism is designed to maintain the coherence of semantic units established during partitioning. This ensures that when a document is sp
lit into chunks, each piece retains its contextual integrity, making it suitable for use cases where understanding the flow and connection between segments is crucial, such as in conversat
ional AI, document summarization, and topic extraction.

- **NLP Model Training Data Preparation**: Prepare your training data with precision. DocWire SDK's structured content extraction is ideal for preparing datasets for NLP model training. By extracting only the relevant sections of text, you can create clean, focused datasets that lead to more effective and efficient model training.

- **Data Sanitization for AI Safety**: Ensure the safety and integrity of your AI models. DocWire SDK's data sanitization capabilities help prevent the introduction of biased, sensitive, or inappropriate content into your models, safeguarding the quality and reliability of your AI applications.

- **Customizable Data Cleaning Pipelines**: Tailor your data cleaning process to fit the unique needs of your project. With DocWire SDK, you can build customizable pipelines that automate the cleaning, normalization, and transformation of your data, saving time and reducing the potential for human error.

- **Seamless Integration with AI and NLP Libraries**: DocWire SDK is designed to work hand-in-hand with popular AI and NLP libraries. Its flexible API allows for easy integration, enabling you to preprocess data within the same framework used for model training and inference.

- **Enhanced AI and NLP Support**: DocWire SDK is not only a tool for data extraction but also a powerful ally for AI and NLP developers. With its advanced chunking and semantic unit preservation, the SDK is perfectly suited for preparing data for AI models that require a deep understanding of document structure and content. Whether you're working on machine learning, deep learning, or any other AI project, DocWire SDK provides the features you need to ensure your data is ready for the challenge.

- **Cross-platform**: Linux, Windows, MacOSX, and more to come
- **Can be embedded** in your application (SDK)
- **Can be integrated** with other data mining and data analytics applications
- **Parsing process can be easily designed** by connecting objects with the pipe `|` operator into a chain
- **Custom parsing chain elements can be added** (parsers, transformers, exporters)
- **Small binaries, fast** native C++ code

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

**AI-Enhanced Chunking and Semantic Analysis**:
The DocWire SDK API is designed to facilitate the integration of AI-enhanced chunking and semantic analysis directly into the PipeChain. This allows developers to apply AI-driven logic to the partitioning of documents, ensuring that chunks are created with a keen understanding of the content's meaning and structure. By incorporating AI models into the chunking process, developers can create data segments that are primed for high-level AI tasks such as summarization, question answering, and knowledge extraction.

### Uniting PipeChain and DataTree: Elevate Your Processing

**Comprehensive Structure**:
Merge the structured flow of PipeChain with the depth of information in DataTree, creating a revolutionary approach that is as comfortable as working with the DOM tree in HTML. Embrace a comprehensive system that navigates, transforms, and exports data seamlessly.

**Effortless Adaptability**:
Experience the synergy of PipeChain and DataTree, effortlessly adapting to diverse document structures. Elevate your document processing game with a harmonious blend of expressive code, structured information, and an efficient SAX parser-inspired processing model.

### AI-Enhanced Document Processing

**AI-Driven Insights and Transformations**:
DocWire SDK's API is designed to accommodate the integration of AI-driven insights and transformations. By incorporating AI models directly into the PipeChain, developers can enrich the data processing pipeline with advanced capabilities such as semantic analysis, context-aware summarization, and intelligent content categorization. This integration allows for the dynamic application of AI insights at various stages of the document processing workflow, enhancing the overall value and utility of the extracted data.

**Custom AI Model Integration**:
The API concept extends to support custom AI model integration, enabling developers to inject their own trained models into the processing chain. This flexibility ensures that the SDK can adapt to the unique requirements of specialized AI applications, providing a tailored processing environment that aligns with the developer's vision.

**Seamless AI and Data Processing Synergy**:
DocWire SDK's API fosters a seamless synergy between AI and data processing. By blending AI functionalities with traditional data extraction, the API offers a unified approach to document processing that leverages the best of both worlds. This synergy is particularly advantageous for developers working on cutting-edge AI projects, as it allows them to harness the full potential of their models within a robust data processing framework.

<a name="examples"></a>
## Examples

Parse file in any format (Office, PDF, mail, etc) having its path, export to plain text and write to string stream:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(out_stream.str() == "Data processing refers to the activities performed on raw data to convert it into meaningful information. It involves collecting, organizing, analyzing, and interpreting data to extract useful insights and support decision-making. This can include tasks such as sorting, filtering, summarizing, and transforming data through various computational and statistical methods. Data processing is essential in various fields, including business, science, and technology, as it enables organizations to derive valuable knowledge from large datasets, make informed decisions, and improve overall efficiency.\n\n");

  return 0;
}
```

Parse file in any format (Office, PDF, mail, etc) having stream, export to HTML and write to stream:

```cpp
#include "docwire.h"
#include <cassert>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::ifstream("data_processing_definition.docx", std::ios_base::binary) | content_type::detector{} | office_formats_parser{} | HtmlExporter() | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(out_stream.str() ==
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
      "<meta charset=\"utf-8\">\n"
      "<title>DocWire</title>\n"
      "<meta name=\"author\" content=\"\">\n"
      "<meta name=\"creation-date\" content=\"2024-01-14 16:22:52\">\n"
      "<meta name=\"last-modified-by\" content=\"\">\n"
      "<meta name=\"last-modification-date\" content=\"2024-01-14 16:35:33\">\n"
    "</head>\n"
    "<body>\n"
      "<p>Data processing refers to the activities performed on raw data to convert it into meaningful information. It involves collecting, organizing, analyzing, and interpreting data to extract useful insights and support decision-making. This can include tasks such as sorting, filtering, summarizing, and transforming data through various computational and statistical methods.</p><p>Data processing is essential in various fields, including business, science, and technology, as it enables organizations to derive valuable knowledge from large datasets, make informed decisions, and improve overall efficiency.</p></body>\n"
    "</html>\n");

  return 0;
}
```

Parse all files in any format inside archives (ZIP, TAR, RAR, GZ, BZ2, XZ) recursively:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  try
  {
    std::filesystem::path("test.zip") | content_type::detector{} | archives_parser{} | office_formats_parser{} | OCRParser{} | PlainTextExporter() | std::cout;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  return 0;
}
```

Classify file in any format (Office, PDF, mail, etc) to any categories using build-in local AI model:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("document_processing_market_trends.odt") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | local_ai::model_chain_element("Classify to one of the following categories and answer with exact category name: agreement, invoice, report, legal, user manual, other:\n\n") | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(out_stream.str() == "report");

  return 0;
}
```

Classify file in any format (Office, PDF, mail, etc) to any categories using OpenAI service:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("document_processing_market_trends.odt") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | openai::Classify({ "agreement", "invoice", "report", "legal", "other"}, std::getenv("OPENAI_API_KEY")) | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(out_stream.str() == "report\n");

  return 0;
}
```

Translate document in any format (Office, PDF, mail, etc) to other language using build-in local AI model:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | local_ai::model_chain_element("Translate to spanish:\n\n") | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(fuzzy_match::ratio(out_stream.str(), "La procesación de datos se refiere a las actividades realizadas en el ámbito de los datos en materia de información. Se trata de recoger, organizar, analizar y interpretar los datos para extraer inteligencias y apoyar el procesamiento de decisión. Esto puede incluir tareas como la etiqueta, la filtración, la summarización y la transformación de los datos a través de diversos métodos compuestos y estadounidenses. El procesamiento de datos es esencial en diversos ámbitos, incluyendo el negocio, la ciencia y la tecnologàa, pues permite a las empresas a extraer conocimientos valiosos de grans de datos, hacer decisiones indicadas y mejorar la eficiencia global.") > 80);
  return 0;
}
```

Translate document in any format (Office, PDF, mail, etc) to other language using OpenAI service:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | openai::TranslateTo("spanish", std::getenv("OPENAI_API_KEY")) | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(fuzzy_match::ratio(out_stream.str(), "El procesamiento de datos se refiere a las actividades realizadas sobre datos en bruto para convertirlos en información significativa. Implica la recopilación, organización, análisis e interpretación de datos para extraer conocimientos útiles y apoyar la toma de decisiones. Esto puede incluir tareas como clasificar, filtrar, resumir y transformar datos mediante diversos métodos computacionales y estadísticos. El procesamiento de datos es esencial en varios campos, incluyendo los negocios, la ciencia y la tecnología, ya que permite a las organizaciones derivar conocimientos valiosos de grandes conjuntos de datos, tomar decisiones informadas y mejorar la eficiencia general.\n") > 80);
  return 0;
}
```

Detect sentiment of document in any format (Office, PDF, mail, etc) using build-in local AI model:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | local_ai::model_chain_element("Detect sentiment:\n\n") | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(out_stream.str() == "positive");

  return 0;
}
```

Detect sentiment of document in any format (Office, PDF, mail, etc) using OpenAI service:

```cpp
#include "docwire.h"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;

  try
  {
    std::filesystem::path("1.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | openai::DetectSentiment(std::getenv("OPENAI_API_KEY")) | std::cout;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
```

Make a summary of document in any format (Office, PDF, mail, etc) using build-in local AI model:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | local_ai::model_chain_element("Write a short summary for this text:\n\n") | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(out_stream.str() == "Data processing is the process of transforming raw data into meaningful information.");

  return 0;
}
```

Make a voice summary of document in any format (Office, PDF, mail, etc) in two steps: summarize using GPT model and convert the summary to speech using text to speech model. Result is saved to mp3 file:

```cpp
#include "docwire.h"
#include <fstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  try
  {
    std::filesystem::path("1.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | openai::Summarize(std::getenv("OPENAI_API_KEY")) | openai::TextToSpeech(std::getenv("OPENAI_API_KEY")) | std::ofstream("summary.mp3");
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
```

Make a text summary of voice recording (e.g. mp3 file with meeting recording) in two steps: convert voice to text and summarize text using OpenAI services (with non-default model selected):

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.mp3") | openai::Transcribe(std::getenv("OPENAI_API_KEY")) | PlainTextExporter() | openai::Summarize(std::getenv("OPENAI_API_KEY"), openai::Model::gpt_4o) | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(fuzzy_match::ratio(out_stream.str(), "Data processing involves converting raw data into meaningful information by collecting, organizing, analyzing, and interpreting it. This process includes tasks like sorting, filtering, summarizing, and transforming data using computational and statistical methods. It is crucial in fields like business, science, and technology, as it helps organizations extract valuable insights from large datasets, make informed decisions, and enhance efficiency.\n") > 80);

  return 0;
}
```

Find phrases, objects and events with smart matching in documents in any format (Office, PDF, mail, etc) using build-in local AI model:

```cpp
#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | local_ai::model_chain_element("Find sentence about \"data convertion\" in the following text:\n\n") | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(out_stream.str() == "Data processing refers to the activities performed on raw data to convert it into meaningful information.");

  return 0;
}
```

Find phrases, objects and events in text or image using GPT model (with non-default model selected):

```cpp
#include "docwire.h"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("scene_1.png") | content_type::detector{} | openai::Find("tree", std::getenv("OPENAI_API_KEY"), openai::Model::gpt_4o) | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  assert(fuzzy_match::ratio(out_stream.str(), "2\n- A tree is located on the left side of the image near the people.\n- Another tree is in the background near the center of the image.\n") > 80);

  return 0;
}
```

Create embedding for document in any format (Office, PDF, mail, etc) using OpenAI service:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::vector<message_ptr> out_msgs;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | openai::embed(std::getenv("OPENAI_API_KEY")) | out_msgs;
    assert(out_msgs.size() == 1);
    assert(out_msgs[0]->is<ai::embedding>());
    auto embedding = out_msgs[0]->get<ai::embedding>();
    assert(embedding.values.size() == 1536);
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
```

Create embedding for document in any format (Office, PDF, mail, etc) using build-in local AI model, create embeddings for two queries and calculate similarity:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;

  try
  {
    // 1. Create an embedding for the document (passage) using the default prefix
    std::vector<message_ptr> passage_msgs;
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | local_ai::embed(local_ai::embed::e5_passage_prefix) | passage_msgs;
    assert(passage_msgs.size() == 1 && passage_msgs[0]->is<ai::embedding>());
    auto passage_embedding = passage_msgs[0]->get<ai::embedding>();
    assert(passage_embedding.values.size() == 384);
    // 2. Create an embedding for a similar query using the query prefix
    std::vector<message_ptr> similar_query_msgs;
    docwire::data_source{std::string{"What is data processing?"}, mime_type{"text/plain"}, confidence::highest} | local_ai::embed(local_ai::embed::e5_query_prefix) | similar_query_msgs;
    assert(similar_query_msgs.size() == 1 && similar_query_msgs[0]->is<ai::embedding>());
    auto similar_query_embedding = similar_query_msgs[0]->get<ai::embedding>();
    // 3. Create an embedding for a partially related query
    std::vector<message_ptr> partial_query_msgs;
    docwire::data_source{std::string{"How can data analysis improve business efficiency?"}, mime_type{"text/plain"}, confidence::highest} | local_ai::embed(local_ai::embed::e5_query_prefix) | partial_query_msgs;
    assert(partial_query_msgs.size() == 1 && partial_query_msgs[0]->is<ai::embedding>());
    auto partial_query_embedding = partial_query_msgs[0]->get<ai::embedding>();
    // 4. Create an embedding for a dissimilar query
    std::vector<message_ptr> dissimilar_query_msgs;
    docwire::data_source{std::string{"What is the best C++ IDE?"}, mime_type{"text/plain"}, confidence::highest} | local_ai::embed(local_ai::embed::e5_query_prefix) | dissimilar_query_msgs;
    assert(dissimilar_query_msgs.size() == 1 && dissimilar_query_msgs[0]->is<ai::embedding>());
    auto dissimilar_query_embedding = dissimilar_query_msgs[0]->get<ai::embedding>();
    // 5. Calculate and check similarities.
    double sim = cosine_similarity(passage_embedding.values, similar_query_embedding.values);
    std::cout << "Similarity (passage, similar_query): " << sim << std::endl;
    double partial_sim = cosine_similarity(passage_embedding.values, partial_query_embedding.values);
    std::cout << "Similarity (passage, partial_query): " << partial_sim << std::endl;
    double dissim = cosine_similarity(passage_embedding.values, dissimilar_query_embedding.values);
    std::cout << "Similarity (passage, dissimilar_query): " << dissim << std::endl;
    // Check that the scores are within expected ranges.
    assert(sim > 0.9 && sim < 1.0);
    assert(partial_sim > 0.8 && partial_sim < 0.9);
    assert(dissim > 0.7 && dissim < 0.8);
    // The most important check is the relative order of the scores.
    assert(sim > partial_sim && partial_sim > dissim);
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
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

  try
  {
    auto chain = content_type::detector{} | office_formats_parser{} | PlainTextExporter() | std::cout;  // create a chain of steps to parse a file
    for (int i = 1; i < 3; ++i)
      std::ifstream(std::to_string(i) + ".docx", std::ios_base::binary) | chain; // set the input file as an input stream
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
```

Handling errors and warnings:

[You can find example of handling errors and warnings here](https://docwire.readthedocs.io/en/latest/handling_errors_and_warnings_8cpp-example.html)

Using transformer to filter out emails (eg. from Outlook PST mailbox) with subject containing "Hello":

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::filesystem::path("1.pst") | content_type::detector{} |
  mail_parser{} | office_formats_parser{}
    | [](message_ptr msg, const message_callbacks& emit_message) // Create an input from file path, parser and connect them to transformer
      {
        if (msg->is<mail::Mail>()) // if current node is mail
        {
          auto subject = msg->get<mail::Mail>().subject; // get the subject attribute
          if (subject) // if subject attribute exists
          {
            if (subject->find("Hello") != std::string::npos) // if subject contains "Hello"
            {
              return continuation::skip; // skip the current node
            }
          }
        }
        return emit_message(std::move(msg));
      }
    | PlainTextExporter() // sets exporter to plain text
    | std::cout;
  return 0;
}
```

![Example flow](doc/images/example_flow.png)

Joining transformers to filter out emails (eg. from Outlook PST mailbox) with subject "Hello" and limit the number of mails to 3:

```cpp
#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::filesystem::path("1.pst") | content_type::detector{} |
  mail_parser{} | office_formats_parser{} |
    [](message_ptr msg, const message_callbacks& emit_message) // Create an input from file path, parser and connect them to transformer
    {
      if (msg->is<mail::Mail>()) // if current node is mail
      {
        auto subject = msg->get<mail::Mail>().subject; // get the subject attribute
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
      if (msg->is<mail::Mail>()) // if current node is mail
      {
        if (++counter > max_mails) // if counter is greater than max_mails
        {
          return continuation::stop; // cancel the parsing process
        }
      }
      return emit_message(std::move(msg));
    } |
    PlainTextExporter() | // sets exporter to plain text
    std::cout;
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

### Supported Platforms
 DocWire SDK is compatible with a variety of operating systems. Windows, Linux, and macOS are supported officially (Supported triplets are: x64-linux-dynamic, x64-windows, x64-osx-dynamic and arm64-osx-dynamic). but in theory it can be run on other operating systems as well. To ensure compatibility our continuous integration tests run on the following GitHub runners:

- [ubuntu-24.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2404-Readme.md)
- [ubuntu-22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)
- [windows-2025](https://github.com/actions/runner-images/blob/main/images/windows/Windows2025-Readme.md)
- [windows-2022](https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md)
- [macos-15](https://github.com/actions/runner-images/blob/main/images/macos/macos-15-Readme.md)
- [macos-14](https://github.com/actions/runner-images/blob/main/images/macos/macos-14-Readme.md)
- [macos-13](https://github.com/actions/runner-images/blob/main/images/macos/macos-13-Readme.md)

As the project evolves, we will continue to expand the list of officially supported platforms to ensure broad compatibility and meet the needs of our users.

### Required Tools
DocWire SDK installation process is based on the VCPKG package manager so all requirements of vcpkg apply:
- ["Prerequisites" on Microsoft Learn](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started#prerequisites)

In addition to vcpkg requirements, DocWire SDK requires the following tools:
- GCC 11 or higher on Linux because of C++20 support
- MSVC 2019 or higher on Windows because of C++20 support
- Doxygen with Graphviz is required for documentation generation: ["Installation" on Doxygen website](https://www.doxygen.nl/manual/install.html)
- Autoconf, Autogen, Automake, Autoconf-archive, Autopoint are required for building some of 3rdparty dependencies on Linux and MacOS (we are working to eliminate this)

### Installation via build.sh or build.ps1 scripts
The current preferred installation method is via build.sh or build.ps1 scripts. The script will automatically install vcpkg, select correct triplet, add required overlays and install DocWire SDK with all dependencies from sources.

1. **Clone the DocWire Repository:**
Clone the DocWire repository from GitHub if you haven't already:
```
git clone https://github.com/docwire/docwire.git
```

2. **Run build.sh (Linux, MacOS) or build.ps1 (Windows):**

- Linux, MacOS:
```
cd docwire
./build.sh
```

- Windows:
```
cd docwire
./build.ps1
```

**Important note**: Sometimes installation of dependencies may fail due to various reasons. Some of those reasons are not related to the DocWire SDK itself, but rather to the vcpkg package manager or the specific dependencies being installed. For example, errors may occur while downloading sources or if there are unexpected changes in vcpkg ports or upstream websites. In such cases, it is recommended to [report the issue to the vcpkg](https://github.com/microsoft/vcpkg/issues) or just try running the installation script again. In rare cases, some dependencies may require manual intervention or additional configuration before they can be successfully installed. If you encounter any issues during the installation process, please don't hesitate to reach out to the [DocWire SDK community for support](https://github.com/docwire/docwire/issues).

3. **Integrate with your project or development environment**

You can use vcpkg toolchain file to integrate DocWire SDK with your CMake project:
```
cmake -DCMAKE_TOOLCHAIN_FILE=docwire/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```
Please refer to ["vcpkg in CMake projects" on Microsoft Learn](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration) for more information on how to use vcpkg with your CMake project.

To use DocWire SDK in your MSBuild projects (Visual Studio) run the following command:
```
cd docwire/vcpkg
vcpkg integrate install
```
This automatically adds installed packages to the following project properties: Include Directories, Link Directories, and Link Libraries. Additionally, it creates a post-build action that ensures that any required DLLs are copied into the build output folder.

Please refer to ["vcpkg in MSBuild projects" on Microsoft Learn](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/msbuild-integration) for more information on how to use vcpkg with your MSBuild project.

For other building systems check your build system specific documentation for how to use prebuilt binaries.
Please refer to ["Manual Integration" on Microsoft Learn](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/manual-integration) for more information.

### Installation in preexisting vcpkg instance
You need to do the configuration, installation and integration manually. Please follow recommendations in vcpkg documentation and check content of build.sh or build.ps1 script for details.

Required overlays are located in "ports" subdirectory.

### Pre-built binaries (binary cache)
Vcpkg builds libraries from source but offers an option to store the results of the build process in a binary cache. This allows other developer machines or continuous integration runs to reference these prebuilt packages without running a new build every time. By using a binary cache, vcpkg can detect if a rebuild is necessary by checking if the cache already contains a valid existing package with appropriate binaries.

Please refer ["What is binary caching?" on Microsoft Learn](https://learn.microsoft.com/en-us/vcpkg/consume/binary-caching-overview) for more information.

We used to provide pre-built binaries but we realized that they are not reliable as they are tightly coupled with specific versions of compiler, operating system and other dependencies. To solve this issue, our continuous integration scripts use vcpkg binary caching with GitHub packages and we are working to offer this to other users. This will allow everyone to use the latest version of DocWire SDK without having to worry about compatibility.

### Conclusion
You're all set! You've successfully installed the DocWire library using vcpkg. You can now use the DocWire library in your code to perform text extraction and other data processing tasks. 

<a name="versioning"></a>
## Versioning

DocWire SDK introduces a distinctive versioning methodology, deviating from conventional semantic versioning (SemVer) in favor of a dynamic, date-centric system. This chapter elucidates the versioning strategy of DocWire SDK, shedding light on its unique attributes and the benefits derived from this innovative approach.

### "Release Early, Release Often" Strategy with Date-Based Versioning

DocWire SDK's versioning system adopts the "release early, release often" concept, ensuring that users benefit from the latest features and improvements as soon as they are available. This approach minimizes the usage of outdated code, ensuring that resources are directed towards delivering the best possible user experience.

DocWire SDK's versioning system integrates release dates as a pivotal element. Each release is identified by a specific date, providing users with a transparent timeline of updates and enhancements. In most cases new release is created immediately after merging of new feature or important bug fix to the main branch.

The "release early, release often" strategy is based on the idea that releasing code frequently allows for faster resolution of issues, faster feedback from users, and enables developers to incorporate community feedback more effectively. By releasing code more frequently, users benefit from the latest changes and features, enabling them to stay up-to-date with the SDK's evolution.

This approach is in line with industry best practices and aligns with the "live at the head" concept, providing users with a transparent timeline of updates and enhancements. This approach ensures clarity and enables users to comprehend the evolution of the SDK over time.

### Advantages of "Release Early, Release Often"

#### Continuous Evolution

The "release early, release often" strategy fosters a model of continuous evolution, allowing for swift development and deployment of new features. This ensures that users have timely access to the latest advancements and can choose to stay current or opt for specific versions tailored to their requirements.

#### Improved Collaboration

Transparent and chronological versioning facilitates collaboration among developers, contributors, and users. This inclusive model enables everyone to track the SDK's progress, understand the sequence of changes, and contribute to discussions around specific releases, fostering a sense of community and shared ownership.

#### Rapid Issue Resolution

The "release early, release often" concept accelerates issue resolution by providing prompt access to bug fixes and improvements. This agile approach ensures that users encounter fewer obstacles, leading to a more responsive and satisfying experience.

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

<a name="error-handling"></a>
## Error handling: robust and secure

The DocWire SDK provides a comprehensive error handling framework designed to handle errors in a clear and specific way. This framework offers several key features that facilitate error diagnosis and handling.

### Chained exceptions

The framework supports building an exception chain, allowing the creation of a longer error context across different layers of the backtrace. This enables tracking of the sequence of events leading up to an error, making it easier to identify the root cause.

In addition to std::nested_exception and std::exception_ptr and std::throw_with_nested, the framework also includes the [errors::make_nested](https://docwire.readthedocs.io/en/latest/namespacedocwire_1_1errors.html#afde521434d8ac75136d13bc9d42c17a2) and [errors::make_nested_ptr](https://docwire.readthedocs.io/en/latest/namespacedocwire_1_1errors.html#a7380b5b4f036f549a9c284261d9f31ac) function templates and [errors::nested](https://docwire.readthedocs.io/en/latest/classdocwire_1_1errors_1_1nested.html) class template for easy error chaining like: `docwire::errors::make_nested_ptr(error_object_1, error_object_2)`.

### Type-safe context values

Context values are type-safe, meaning any type, including custom types, can be used in the context value. The context is built from original C++ types, and context objects can be accessed during the error handling process.

[errors::impl](https://docwire.readthedocs.io/en/latest/structdocwire_1_1errors_1_1impl.html) structure template provides an implementation of the context value mechanism, including support for custom types: `throw errors::impl{custom_type_value}`.

[errors::base](https://docwire.readthedocs.io/en/latest/structdocwire_1_1errors_1_1base.html) structure provides a base class for all error types for easy error handling: `catch (docwire::errors::base& e) { ... }`.

### Embedded context variable names and triggering expressions

Error contexts include stringified context variable names and triggering C++ expressions, providing valuable information for diagnosing and handling errors via make_error and throw_if macros.

For example: `throw_if(x < 0 || y < 0, x, y)` gives, on failure, an exception chain with `std::make_pair("triggering_expression", "x < 0 || y < 0")`, `std::make_pair("x", x)` and `std::make_pair("y", y)`.

### Categorized and tagged errors

Errors can be tagged with a custom type or multiple types like [errors::network_failure](https://docwire.readthedocs.io/en/latest/structdocwire_1_1errors_1_1network__failure.html) or [errors::file_encrypted](https://docwire.readthedocs.io/en/latest/structdocwire_1_1errors_1_1file__encrypted.html), providing fine-grained control over error handling. Decisions, such as retrying an operation or asking for password, can be made based on the presence of a specific tag type in error chain: `catch (const docwire::errors::base& e) if (docwire::errors::contains_type<docwire::errors::network_failure>(e)) { retry(); }`.

### Embedded source location

Error objects include embedded source location information, providing essential context for debugging: `catch (const docwire::errors::base& e) { std::cerr << e.source_location.file_name() << ":" << e.source_location.line() << std::endl; }`.

### Secure error messages

To reduce the risk of security breaches, the framework avoids including sensitive information in formatted error messages. There is no implicit stringification of context values, and the standard what() method is secured to return only the exception type name. Sensitive information can be retrieved from the error object on-demand only: `std::cerr << e.content_type() << ": " << e.context_string()` or `if (docwire::errors::contains_type<std::filesystem::path>(e)) { auto fn = dynamic_cast<docwire::errors::impl<std::file_system::path>>(e).context; }`.

### Easy context data retrieval

Functions like [errors::diagnostic_message](https://docwire.readthedocs.io/en/latest/namespacedocwire_1_1errors.html#a2446f6c81b6a5b338dea00cb29c40263) and [errors::contains_type](https://docwire.readthedocs.io/en/latest/namespacedocwire_1_1errors.html#aa2af5b81a3e66772f19506d8e8a93184) allow access to context data in a controlled manner, facilitating effective error handling.

For example `std::cerr << docwire::errors::diagnostic_message(e) << std::endl;` can give the following results:
```
[ERROR] Error "file encrypted error tag"
in void docwire::XLSParser::Implementation::processRecord(int, const std::vector<unsigned char>&, std::string&)
at /docwire/src/xls_parser.cpp:483
with context "RC4 encryption"
in void docwire::XLSParser::Implementation::processRecord(int, const std::vector<unsigned char>&, std::string&)
at /docwire/src/xls_parser.cpp:483
with context "Error parsing XLS document"
in std::string docwire::XLSParser::parse(docwire::ThreadSafeOLEStorage&) const
at /docwire/src/xls_parser.cpp:931
processing file tests/password_protected.xls
```

### Non-fatal errors and warnings

Errors that are not fatal are represented with the same chained error objects, but instead of using C++ throw/catch mechanism, they are pushed to the parsing chain or returned via callbacks like other results.

This assumption gives them similar security and debugging capabilities to fatal errors and allows easy conversion between fatal and non-fatal errors on different backtrace levels (it usually cannot be decided in a point of failure) without breaking of the error chain.

[You can find example of handling non-fatal errors here](https://docwire.readthedocs.io/en/latest/handling_errors_and_warnings_8cpp-example.html)

### Modern C++ features used

The framework is designed to take advantage of modern C++ features, such as std::nested_exception and std::exception_ptr, to build an exception chain instead of adding multiple values to a single exception object. This approach ensures alignment with the latest C++ standards and best practices. Although the design shares similarities with Boost Exception, the DocWire SDK's error handling framework is more closely aligned with modern C++ standards.

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

### Local AI Integration

Process data securely using offline AI models with the following options:

- **&ndash;&ndash;local-ai-prompt <prompt>**: prompt to process text via local AI model
- **&ndash;&ndash;local-ai-embed <prefix>**: generate embedding of text via local AI model. Optional argument is a prefix (e.g. "passage: " or "query: ")
- **&ndash;&ndash;local-ai-model <path>**: path to local AI model data (build-in default model is used if not specified)

### OpenAI Integration

Unlock the power of OpenAI with the following options:

- **&ndash;&ndash;openai-chat <prompt>**: Initiate a chat prompt for processing text and images via OpenAI.
- **&ndash;&ndash;openai-extract-entities**: Extract entities from text and images via OpenAI.
- **&ndash;&ndash;openai-extract-keywords <N>**: Extract N keywords/key phrases from text and images via OpenAI.
- **&ndash;&ndash;openai-summarize**: Summarize text and images via OpenAI.
- **&ndash;&ndash;openai-detect-sentiment**: Detect sentiment of text and images via OpenAI.
- **&ndash;&ndash;openai-analyze-data**: Analyze text and images for important insights and generate conclusions via OpenAI.
- **&ndash;&ndash;openai-classify <category>**: Classify text and images via OpenAI to one of the specified categories.
- **&ndash;&ndash;openai-translate-to <language>**: Language to translate text and images to via OpenAI.
- **&ndash;&ndash;openai-find <what>**: Find specified phrase, object or event in text and images via OpenAI.
- **&ndash;&ndash;openai-text-to-speech**: Convert text to speech via OpenAI
- **&ndash;&ndash;openai-embed**: Generate embedding of text via OpenAI
- **&ndash;&ndash;openai-transcribe**: Convert speech to text (transcribe) via OpenAI
- **&ndash;&ndash;openai-key <key>**: OpenAI API key.
- **&ndash;&ndash;openai-model <model>** (default: gpt_5): Choose the OpenAI model. Available models are: gpt_5, gpt_5_mini, gpt_5_nano, gpt_5_chat_latest, gpt_41, gpt_41_mini, gpt_41_nano, gpt_4o, gpt_4o_mini, o3, o3_pro, o3_deep_research, o3_mini, o4_mini, o4_mini_deep_research.
- **&ndash;&ndash;openai-temperature <temp>**: Force specified temperature for OpenAI prompts.
- **&ndash;&ndash;openai-image-detail <detail>**: Force specified image detail parameter for OpenAI image prompts. Available options are: low, high and automatic.
- **&ndash;&ndash;openai-tts-model <model>** (default: gpt_4o_mini_tts): Choose the TTS model. Available models are: gpt_4o_mini_tts, tts_1, tts_1_hd.
- **&ndash;&ndash;openai-embed-model <model>** (default: text_embedding_3_small): Choose the embedding model. Available models are: text_embedding_3_small, text_embedding_3_large, text_embedding_ada_002.
- **&ndash;&ndash;openai-transcribe-model <model>** (default: gpt_4o_transcribe): Choose the transcribe model. Available models are: gpt_4o_transcribe, gpt_4o_mini_transcribe, whisper_1.
- **&ndash;&ndash;openai-voice <voice>** (default: alloy): Choose voice for text to speech conversion. Available voices are: alloy, echo, fable, onyx, nova, shimmer.

### Additional Options

- **&ndash;&ndash;language <lang> (default: eng)**: Set the document language(s) for OCR as ISO 639-3 identifiers like: spa, fra, deu, rus, chi_sim, chi_tra etc. More than 100 languages are supported. Multiple languages can be enabled.
- **&ndash;&ndash;ocr-confidence-threshold <value> (default: 75)**: Set the OCR confidence threshold (0-100). Words with confidence below this will be excluded.
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

Note: The "min_creation_time" and "max_creation_time" options currently work only for emails within PST/OST files.

### Example Usage

#### Extracting Structured Content in HTML Format

To extract structured content in HTML format, use the following command:

```bash
docwire --output_type html document.docx
```

#### Secure offline AI document analysis

```bash
docwire --local-ai-prompt "What is the conclusion of the following document?" data_processing_definition.doc
```

### Secure offline AI embedding generation

```bash
docwire --local-ai-embed "passage: " data_processing_definition.doc
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

#### Searching for objects or events in image or photo

Find objects or events in image using GPT model:

```bash
docwire --openai-find car --local-processing=off image.jpg
docwire --openai-find person --local-processing=off image.jpg
docwire --openai-find running --local-processing=off image.jpg
```

#### Embeddings generation for advanced AI applications

Generate embedding of document using OpenAI service for advanced AI applications like Retrieval Augmented Generation (RAG), semantic search, and document clustering.

```bash
docwire --openai-embed data_processing_definition.doc
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
