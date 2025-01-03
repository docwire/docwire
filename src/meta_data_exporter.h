/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_META_DATA_EXPORTER_H
#define DOCWIRE_META_DATA_EXPORTER_H

#include "chain_element.h"

namespace docwire
{

/**
 * @brief Exports meta data only to plain text format.
 */
class DllExport MetaDataExporter: public ChainElement, public with_pimpl<MetaDataExporter>
{
public:
  MetaDataExporter();

  void process(Info& info) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
  using with_pimpl<MetaDataExporter>::impl;
};

} // namespace docwire

#endif //DOCWIRE_EXPORTER_H
