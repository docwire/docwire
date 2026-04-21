
#ifndef DOCWIRE_LOCAL_AI_SUMMARIZE_H
#define DOCWIRE_LOCAL_AI_SUMMARIZE_H

#include "local_ai_export.h"
#include "ai_summarize.h"

namespace docwire::ai::local
{

class DOCWIRE_LOCAL_AI_EXPORT summarize : public docwire::ai::summarize
{
public:
    summarize();
    explicit summarize(std::shared_ptr<docwire::ai::ai_runner> runner);
};

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_SUMMARIZE_H
