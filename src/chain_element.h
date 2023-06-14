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

  /**
   * @brief Connects next object to the end of chain
   * @param chain_element
   */
  void connect(const ChainElement &chain_element);

  /**
   * @brief Emits signal with Info object to the next element
   * @param info
   */
  void emit(Info &info) const;

  /**
   * @brief Check if ChainElement is a leaf(last element which doesn't emit any signals). At this moment only Exporters are leafs.
   * @return true if leaf
   */
  virtual bool is_leaf() const = 0;

  ChainElement& operator=(const ChainElement &chain_element);

  ParsingChain operator|(ChainElement& chainElement);

  ParsingChain operator|(ChainElement&& chainElement);

  /**
   * @brief Start processing
   * @param info
   */
  virtual void process(Info &info) const = 0;

  /**
   * @brief Creates clone
   * @return new ChainElement
   */
  virtual ChainElement* clone() const = 0;

  /**
   * @brief Set parent (previous element)
   * @param chainElement
   */
  void set_parent(const std::shared_ptr<ChainElement>& chainElement);

  /**
   * @return pointer to previous element in a chain
   */
  std::shared_ptr<ChainElement> get_parent() const;

private:
  struct DllExport Implementation;
  struct DllExport ImplementationDeleter { void operator() (Implementation*); };
  std::unique_ptr<Implementation, ImplementationDeleter> base_impl;

  std::shared_ptr<ChainElement> m_parent;
};

}
#endif //DOCTOTEXT_CHAIN_ELEMENT_H
