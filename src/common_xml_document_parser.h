/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_COMMON_XML_PARSER_H
#define DOCWIRE_COMMON_XML_PARSER_H

#include "attributes.h"
#include "chain_element.h"
#include "pimpl.h"
#include "xml_children.h"
#include <string>
#include <vector>
#include <map>

namespace docwire
{
	class zip_reader;
	class Metadata;

enum xml_parse_mode { PARSE_XML, FIX_XML, STRIP_XML };

/**
 * @brief Base class for XML-based document parsers (ODF, OOXML, etc.).
 * 
 * This class is inherited by specific parsers (e.g., odf_ooxml_parser, odfxml_parser).
 * It allows registering handlers for specific XML tags.
 * 
 * @tparam safety_level The safety policy used for XML parsing operations.
 * @sa xml::reader
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level = default_safety_level>
class common_xml_document_parser: public chain_element, public with_pimpl<common_xml_document_parser<safety_level>>
{
	private:
		friend pimpl_impl<common_xml_document_parser<safety_level>>;
		using with_pimpl<common_xml_document_parser<safety_level>>::impl;

	//public interface for derived classes (and its components)
	public:
		/// Enum for list styles (e.g., numbered or bulleted).
		enum odfooxml_list_style
		{
			number,
			bullet
		};

		/// Represents a comment with author, time, and text.
		struct comment
		{
			std::string m_author;
			std::string m_time;
			std::string m_text;
			comment() {}
			/**
			 * @brief Constructs a Comment.
			 * @param author The author of the comment.
			 * @param time The timestamp of the comment.
			 * @param text The content of the comment.
			 */
			comment(const std::string& author, const std::string& time, const std::string& text)
				: m_author(author), m_time(time), m_text(text) {}
		};

		/// Represents a relationship, typically for hyperlinks or embedded objects.
		struct relationship
		{
			std::string m_target;
		};

		/// Represents a shared string, a common optimization in OOXML formats.
		struct shared_string
		{
			std::string m_text;
		};

		/// Type alias for a vector of list styles.
		typedef std::vector<odfooxml_list_style> ListStyleVector;
		/// Type alias for a map of list style names to their definitions.
		using ListStyleMap = std::map<std::string, common_xml_document_parser<safety_level>::ListStyleVector>;
		/// Type alias for a map of comment IDs to Comment objects.
		using CommentMap = std::map<int, common_xml_document_parser<safety_level>::comment>;
		/// Type alias for a map of relationship IDs to Relationship objects.
		using RelationshipMap = std::map<std::string, common_xml_document_parser<safety_level>::relationship>;
		/// Type alias for a vector of shared strings.
		using SharedStringVector = std::vector<shared_string>;

		/**
		 * @brief Defines the function signature for an XML tag command handler.
		 */
		typedef std::function<void(xml::node_ref<safety_level>& xml_node, xml_parse_mode mode,
                                 zip_reader* zipfile, std::string& text,
                                 bool& children_processed, std::string& level_suffix, bool first_on_level)> CommandHandler;

		/**
		 * @brief Registers a handler for a specific XML tag.
		 * 
		 * Derived classes can use this to add or override behavior for specific XML tags.
		 * 
		 * @param xml_tag The XML tag name to handle.
		 * @param handler The function to execute when the tag is encountered.
		 */
		void registerODFOOXMLCommandHandler(const std::string& xml_tag, const CommandHandler& handler);

		/**
		 * @brief Parses XML data from a view of nodes.
		 * 
		 * Iterates through the provided XML nodes and executes registered command handlers.
		 * 
		 * @param xml_nodes The view of XML nodes to parse.
		 * @param mode The parsing mode (e.g., PARSE_XML, STRIP_XML).
		 * @param zipfile Pointer to the zip_reader if the XML is part of a zipped archive (e.g., DOCX, ODT).
		 * @return The extracted text content.
		 */
		std::string parseXmlData(xml::children_view<safety_level> xml_nodes, xml_parse_mode mode, zip_reader* zipfile);

		/**
		 * @brief Parses the children of a given XML node.
		 * 
		 * @param xml_node The parent node whose children will be parsed.
		 * @param mode The parsing mode.
		 * @param zipfile Pointer to the zip_reader if applicable.
		 * @return The extracted text content from the children.
		 */
		std::string parseXmlChildren(xml::node_ref<safety_level>& xml_node, xml_parse_mode mode, zip_reader* zipfile);

		/**
		 * @brief Extracts text from raw XML content.
		 * 
		 * This is a high-level function that initializes the XML reader and calls parseXmlData.
		 * 
		 * @param xml_contents The raw XML string.
		 * @param mode The parsing mode.
		 * @param zipfile Pointer to the zip_reader if applicable.
		 * @param text Output parameter where the extracted text will be appended.
		 */
		void extractText(std::string_view xml_contents, xml_parse_mode mode, zip_reader* zipfile, std::string& text);

		/**
		 * @brief Parses ODF metadata from XML content.
		 * @param xml_content The raw XML content of the metadata file.
		 * @param metadata The structure to populate with parsed metadata.
		 */
		void parseODFMetadata(std::string_view xml_content, attributes::metadata& metadata) const;

		/**
		 * @brief Formats a comment for output.
		 * @param author The author of the comment.
		 * @param time The timestamp of the comment.
		 * @param text The content of the comment.
		 * @return The formatted comment string.
		 */
		const std::string formatComment(const std::string& author, const std::string& time, const std::string& text);

		/// Returns the current nesting depth of lists.
		size_t& getListDepth();

		/// Gets the map of list styles.
		ListStyleMap& getListStyles();

		/// Gets the map of comments.
		CommentMap& getComments();

		/// Gets the map of relationships.
		RelationshipMap& getRelationships();

		/// Gets the vector of shared strings.
		SharedStringVector& getSharedStrings();

		/// Checks if text extraction is currently disabled.
		bool disabledText() const;

		/// Gets the current blank node handling policy.
		xml::reader_blanks blanks() const;

		/// Enables or disables text extraction.
		void disableText(bool disable);

		/// Sets the blank node handling policy for the XML reader.
		void set_blanks(xml::reader_blanks blanks);

		/// Controls whether signal emission (callbacks) is active.
		void activeEmittingSignals(bool flag);

	//public interface
	public:
		/**
		 * @brief Default constructor.
		 */
		common_xml_document_parser();

	protected:
		/**
		 * @brief Helper class to manage the context stack scope.
		 * Pushes a new context on construction and pops it on destruction.
		 */
		class scoped_context_stack_push
		{
		public:
			/**
			 * @brief Constructs the helper and pushes a new context onto the parser's stack.
			 * @param parser The parser instance.
			 * @param emit_message The message callbacks for the new context.
			 */
			scoped_context_stack_push(common_xml_document_parser& parser, const message_callbacks& emit_message);
			/// Destructor that pops the context from the parser's stack.
			~scoped_context_stack_push();
		private:
			common_xml_document_parser& m_parser;
		};
};

} // namespace docwire

#endif
