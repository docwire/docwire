#include "local_ai_misc_task.h"
#include "ct2_runner.h"
#include "resource_path.h"

namespace docwire::ai::local
{

task::task(const std::string& prompt)
    : docwire::ai::task(prompt, std::make_shared<docwire::ai::ct2::ct2_runner>(
                                    resource_path("flan-t5-large-ct2-int8")))
{
}

task::task(const std::string& prompt, std::shared_ptr<docwire::ai::ai_runner> runner)
    : docwire::ai::task(prompt, runner)
{
}

} // namespace docwire::ai::local
