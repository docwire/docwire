#ifndef DOCWIRE_AUDIT_SINK_OSTREAM_HPP
#define DOCWIRE_AUDIT_SINK_OSTREAM_HPP

#include "../../json_serialization.h"
#include "../../ref_or_owned.h"
#include "../../serialization_base.h"
#include "../../source_location.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <utility>

namespace docwire::audit::sink
{

class ostream
{
public:
    explicit ostream(ref_or_owned<std::ostream> stream)
        : m_state(std::make_shared<state>(std::move(stream)))
    {
    }

    void record(source_location, const serialization::object& record_object)
    {
        std::string json_output = serialization::to_json(record_object);

        std::lock_guard lock(m_state->m_mutex);
        if (m_state->m_first_log)
        {
            m_state->m_stream.get() << "[" << std::endl;
            m_state->m_first_log = false;
        }
        else
        {
            m_state->m_stream.get() << "," << std::endl;
        }

        m_state->m_stream.get() << json_output;
    }

private:
    struct state
    {
        explicit state(ref_or_owned<std::ostream> stream)
            : m_stream(std::move(stream))
        {
        }

        ~state()
        {
            if (!m_first_log)
            {
                std::lock_guard lock(m_mutex);
                m_stream.get() << std::endl << "]" << std::endl;
            }
        }

        ref_or_owned<std::ostream> m_stream;
        bool m_first_log{true};
        std::mutex m_mutex;
    };

    std::shared_ptr<state> m_state;
};

} // namespace docwire::audit::sink

#endif
