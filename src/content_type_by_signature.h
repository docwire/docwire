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

#ifndef DOCWIRE_CONTENT_TYPE_BY_SIGNATURE_H
#define DOCWIRE_CONTENT_TYPE_BY_SIGNATURE_H

#include "chain_element.h"
#include "content_type_export.h"
#include "data_source.h"
#include "document_elements.h"
#include "make_error.h"
#include "nested_exception.h"
#include "ref_or_owned.h"

/**
 * @namespace docwire::content_type::by_signature
 * @brief Provides content type detection based on file signatures (magic bytes).
 * 
 * This module wraps `libmagic`. 
 * 
 * ### Quirks and Normalization
 * `libmagic` occasionally returns obsolete or non-standard MIME types (e.g., returning `text/xml` 
 * instead of `application/xml`, or `image/x-ms-bmp` instead of `image/bmp`). This module automatically 
 * normalizes these outputs to modern IANA standards so downstream detectors have a consistent target.
 * 
 * ### Stream Limitations
 * For non-seekable streams, `libmagic` cannot read the Central Directory of ZIP containers or strictly 
 * validate RIFF containers. It will often return `application/zip` or `application/octet-stream` in 
 * these cases. Downstream heuristic detectors are required to resolve these generic types.
 */
namespace docwire::content_type::by_signature
{

struct allow_multiple
{
    bool v;
};

/**
 * @brief Database of signatures
 *
 * This class represents a database of signatures used for content type detection.
 * Database is loaded from a file during initialization and provides a list of file signatures along with their associated mime types.
 *
 * @see content_type::detect
 * @see content_type::detector
 * @see content_type::by_signature::detector
 * @see content_type::by_signature::detect
 */
class DOCWIRE_CONTENT_TYPE_EXPORT database : public with_pimpl<database>
{
public:
    explicit database();
    friend DOCWIRE_CONTENT_TYPE_EXPORT void detect(data_source& data, const database& database_to_use, allow_multiple allow_multiple);
};

/**
* @brief Detects and assigns content types to the provided data source using signatures-based content detection.
*
* Reads the initial bytes of the data source and queries the signature database. 
* Any legacy MIME types returned by the database are automatically normalized to modern IANA standards.
*
* @param data The data source to be analyzed for content type detection. Detected MIME types will be added to this object.
* @param database_to_use The loaded database of signatures used for signature-based content detection. It will be created (and loaded) if not provided.
* @param allow_multiple Allow multiple content types to be assigned to the same data source.
*
* @see @ref file_type_determination.cpp "performing file type detection example"
* @see content_type::detect
* @see content_type::detector
* @see content_type::by_signature::database
* @see content_type::by_signature::detector
*/
DOCWIRE_CONTENT_TYPE_EXPORT void detect(data_source& data, const database& database_to_use = database{}, allow_multiple allow_multiple = {false});

/**
* @brief Detector chain element
*
* This class represents a chain element that detects and assigns content types to data sources using signatures-based content detection.
*
* @see @ref file_type_determination.cpp "performing file type detection example"
* @see content_type::detector
* @see content_type::by_signature::detect
*/
class detector : public ChainElement
{
public:

    /**
     * @brief Constructs a new detector with the given database of signatures and multiple content type detection configuration.
     *
     * The detector will use the provided database of signatures for content type detection.
     * If no database is provided, it will be created and loaded.
     *
     * @param database_to_use The database of signatures to be used for content type detection.
     * @param allow_multiple Allow multiple content types to be assigned to the same data source.
     *
     * @see content_type::detector
     * @see content_type::by_signature::database
     * @see content_type::by_signature::detect
     */
    explicit detector(ref_or_owned<database> database_to_use = database{}, allow_multiple allow_multiple = {false})
        : m_database_to_use(database_to_use), m_allow_multiple{allow_multiple} {}

    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override
    {
        try
        {
            if (msg->is<data_source>())
            {
                data_source& data = msg->get<data_source>();
                detect(data, m_database_to_use.get(), m_allow_multiple);
            }
            else if (msg->is<document::Image>())
            {
                data_source& data = msg->get<document::Image>().source;
                detect(data, m_database_to_use.get(), m_allow_multiple);
            }
        }
        catch (const std::exception& e)
        {
            emit_message(make_nested_ptr(std::current_exception(), DOCWIRE_MAKE_ERROR("Content type detection by signature failed")));
        }
        return emit_message(std::move(msg));
    }

    bool is_leaf() const override
	{
		return false;
	}

private:
    ref_or_owned<database> m_database_to_use;
    allow_multiple m_allow_multiple;
};

} // namespace docwire::content_type::by_signature

#endif // DOCWIRE_CONTENT_TYPE_BY_SIGNATURE_H
