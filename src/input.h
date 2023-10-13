/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  https://github.com/docwire/docwire                                                                                                             */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#ifndef DOCTOTEXT_INPUT_H
#define DOCTOTEXT_INPUT_H

#include <iostream>
#include "chain_element.h"
#include "parsing_chain.h"
#include <filesystem>

namespace docwire
{

class DllExport InputBase
{
public:
  explicit InputBase(std::istream* stream)
  : m_stream(stream)
  {}

  explicit InputBase(const std::string &path)
  : m_path(path),
    m_stream(nullptr)
  {}

  ParsingChain operator|(ChainElement &chainElement) const;

  ParsingChain operator|(ChainElement &&chainElement) const;

  ParsingChain operator|(ParsingChain &parsingChain);

  ParsingChain operator|(ParsingChain &&parsingChain);

  void process(ChainElement& chain_element) const;

private:
  std::istream* m_stream;
  std::string m_path;
};

template<class StreamClass> class StreamInput : public InputBase
{
  static_assert(std::is_base_of<std::istream, StreamClass>::value, "StreamClass must inherit from std::istream");
  public:
    explicit StreamInput(StreamClass&& stream)
      : m_s(std::move(stream)), InputBase(&m_s)
  {}

  private:
    StreamClass m_s;
};

template<class T> class OtherInput : public InputBase
{
  //static_assert(std::is_base_of<std::istream, StreamClass>::value, "StreamClass must inherit from std::istream");
  public:
    explicit OtherInput(const T& v)
      : InputBase(v)
  {}
};


template<class T>
using InputV=typename std::conditional<std::is_base_of_v<std::istream, T>,StreamInput<T>,/*InputBase*/OtherInput<T>>::type;

template<class T>
struct Input : InputV<T>
{
  using InputV<T>::InputV;
};

template<class T> Input(T) -> Input<T>;

}
#endif //DOCTOTEXT_INPUT_H
