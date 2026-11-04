/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CSV_EXPORTER_H
#define DF0_CSV_EXPORTER_H

#include "pipeline/element_base.hpp"
#include "csv_writer.h"
#include "data_source.h"
#include "document_elements.h"
#include <memory>
#include <sstream>

namespace df0
{

/**
 * @brief Exports data to CSV format.
 */
class csv_exporter : public pipeline::transformer_element<csv_exporter>
{
public:
    csv_exporter() = default;

    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
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

private:
    std::shared_ptr<std::stringstream> m_stream;
    csv_writer m_writer;
};

} // namespace df0

#endif //DF0_CSV_EXPORTER_H
