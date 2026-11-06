/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_asp.h"

namespace df0::content_type::asp
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

} // namespace df0::content_type::asp
