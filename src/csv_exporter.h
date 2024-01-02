/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_CSV_EXPORTER_H
#define DOCWIRE_CSV_EXPORTER_H

#include "chain_element.h"

namespace docwire
{

/**
 * @brief Exports data to CSV format.
 */
class DllExport CsvExporter: public ChainElement
{
public:
	CsvExporter();
	CsvExporter(const CsvExporter& other);

  CsvExporter* clone() const override
  {
    return new CsvExporter(*this);
  }

  void process(Info& info) const override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	struct Implementation;
	struct DllExport ImplementationDeleter { void operator() (Implementation*); };
	std::unique_ptr<Implementation, ImplementationDeleter> impl;
};

} // namespace docwire

#endif //DOCWIRE_CSV_EXPORTER_H
