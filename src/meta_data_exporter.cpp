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

#include "meta_data_exporter.h"

#include "meta_data_writer.h"
#include <sstream>

namespace docwire
{

template<>
struct pimpl_impl<MetaDataExporter> : pimpl_impl_base
{
	std::shared_ptr<std::stringstream> m_stream;
	MetaDataWriter m_writer;
};

MetaDataExporter::MetaDataExporter()
{}

void MetaDataExporter::process(Info& info)
{
	if (std::holds_alternative<std::exception_ptr>(info.tag))
	{
		emit(info);
		return;
	}
	if (std::holds_alternative<tag::Document>(info.tag) || !impl().m_stream)
		impl().m_stream = std::make_shared<std::stringstream>();
	impl().m_writer.write_to(info.tag, *impl().m_stream);
	if (std::holds_alternative<tag::CloseDocument>(info.tag))
	{
		Info info(data_source{seekable_stream_ptr{impl().m_stream}});
		emit(info);
		impl().m_stream.reset();
	}
}

} // namespace docwire
