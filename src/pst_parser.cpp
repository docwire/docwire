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


#include <iomanip>
#include <ctime>

#include <optional>
#include <iostream>
extern "C"
{
#define LIBPFF_HAVE_BFIO
#include <stdio.h>
#include <libpff.h>
 #include <libbfio.h>
}

#include "error_tags.h"
#include "log.h"
#include "misc.h"
#include "pst_parser.h"
#include "throw_if.h"

namespace docwire
{

inline std::string toString(const uint8_t* text)
{
	if (text == nullptr)
		return "";
	return {(const char*)text};
}

constexpr uint64_t WINDOWS_TICK = 10000000;
constexpr uint64_t SHIFT = 11644473600;

inline std::unique_ptr<char[]> getAttachmentName(libpff_item_t* item)
{
	size_t size;
	if (libpff_attachment_get_utf8_short_filename_size(item, &size, nullptr) != 1)
	{
		return nullptr;
	}
	std::unique_ptr<char[]> out(std::make_unique<char[]>(size + 1));
	auto* filename = (uint8_t*)out.get();
	if (libpff_attachment_get_utf8_short_filename(item, filename, size, nullptr) != 1)
	{
		return nullptr;
	}
	return out;
}

/**
	TODO: Consider changing these to just unique_ptr to avoid boilerplate
*/

struct pffError
{
	libpff_error_t* error = nullptr;
	pffError() = default;
	pffError(libpff_error_t* error) : error{error} {}
	~pffError()
	{
		libpff_error_free(&error);
	}
	pffError& operator=(pffError&& in)
	{
		auto ptr = std::exchange(error, std::exchange(in.error, nullptr));
		libpff_error_free(&ptr);
		return *this;
	}
	pffError(const pffError&) = delete;
	pffError& operator=(const pffError&) = delete;
	pffError(pffError&& in)
	{
		auto ptr = std::exchange(error, std::exchange(in.error, nullptr));
		libpff_error_free(&ptr);
	}
	operator libpff_error_t*() const
	{
		return error;
	}
	auto operator&() const
	{
		return &error;
	}
	operator libpff_error_t*()
	{
		return error;
	}
	auto operator&()
	{
		return &error;
	}
};

struct pffItem
{
	libpff_item_t* handle = nullptr;
	pffItem() = default;
	pffItem(libpff_item_t* in) : handle{in}
	{
	}
	~pffItem()
	{
		libpff_item_free(&handle, nullptr);
	}
	pffItem& operator=(pffItem&& in)
	{
		auto ptr = std::exchange(handle, std::exchange(in.handle, nullptr));
		libpff_item_free(&ptr, nullptr);
		return *this;
	}
	pffItem(const pffItem&) = delete;
	pffItem& operator=(const pffItem& in) = delete;
	pffItem(pffItem&& in)
	{
		auto ptr = std::exchange(handle, std::exchange(in.handle, nullptr));
		libpff_item_free(&ptr, nullptr);
	}
	operator libpff_item_t*() const
	{
		return handle;
	}
	auto operator&() const
	{
		return &handle;
	}
	operator libpff_item_t*()
	{
		return handle;
	}
	auto operator&()
	{
		return &handle;
	}
};

struct RawAttachment
{
  RawAttachment(size_t size, std::unique_ptr<uint8_t[]>& raw_data, std::string name)
  : m_size(size),
    m_raw_data(std::move(raw_data)),
    m_name(name)
  {}
  std::string m_name;
  size_t m_size;
  std::unique_ptr<uint8_t[]> m_raw_data;
};

class Message
{
  public:
	explicit Message(pffItem in)
		: _messageHandle{std::move(in)}
	{
	}

	std::string getName() const
	{
		pffError error;
		size_t name_size;
    if (libpff_message_get_utf8_subject_size(_messageHandle, &name_size, &error) == 1)
    {
      std::unique_ptr<uint8_t[]> name = std::make_unique<uint8_t[]>(name_size);
      if (libpff_message_get_utf8_subject(_messageHandle, name.get(), name_size, &error) == 1)
      {
        return toString(name.get());
      }
      return "";
    }
    return "";
	}

