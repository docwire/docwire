/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/


#ifndef DOCWIRE_PARSER_H
#define DOCWIRE_PARSER_H

#include <any>
#include <string>
#include <functional>
#include <memory>

#include "formatting_style.h"
#include "parser_manager.h"
#include "parser_parameters.h"
#include "defines.h"

namespace docwire
{

/**
 * @brief Contains set of basic tags using in parsers.
 */
class StandardTag
{
public:
  inline static const std::string TAG_P = "p"; ///< Tag for paragraph.
  inline static const std::string TAG_CLOSE_P = "/p"; ///< Tag for closing paragraph.
  inline static const std::string TAG_SECTION = "div"; ///< Tag for document section.
  inline static const std::string TAG_CLOSE_SECTION = "/div"; ///< Tag for closing document section.
  inline static const std::string TAG_SPAN = "span"; ///< Tag for text span.
  inline static const std::string TAG_CLOSE_SPAN = "/span"; ///< Tag for closing text span.
  inline static const std::string TAG_BR = "br/"; ///< Tag for line break.
  inline static const std::string TAG_B = "b"; ///< Tag for bold.
  inline static const std::string TAG_CLOSE_B = "/b"; ///< Tag for closing bold.
  inline static const std::string TAG_I = "i"; ///< Tag for italic.
  inline static const std::string TAG_CLOSE_I = "/i"; ///< Tag for closing italic.
  inline static const std::string TAG_U = "u"; ///< Tag for underline.
  inline static const std::string TAG_CLOSE_U = "/u"; ///< Tag for closing underline.
  inline static const std::string TAG_TABLE = "table"; ///< Tag for table.
  inline static const std::string TAG_CLOSE_TABLE = "/table"; ///< Tag for closing table.
  inline static const std::string TAG_TR = "tr"; ///< Tag for table row.
  inline static const std::string TAG_CLOSE_TR = "/tr"; ///< Tag for closing table row.
  inline static const std::string TAG_TD = "td"; ///< Tag for table cell.
  inline static const std::string TAG_CLOSE_TD = "/td"; ///< Tag for closing table cell.
  inline static const std::string TAG_TEXT = "text/"; ///< Tag for text.
  inline static const std::string TAG_LINK = "a"; ///< Tag for link. Attributes: "url": std::string
  inline static const std::string TAG_CLOSE_LINK = "/a"; ///< Tag for link.
  inline static const std::string TAG_IMAGE = "img/"; ///< Tag for image. Attributes: "src": std::string, "alt": std::string
  inline static const std::string TAG_STYLE = "style/"; ///< Tag for style. Attributes: "css_text": std::string

  inline static const std::string TAG_LIST = "list"; ///< Tag for list. Attributes: "type": std::string (decimal | disc | none | custom string), "is_ordered": bool (def. is false, DEPRECATED), "list_style_prefix": std::string (DEPRECATED)
  inline static const std::string TAG_CLOSE_LIST = "/list"; ///< Tag for closing list
  inline static const std::string TAG_LIST_ITEM = "list-item"; ///< Tag for list item
  inline static const std::string TAG_CLOSE_LIST_ITEM = "/list-item"; ///< Tag for closing list item

  inline static const std::string TAG_MAIL = "mail"; ///< Tag for mail. Attributes: "subject": std::string, "date": uint (unix timestamp).
  inline static const std::string TAG_CLOSE_MAIL = "/mail"; ///< Tag for closing mail.
  inline static const std::string TAG_MAIL_BODY = "mail-body"; ///< Tag for mail body.
  inline static const std::string TAG_CLOSE_MAIL_BODY = "/mail-body"; ///< Tag for closing mail body.
  inline static const std::string TAG_ATTACHMENT = "attachment"; ///< Tag for attachment. If you set skip in this tag, then the attachment won't be parsed. Attributes: "name": std::string, "size": uint, "extension": std::string
  inline static const std::string TAG_CLOSE_ATTACHMENT = "/attachment"; ///< Tag for closing attachment.
  inline static const std::string TAG_FOLDER = "folder"; ///< Tag for folder. If you set skip in this tag, then the folder won't be parsed. Attributes: "name": std::string.
  inline static const std::string TAG_CLOSE_FOLDER = "/folder"; ///< Tag for closing folder.

