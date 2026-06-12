#include "docwire.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

int main(int argc, char* argv[])
{
  using namespace docwire;

  try
  {
    // 1. Create an embedding for the document (passage) using the default prefix
    std::vector<message_ptr> passage_msgs;
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | local_ai::embed(local_ai::embed::e5_passage_prefix) | passage_msgs;
    ensure(passage_msgs.size()) == 1;
    ensure(passage_msgs[0]->is<ai::embedding>()) == true;
    auto passage_embedding = passage_msgs[0]->get<ai::embedding>();
    ensure(passage_embedding.values.size()) == 384;

    // 2. Create an embedding for a similar query using the query prefix
    std::vector<message_ptr> similar_query_msgs;
    docwire::data_source{std::string{"What is data processing?"}, mime_type{"text/plain"}, confidence::highest} | local_ai::embed(local_ai::embed::e5_query_prefix) | similar_query_msgs;
    ensure(similar_query_msgs.size()) == 1;
    ensure(similar_query_msgs[0]->is<ai::embedding>()) == true;
    auto similar_query_embedding = similar_query_msgs[0]->get<ai::embedding>();

    // 3. Create an embedding for a partially related query
    std::vector<message_ptr> partial_query_msgs;
    docwire::data_source{std::string{"How can data analysis improve business efficiency?"}, mime_type{"text/plain"}, confidence::highest} | local_ai::embed(local_ai::embed::e5_query_prefix) | partial_query_msgs;
    ensure(partial_query_msgs.size()) == 1;
    ensure(partial_query_msgs[0]->is<ai::embedding>()) == true;
    auto partial_query_embedding = partial_query_msgs[0]->get<ai::embedding>();

    // 4. Create an embedding for a dissimilar query
    std::vector<message_ptr> dissimilar_query_msgs;
    docwire::data_source{std::string{"What is the best C++ IDE?"}, mime_type{"text/plain"}, confidence::highest} | local_ai::embed(local_ai::embed::e5_query_prefix) | dissimilar_query_msgs;
    ensure(dissimilar_query_msgs.size()) == 1;
    ensure(dissimilar_query_msgs[0]->is<ai::embedding>()) == true;
    auto dissimilar_query_embedding = dissimilar_query_msgs[0]->get<ai::embedding>();

    // 5. Calculate and check similarities.
    double sim = cosine_similarity(passage_embedding.values, similar_query_embedding.values);
    double partial_sim = cosine_similarity(passage_embedding.values, partial_query_embedding.values);
    double dissim = cosine_similarity(passage_embedding.values, dissimilar_query_embedding.values);

    // The most important check is the relative order of the scores.
    ensure(sim) > partial_sim;
    ensure(partial_sim) > dissim;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
