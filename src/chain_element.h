#ifndef DOCTOTEXT_CHAIN_ELEMENT_H
#define DOCTOTEXT_CHAIN_ELEMENT_H

#include "parser.h"

namespace doctotext
{

class ParsingChain;

class ChainElement
{
public:
  ChainElement();
  ChainElement(const ChainElement& element);
  virtual ~ChainElement() = default;
  int connect(const ChainElement &chain_element);
  void emit(Info &info) const;

  virtual bool is_leaf() const = 0;

  std::shared_ptr<ChainElement> parent;

  ChainElement& operator=(const ChainElement &chain_element);

  ParsingChain operator|(ChainElement& chainElement);

  ParsingChain operator|(ChainElement&& chainElement);

  virtual NewNodeCallback get_function() const = 0;

  virtual ChainElement* clone() const = 0;

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> base_impl;
};

}
#endif //DOCTOTEXT_CHAIN_ELEMENT_H