  std::string getMailSender() const
  {
    pffError error;
    size_t name_size;
    if (libpff_message_get_utf8_sender_email_address_size(_messageHandle, &name_size, &error) == 1)
    {
      std::unique_ptr<uint8_t[]> name = std::make_unique<uint8_t[]>(name_size);
      if (libpff_message_get_utf8_sender_email_address(_messageHandle, name.get(), name_size, &error) == 1)
      {
        return toString(name.get());
      }
      return "";
    }
    return "";
  }

  std::string getMailRecipient() const
  {
    pffError error;
    size_t name_size;
    if (libpff_message_get_utf8_received_by_email_address_size(_messageHandle, &name_size, &error) == 1)
    {
      std::unique_ptr<uint8_t[]> name = std::make_unique<uint8_t[]>(name_size);
      if (libpff_message_get_utf8_received_by_email_address(_messageHandle, name.get(), name_size, &error) == 1)
      {
        return toString(name.get());
      }
      return "";
    }
    return "";
  }

	std::string getCreationDateAsString() const
	{
		std::time_t temp = getCreationDate();
    struct tm time_buffer;
		std::tm* t = thread_safe_gmtime(&temp, time_buffer);
		std::stringstream stream;
		stream << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
		return stream.str();
	}

  uint32_t getCreationDate() const
  {
    uint64_t creation_time;
    libpff_message_get_creation_time(_messageHandle, &creation_time, nullptr);

    creation_time = creation_time / WINDOWS_TICK - SHIFT;
    return static_cast<uint32_t>(creation_time);
  }

	std::optional<std::string> getTextAsRtf() const
	{
    size_t sizeOfBody;
    if (libpff_message_get_rtf_body_size(_messageHandle, &sizeOfBody, nullptr) == 1)
    {
      std::string text(sizeOfBody, 0);
      libpff_message_get_rtf_body(_messageHandle, (uint8_t *) text.data(), text.size(), nullptr);
      return text;
    }
    return {};
	}

	std::optional<std::string> getTextAsHtml() const
	{
    size_t sizeOfBody;
    if (libpff_message_get_html_body_size(_messageHandle, &sizeOfBody, nullptr) == 1)
    {
      std::string text(sizeOfBody, 0);
      libpff_message_get_html_body(_messageHandle, (uint8_t *) text.data(), text.size(), nullptr);
      return text;
    }
    return {};
	}

	std::vector<RawAttachment>
	getAttachments()
	{
		int items;
		pffError err;
    std::vector<RawAttachment> attachments;
    if (libpff_message_get_number_of_attachments(_messageHandle, &items, &err) != 1)
		{
			return {};
		}
		for (int i = 0; i < items; ++i)
		{
			pffItem item;
			if (libpff_message_get_attachment(_messageHandle, i, &item, &err) != 1)
			{
				docwire_log(debug) << "Message Get attachments failed: ";
				continue;
			}
			size64_t size;
			if (libpff_attachment_get_data_size(item, &size, &err) == -1)
			{
				docwire_log(debug) << "Get data size failed";
				continue;
			}
			std::unique_ptr<uint8_t[]> datablob(new uint8_t[size + 1]);
			if (libpff_attachment_data_read_buffer(item, datablob.get(), size, &err) == -1)
			{
				docwire_log(debug) << "Data Read failed";
				continue;
			}
      std::string attachment_name = getAttachmentName(item).get();
      attachments.push_back(RawAttachment(size, datablob, attachment_name));
		}
		return attachments;
	}

  private:
	pffItem _messageHandle;
};

class Folder
{
  public:
	explicit Folder(pffItem&& folderHandle)
		: _folderHandle(std::move(folderHandle))
	{
	}

	int getSubFolderNumber() const
	{
		int folders_no;
		libpff_folder_get_number_of_sub_folders(_folderHandle, &folders_no, nullptr);
		return folders_no;
	}

