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

#include "detect_by_signature.h"

#include <magic.h>
#include "resource_path.h"

namespace docwire::detect
{

void by_signature(data_source& data)
{
    magic_t magic_cookie = magic_open(MAGIC_MIME_TYPE);
    throw_if (magic_cookie == NULL);
    throw_if (magic_load(magic_cookie, resource_path("libmagic/misc/magic.mgc").string().c_str()) != 0, magic_error(magic_cookie));
    std::span<const std::byte> span = data.span();
    const char *file_type = magic_buffer(magic_cookie, span.data(), span.size());
    throw_if (file_type == NULL, magic_error(magic_cookie));
    data.content_type = mime_type { file_type };
    magic_close(magic_cookie);
}

} // namespace docwire::detect
