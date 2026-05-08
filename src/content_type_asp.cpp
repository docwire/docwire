/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "content_type_asp.h"

namespace docwire::content_type::asp
{

void detect(data_source& data)
{
    if (!data.mime_types.empty() && data.mime_type_confidence(mime_type { "text/html" }) < confidence::medium)
      return;
    std::string_view initial_content = data.string_view(length_limit{2048});
    if (initial_content.find("<%@ Page") != std::string_view::npos || // very common ASP.NET directives
        initial_content.find("<%@page") != std::string_view::npos || // case-insensitive common practice
        initial_content.find("<%@ Control") != std::string_view::npos || // very common ASP.NET directives
        initial_content.find("<%@control") != std::string_view::npos || // case-insensitive common practice
        initial_content.find("runat=\"server\"") != std::string_view::npos || // Common in ASP.NET server controls
        initial_content.find("<script language=\"C#\" runat=\"server\">") != std::string_view::npos || // C# script block
        initial_content.find("<script language=\"VB\" runat=\"server\">") != std::string_view::npos || // VB.NET script block
        initial_content.find("<%#") != std::string_view::npos) // ASP.NET data-binding expression
    {
        data.add_mime_type(mime_type{"text/aspdotnet"}, confidence::highest);
    }
    else if (initial_content.find("<%") != std::string_view::npos || // Generic ASP/PHP/JSP open tag, but common in classic ASP
        initial_content.find("<%=") != std::string_view::npos || // ASP expression
        initial_content.find("<!-- #include") != std::string_view::npos) // Server-Side Include (SSI), often used with ASP
    {
        data.add_mime_type(mime_type{"text/asp"}, confidence::highest);
    }
}

} // namespace docwire::content_type::asp