	int getMessageNumber() const
	{
		int messages_no;
		libpff_folder_get_number_of_sub_messages(_folderHandle, &messages_no, nullptr);
		return messages_no;
	}

	Folder getSubFolder(int index) const
	{
		libpff_item_t* sub_folder = NULL;
		libpff_folder_get_sub_folder(_folderHandle, index, &sub_folder, nullptr);
		Folder folder(sub_folder);
		return folder;
	}

	Message getMessage(int index) const
	{
		libpff_item_t* mail = NULL;
		libpff_folder_get_sub_message(_folderHandle, index, &mail, nullptr);
		Message msg(mail);
		return msg;
	}

	std::string getName() const
	{
		size_t name_size;
		libpff_folder_get_utf8_name_size(_folderHandle, &name_size, nullptr);
		std::unique_ptr<uint8_t[]> name = std::make_unique<uint8_t[]>(name_size);
		libpff_folder_get_utf8_name(_folderHandle, name.get(), name_size, nullptr);
		return toString(name.get());
	}

  private:
	pffItem _folderHandle;
};

template<>
struct pimpl_impl<PSTParser> : with_pimpl_owner<PSTParser>
{
	pimpl_impl(PSTParser& owner) : with_pimpl_owner{owner} {}
	void parse(std::shared_ptr<std::istream>) const;

