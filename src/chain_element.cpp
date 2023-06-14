#include <boost/signals2.hpp>

#include "chain_element.h"
#include "parsing_chain.h"

using namespace doctotext;

struct ChainElement::Implementation
{
  Implementation()
  : m_on_new_node_signal(std::make_shared<boost::signals2::signal<void(doctotext::Info &info)>>())
  {}

  Implementation(const Implementation& implementation)
  : m_on_new_node_signal(std::make_shared<boost::signals2::signal<void(doctotext::Info &info)>>())
  {}

  int connect(const ChainElement &chain_element)
  {
    m_on_new_node_signal->connect([&chain_element](Info &info){chain_element.get_function()(info);});
    return 0;
  }

  void emit(Info &info) const
  {
    (*m_on_new_node_signal)(info);
  }

  std::shared_ptr<boost::signals2::signal<void(doctotext::Info &info)>> m_on_new_node_signal;
};

ChainElement::ChainElement()
{
  base_impl = std::unique_ptr<Implementation, ImplementationDeleter>{new Implementation{}, ImplementationDeleter{}};
}

ChainElement::ChainElement(const ChainElement& element)
: base_impl(new Implementation(*(element.base_impl))),
  parent(element.parent)
{}

ChainElement&
ChainElement::operator=(const ChainElement &chain_element)
{
  base_impl->m_on_new_node_signal = chain_element.base_impl->m_on_new_node_signal;
  parent = chain_element.parent;
  return *this;
}

ParsingChain
ChainElement::operator|(ChainElement& chainElement)
{
  return ParsingChain(*this, chainElement);
}

ParsingChain
ChainElement::operator|(ChainElement&& chainElement)
{
  return ParsingChain(*this, chainElement);
}

int
ChainElement::connect(const ChainElement &chain_element)
{
  return base_impl->connect(chain_element);
}

void
ChainElement::emit(Info &info) const
{
  base_impl->emit(info);
}

void
ChainElement::ImplementationDeleter::operator()(ChainElement::Implementation *impl)
{
  delete impl;
}