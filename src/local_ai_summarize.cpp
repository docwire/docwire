#include "ct2_runner.h"
#include "resource_path.h"
#include "local_ai_summarize.h"

namespace docwire::ai::local
{

summarize::summarize()
    : docwire::ai::summarize(
        std::make_shared<docwire::ai::ct2::ct2_runner>(resource_path("flan-t5-large-ct2-int8")))
{}

summarize::summarize(std::shared_ptr<docwire::ai::ai_runner> runner)
    : docwire::ai::summarize(runner)
{}

}
