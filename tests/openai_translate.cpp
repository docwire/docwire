/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace df0;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | openai::translate_to("spanish", environment::get("OPENAI_API_KEY").value()) | out_stream;
    ensure(fuzzy_match::ratio(out_stream.str(), "El procesamiento de datos se refiere a las actividades realizadas sobre datos en bruto para convertirlos en información significativa. Implica la recopilación, organización, análisis e interpretación de datos para extraer conocimientos útiles y apoyar la toma de decisiones. Esto puede incluir tareas como clasificar, filtrar, resumir y transformar datos mediante diversos métodos computacionales y estadísticos. El procesamiento de datos es esencial en varios campos, incluyendo los negocios, la ciencia y la tecnología, ya que permite a las organizaciones derivar conocimientos valiosos de grandes conjuntos de datos, tomar decisiones informadas y mejorar la eficiencia general.\n")) > 80;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  return 0;
}
