/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_HTML_WRITER_H
#define DF0_HTML_WRITER_H

#include "html_export.h"
#include <iostream>

#include "pimpl.h"
#include "writer.h"

namespace df0
{

class DF0_HTML_EXPORT html_writer : public writer, public with_pimpl<html_writer>
{
public:

  html_writer();

  /**
   * @brief Converts text from callback to html format
   * @param tag data from callback
   * @param stream output stream
   */
  void write_to(const message_ptr& msg, std::ostream &stream) override;

private:
  using with_pimpl<html_writer>::impl;
};
} // namespace df0

#endif //DF0_HTML_WRITER_H
