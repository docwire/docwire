/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_TEXT_FORMATTING_H
#define DOCWIRE_TEXT_FORMATTING_H

#include <cstdio>
#include <string>
#include <vector>
#include "log_scope.h"

namespace docwire
{

using svector = std::vector<std::string>;

inline int prepareTable(std::vector<svector>& p_table)
{
	DOCWIRE_LOG_SCOPE();
	if (p_table.empty())
		return 0;
	for (int i = 0; i < p_table.size(); ++i)
	{
		if (p_table.at(i).empty())
			continue;
		for (int j = 0; j < p_table.at(i).size(); ++j)
		{
			if (p_table.at(i).at(j).length() < 2)
				continue;
			int k = p_table.at(i).at(j).find('\n', p_table.at(i).at(j).length() - 2);
			if (k >= 0)
				p_table.at(i).at(j).replace(k, 1, "");
		}
	}
	return 1;
}

inline int prepareList(std::vector<std::string>& p_list)
{
	DOCWIRE_LOG_SCOPE();
	if (p_list.empty())
		return 0;
	for (int i = 0; i < p_list.size(); ++i)
	{
		if (p_list.at(i).empty())
			continue;
		if (p_list.at(i).length() < 2)
			continue;
		int k = p_list.at(i).find('\n', p_list.at(i).length() - 2);
		if (k >= 0)
			p_list.at(i).replace(k, 1, "");
	}
	return 1;
}

inline std::string formatTable(std::vector<svector>& mcols)
{
	DOCWIRE_LOG_SCOPE();
	std::string table_out;

	prepareTable(mcols);

	for (int i = 0; i < mcols.size(); ++i)
	{
		for (int j = 0; j < mcols.at(i).size(); ++j)
		{
			table_out += mcols.at(i).at(j);
			if (j + 1 == mcols.at(i).size())
			{
				table_out += "\n";
				break;
			}
			table_out += "\t";
		}
	}
	return table_out;
}

inline std::string formatUrl(const std::string& mlink_url, const std::string& mlink_text)
{
	DOCWIRE_LOG_SCOPE(mlink_url, mlink_text);
	std::string u_url;
	if (mlink_url.length() > 0)
	{
		u_url += "<" + mlink_url + ">";
	}
	u_url += mlink_text;
	return u_url;
}

inline std::string formatList(std::vector<std::string>& mlist)
{
	DOCWIRE_LOG_SCOPE();
	std::string list_out;
	prepareList(mlist);

	for (int i = 0; i < mlist.size(); ++i)
	{
		list_out = list_out + "- " + mlist.at(i) + "\n";
	}
	return list_out;
}

inline std::string formatNumberedList(std::vector<std::string>& mlist)
{
	DOCWIRE_LOG_SCOPE();
	std::string list_out;
	prepareList(mlist);
	char count[100];
	for (int i = 0; i < mlist.size(); ++i)
	{
		sprintf(count, "%d. ", i + 1);
		list_out = list_out + count;
		list_out = list_out + mlist.at(i);
		list_out = list_out + "\n";
	}
	return list_out;
}

} // namespace docwire

#endif
