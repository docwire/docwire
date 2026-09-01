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

#ifndef DOCWIRE_CSV_EXPORTER_H
#define DOCWIRE_CSV_EXPORTER_H

#include "chain_element.h"
#include "csv_writer.h"
#include "data_source.h"
#include "document_elements.h"
#include <memory>
#include <sstream>

namespace docwire
{

/**
 * @brief Exports data to CSV format.
 */
class csv_exporter : public chain_element
{
public:
    csv_exporter() = default;

    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override
    {
        if (msg->is<std::exception_ptr>())
            return emit_message(std::move(msg));
        if (msg->is<document::document>() || !m_stream)
            m_stream = std::make_shared<std::stringstream>();
        m_writer.write_to(msg, *m_stream);
        if (msg->is<document::close_document>())
        {
            emit_message(data_source{seekable_stream_ptr{m_stream}});
            m_stream.reset();
        }
        return continuation::proceed;
    }

    bool is_leaf() const override
    {
        return false;
    }

private:
    std::shared_ptr<std::stringstream> m_stream;
    csv_writer m_writer;
};

} // namespace docwire

#endif //DOCWIRE_CSV_EXPORTER_H
