#include "docwire.h"
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  using namespace docwire;
  try {
    std::stringstream out_stream;
    docwire::ai::model_inference_config config;
    config.model_path = docwire::ai::llama::granite_model_path();
    config.max_tokens = docwire::ai::token_limit{256};
    config.n_ctx = docwire::ai::context_size{4096};
    config.n_threads = docwire::ai::thread_count{4};
    config.temp = docwire::ai::temperature{0.2f};
    config.min_probability = docwire::ai::min_p{0.05f};

    std::filesystem::path("data_processing_definition.doc") |
        content_type::detector{} | office_formats_parser{} |
        plain_text_exporter() | ai::local::llama::translate("spanish", config) |
        out_stream;
    ensure(fuzzy_match::ratio(
        out_stream.str(),
        "La procesación de datos se refiere a las actividades realizadas en el "
        "ámbito de los datos en materia de información. Se trata de recoger, "
        "organizar, analizar y interpretar los datos para extraer "
        "inteligencias y apoyar el procesamiento de decisión. Esto puede "
        "incluir tareas como la etiqueta, la filtración, la summarización y la "
        "transformación de los datos a través de diversos métodos compuestos y "
        "estadounidenses. El procesamiento de datos es esencial en diversos "
        "ámbitos, incluyendo el negocio, la ciencia y la tecnologàa, pues "
        "permite a las empresas a extraer conocimientos valiosos de grans de "
        "datos, hacer decisiones indicadas y mejorar la eficiencia global.")) >
        50;
    std::cout << "Result: " + out_stream.str();

  } catch (const std::exception &e) {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