  inline static const std::string TAG_METADATA = "metadata/"; ///< Tag for metadata.
  inline static const std::string TAG_COMMENT = "comment/"; ///< Tag for comments. Attributes: "author": std::string, "time": std::string (format:(yyyy-mm-ddThh:mm:ss)), "comment": std::string

  inline static const std::string TAG_PAGE = "new-page"; ///< Tag for page. This tag is sent before parsing the page, so if we set in this tag, then the page won't be parsed.
  inline static const std::string TAG_CLOSE_PAGE = "/new-page"; ///< Tag for closing page.

  inline static const std::string TAG_FILE = "new-file"; ///< Tag for new file.
  inline static const std::string TAG_CLOSE_FILE = "/new-file"; ///< Tag for close file.
  inline static const std::string TAG_DOCUMENT = "new-document"; ///< Tag for new file.
  inline static const std::string TAG_CLOSE_DOCUMENT = "/new-document"; ///< Tag for new file.
};

typedef std::map<std::string, std::any> Attributes;

struct DllExport Info
{
  std::string tag_name; //!< tag name
  Attributes attributes; //!< tag attributes
  bool cancel = false; //!< cancel flag. If set true then parsing process will be stopped.
  bool skip = false; //!< skip flag. If set true then tag will be skipped.
  std::string plain_text; //!< Stores text from last parsed node.

  explicit Info(const std::string &tagName = "", const std::string &plainText = "", const std::map<std::string, std::any> &attrs = {})
  : tag_name(tagName),
    plain_text(plainText),
    attributes(attrs)
  {}

  template<typename T>
  std::optional<T> getAttributeValue(const std::string &name) const
  {
    auto attribute_value = attributes.find(name);
    if (attribute_value!= attributes.end() && attribute_value->second.type() == typeid(T))
    {
      return std::any_cast<T>(attribute_value->second);
    }
    return std::nullopt;
  }
};

typedef std::function<void(Info &info)> NewNodeCallback;

/**
 * @brief Abstract class for all parsers
 */
class DllExport Parser
{
public:
  /**
   *
   * @param inParserManager parser manager contains all available parsers which could be used recursive
   */
  explicit Parser(const std::shared_ptr<ParserManager> &inParserManager = nullptr);

  virtual ~Parser() = default;

  /**
   * @brief Executes text parsing
   */
  virtual void parse() const = 0;
  /**
   * @brief Adds new function to execute when new node will be created. Node is a part of parsed text.
   * Depends on the kind of parser it could be. For example, email from pst file or page from pdf file.
   * @param callback function to execute
   * @return reference to self
   */
  virtual Parser &addOnNewNodeCallback(NewNodeCallback callback);

  virtual Parser &withParameters(const ParserParameters &parameters);

protected:
  /**
  * @brief Loads FormattingStyle from ParserParameters.
  * @return Loaded FormattingStyle if exists, otherwise defualt FormattingStyle .
  **/
  FormattingStyle getFormattingStyle() const;

  Info sendTag(const std::string& tag_name, const std::string& text = "", const std::map<std::string, std::any> &attributes = {}) const;
  Info sendTag(const std::string& tag_name, const std::map<std::string, std::any> &attributes) const
  {
	  return sendTag(tag_name, "", attributes);
  }
  Info sendTag(const Info &info) const;

  std::shared_ptr<ParserManager> m_parser_manager;
  ParserParameters m_parameters;

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> base_impl;
};

} // namespace docwire
#endif //DOCWIRE_PARSER_H
