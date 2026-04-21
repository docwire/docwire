
#ifndef DOCWIRE_LOCAL_AI_TASK_H
#define DOCWIRE_LOCAL_AI_TASK_H

#include "local_ai_export.h"
#include "ai_misc_task.h"

namespace docwire::ai::local
{

class DOCWIRE_LOCAL_AI_EXPORT task : public docwire::ai::task
{
public:
    explicit task(const std::string& prompt);
    explicit task(const std::string& prompt, std::shared_ptr<docwire::ai::ai_runner> runner);
};

} // namespace docwire::ai::local

#endif // DOCWIRE_LOCAL_AI_TASK_H
