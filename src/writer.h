/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_WRITER_H
#define DF0_WRITER_H

#include "core_export.h"
#include "message.h"
#include <ostream>
#include <string>

namespace df0
{

class text_element
{
public:
  text_element() = default;
  explicit text_element(std::string text) : text(std::move(text)) {};
  virtual void write_to(std::ostream &file) const
  {
    file << text;
  }

private:
  std::string text;
};

/**
 * @brief The Writer class is used to write data from callbacks to an output stream.
 */
class writer
{
public:
  /**
   * @brief writes data from callback to the specific output stream
   * @param msg data from callback
   * @param stream output stream
   */
  virtual void write_to(const message_ptr& msg, std::ostream &stream) = 0;
};

} // namespace df0

#endif //DF0_WRITER_H