  private:
    void parse_element(const char* buffer, size_t size, const std::string& extension="") const;
    void parse_internal(const Folder& root, int deep, unsigned int &mail_counter) const;
};

void pimpl_impl<PSTParser>::parse_internal(const Folder& root, int deep, unsigned int &mail_counter) const
{
	for (int i = 0; i < root.getSubFolderNumber(); ++i)
	{
		auto sub_folder = root.getSubFolder(i);
    auto callback = owner().sendTag(tag::Folder{.name = sub_folder.getName(), .level = deep});
    if(callback.skip)
    {
      continue;
    }
    parse_internal(sub_folder, deep + 1, mail_counter);
	owner().sendTag(tag::CloseFolder{});
	}
	for (int i = 0; i < root.getMessageNumber(); ++i)
	{
		auto message = root.getMessage(i);

    auto html_text = message.getTextAsHtml();
    if(html_text)
    {
      auto callback = owner().sendTag(tag::Mail{.subject = message.getName(), .date = message.getCreationDate(), .level = deep});
      if(callback.skip)
      {
        continue;
      }
      owner().sendTag(tag::MailBody{});
      owner().sendTag(data_source{*html_text, mime_type { "text/html" }, confidence::very_high});
      ++mail_counter;
      owner().sendTag(tag::CloseMailBody{});
    }

		auto attachments = message.getAttachments();
    for (auto &attachment : attachments)
    {
      file_extension extension { std::filesystem::path{attachment.m_name} };
      auto callback = owner().sendTag(
        tag::Attachment{.name = attachment.m_name, .size = attachment.m_size, .extension = extension});
      if(callback.skip)
      {
        continue;
      }
      owner().sendTag(data_source{std::string((const char*)attachment.m_raw_data.get(), attachment.m_size), extension});
      owner().sendTag(tag::CloseAttachment{});
    }
	owner().sendTag(tag::CloseMail{});
	}
}

namespace
{

void libbfio_stream_initialize(libbfio_handle_t** handle, std::shared_ptr<std::istream> stream)
{
	throw_if (handle == NULL, "Invalid handle", errors::program_logic{});
	throw_if (*handle != NULL, "Handle already initialized", errors::program_logic{});
	auto libbfio_handle_initialize_result = libbfio_handle_initialize(
		handle,
		(intptr_t*)stream.get(),
		[](intptr_t **, libbfio_error_t **)->int { return 1; }, // free
    	[](intptr_t **, intptr_t *, libbfio_error_t **)->int { return -1; }, // clone
	    [](intptr_t *, int, libbfio_error_t **)->int { return 1; }, // open
	    [](intptr_t *, libbfio_error_t **)->int { return 1; }, // close
	    [](intptr_t* io_handle, uint8_t* buffer, size_t size, libbfio_error_t **)->ssize_t // read
    	{
        	std::istream* stream = reinterpret_cast<std::istream*>(io_handle);
        	stream->read(reinterpret_cast<char*>(buffer), size);
        	return stream->gcount();
    	},
	    [](intptr_t *, const uint8_t *, size_t, libbfio_error_t **)->ssize_t { return -1; }, // write
	    [](intptr_t* io_handle, off64_t offset, int whence, libbfio_error_t **)->off64_t // seek offset
    	{
        	std::istream* stream = reinterpret_cast<std::istream*>(io_handle);
        	switch (whence)
        	{
        		case SEEK_SET:
            		stream->seekg(offset, std::ios_base::beg);
            		break;
            	case SEEK_CUR:
            		stream->seekg(offset, std::ios_base::cur);
            		break;
            	case SEEK_END:
            		stream->seekg(offset, std::ios_base::end);
            		break;
            	default:
					throw make_error("Invalid whence argument value", whence, errors::program_logic{});
        	}
			return stream->tellg();
    	},
		[](intptr_t *, libbfio_error_t **)->int { return 1; }, // exists
	    [](intptr_t *, libbfio_error_t **)->int { return 1; }, // is open
	    [](intptr_t* io_handle, size64_t* size, libbfio_error_t **)->int // get size
    	{
    		std::istream* stream = reinterpret_cast<std::istream*>(io_handle);
    		std::streampos pos = stream->tellg();
			throw_if (pos == std::streampos{-1}, "Failed to get stream position");
    		throw_if (!stream->seekg(0, std::ios_base::end), "Failed to seek to the end of the stream");
        	*size = stream->tellg();
			throw_if (*size == std::streampos(-1), "Failed to get stream position");
        	throw_if (!stream->seekg(pos, std::ios_base::beg), "Failed to seek to the original position");
    		return 1;
    	},
	    LIBBFIO_FLAG_IO_HANDLE_MANAGED | LIBBFIO_FLAG_IO_HANDLE_CLONE_BY_FUNCTION, nullptr);
	throw_if (libbfio_handle_initialize_result != 1, "libbfio_handle_initialize failed", libbfio_handle_initialize_result);
}

} // anonymous namespace

void pimpl_impl<PSTParser>::parse(std::shared_ptr<std::istream> stream) const
{
	libpff_file_t* file = nullptr;
	pffError error{nullptr};
	if (libpff_file_initialize(&file, &error) != 1)
	{
		docwire_log(severity_level::error) << "Unable to initialize file.";
		return;
	}

  libbfio_handle_t* handle = nullptr;
    libbfio_error_t* bfio_error = nullptr;
    libbfio_stream_initialize(&handle, stream);
    libbfio_handle_open(handle, LIBBFIO_OPEN_READ_WRITE_TRUNCATE, &bfio_error);
    libpff_file_open_file_io_handle(file, handle, LIBBFIO_OPEN_READ, &error);

	pffItem root = NULL;
	libpff_file_get_root_folder(file, &root, &error);
	Folder root_folder(std::move(root));
  unsigned int mail_counter = 0;
	owner().sendTag(tag::Document{.metadata = []() { return attributes::Metadata{}; }});
  parse_internal(root_folder, 0, mail_counter);
	owner().sendTag(tag::CloseDocument{});

  if (file)
  {
    libpff_file_close(file, &error);
    libpff_file_free(&file, &error);
  }
  if (handle)
  {
    libbfio_handle_close(handle, &bfio_error);
    libbfio_handle_free(&handle, &bfio_error);
    handle = nullptr;
  }
  libbfio_error_free(&bfio_error);
}

void
PSTParser::parse(const data_source& data)
{
	docwire_log(debug) << "Using PST parser.";
  std::shared_ptr<std::istream> stream = data.istream();
	impl().parse(stream);
}

PSTParser::PSTParser()
{
}

} // namespace docwire
