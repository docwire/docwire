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

#include "pst_parser.h"

#include "document_elements.h"
#include "mail_elements.h"
#include "scoped_stack_push.h"
#include <iomanip>
#include <ctime>

#include <optional>
#include <iostream>
#include <stack>
extern "C"
{
#define LIBPFF_HAVE_BFIO
#include <stdio.h>
#include <libpff.h>
 #include <libbfio.h>
}

#include "error_tags.h"
#include "log_entry.h"
#include "log_scope.h"
#include "misc.h"
#include "make_error.h"
#include "nested_exception.h"
#include "serialization_message.h" // IWYU pragma: keep
#include "throw_if.h"
#include "message_counters.h"

namespace docwire
{

inline std::string toString(const uint8_t* text)
{
	if (text == nullptr)
		return "";
	return {(const char*)text};
}

inline std::string getAttachmentName(libpff_item_t* item)
{
	log_scope();
	size_t size;
	if (libpff_attachment_get_utf8_short_filename_size(item, &size, nullptr) != 1)
	{
		return "";
	}
	std::vector<uint8_t> buffer(size);
	if (libpff_attachment_get_utf8_short_filename(item, buffer.data(), size, nullptr) != 1)
	{
		return "";
	}
	return std::string(reinterpret_cast<const char*>(buffer.data()));
}

template <typename T, typename Deleter>
struct unique_handle
{
	T* handle = nullptr;

	unique_handle() = default;
	unique_handle(T* in) : handle{in} {}
	
	~unique_handle()
	{
		if (handle) Deleter{}(handle);
	}

	unique_handle(unique_handle&& in) noexcept : handle(std::exchange(in.handle, nullptr)) {}
	
	unique_handle& operator=(unique_handle&& in) noexcept
	{
		if (this != &in)
		{
			if (handle) Deleter{}(handle);
			handle = std::exchange(in.handle, nullptr);
		}
		return *this;
	}

	unique_handle(const unique_handle&) = delete;
	unique_handle& operator=(const unique_handle&) = delete;

	operator T*() const { return handle; }
	T** operator&() { return &handle; }
};

using pff_error = unique_handle<libpff_error_t, decltype([](libpff_error_t* ptr) { libpff_error_free(&ptr); })>;

using pff_item = unique_handle<libpff_item_t, decltype([](libpff_item_t* ptr) { libpff_item_free(&ptr, nullptr); })>;

using pff_file = unique_handle<libpff_file_t, decltype([](libpff_file_t* ptr) {
	pff_error error;
	libpff_file_close(ptr, &error);
	libpff_file_free(&ptr, &error);
})>;

using bfio_error = unique_handle<libbfio_error_t, decltype([](libbfio_error_t* ptr) { libbfio_error_free(&ptr); })>;

using bfio_handle = unique_handle<libbfio_handle_t, decltype([](libbfio_handle_t* ptr) {
	bfio_error error;
	libbfio_handle_close(ptr, &error);
	libbfio_handle_free(&ptr, &error);
})>;

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
  static constexpr uint64_t WINDOWS_TICK = 10000000;
  static constexpr uint64_t SHIFT = 11644473600;

  public:
	explicit Message(pff_item in)
		: _messageHandle{std::move(in)}
	{
		log_scope();
	}

	std::string getName() const
	{
		pff_error error;
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
    pff_error error;
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
    pff_error error;
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
		log_scope();
		int items;
		pff_error err;
    std::vector<RawAttachment> attachments;
    if (libpff_message_get_number_of_attachments(_messageHandle, &items, &err) != 1)
		{
			return {};
		}
		for (int i = 0; i < items; ++i)
		{
			pff_item item;
			if (libpff_message_get_attachment(_messageHandle, i, &item, &err) != 1)
			{
				log_entry();
				continue;
			}
			size64_t size;
			if (libpff_attachment_get_data_size(item, &size, &err) == -1)
			{
				log_entry();
				continue;
			}
			std::unique_ptr<uint8_t[]> datablob(new uint8_t[size + 1]);
			if (libpff_attachment_data_read_buffer(item, datablob.get(), size, &err) == -1)
			{
				log_entry();
				continue;
			}
      std::string attachment_name = getAttachmentName(item);
      attachments.push_back(RawAttachment(size, datablob, attachment_name));
		}
		return attachments;
	}

  private:
	pff_item _messageHandle;
};

class Folder
{
  public:
	explicit Folder(pff_item&& folderHandle)
		: _folderHandle(std::move(folderHandle))
	{
		log_scope();
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
	pff_item _folderHandle;
};

namespace
{

struct context
{
	const message_callbacks& emit_message;
};

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/vnd.ms-outlook-pst"},
	mime_type{"application/vnd.ms-outlook-ost"}
};

} // anonymous namespace

template<>
struct pimpl_impl<PSTParser> : pimpl_impl_base
{
	std::stack<context> m_context_stack;

	template <typename T>
	continuation emit_message(T&& object) const
	{
		return m_context_stack.top().emit_message(std::forward<T>(object));
	}

	template <typename T>
	continuation emit_message_back(T&& object) const
	{
		return m_context_stack.top().emit_message.back(std::forward<T>(object));
	}

	void parse(std::shared_ptr<std::istream> stream) const;

  private:
    void parse_element(const char* buffer, size_t size, const std::string& extension="") const;
    void parse_internal(const Folder& root, int deep, unsigned int &mail_counter) const;
};

void pimpl_impl<PSTParser>::parse_internal(const Folder& root, int deep, unsigned int &mail_counter) const
{
	log_scope(deep, mail_counter);
	for (int i = 0; i < root.getSubFolderNumber(); ++i)
	{
		auto sub_folder = root.getSubFolder(i);
    auto result = emit_message(mail::Folder{.name = sub_folder.getName(), .level = deep});
    if (result == continuation::skip)
    {
      continue;
    }
    parse_internal(sub_folder, deep + 1, mail_counter);
	emit_message(mail::CloseFolder{});
	}
	for (int i = 0; i < root.getMessageNumber(); ++i)
	{
		auto message = root.getMessage(i);

    auto html_text = message.getTextAsHtml();
    if(html_text)
    {
      auto result = emit_message(mail::Mail{.subject = message.getName(), .date = message.getCreationDate(), .level = deep});
      if (result == continuation::skip)
      {
        continue;
      }
      emit_message(mail::MailBody{});
      try
      {
        emit_message_back(data_source{*html_text, mime_type { "text/html" }, confidence::very_high});
      }
      catch (const std::exception&)
      {
        emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Failed to process mail body")));
      }
      ++mail_counter;
      emit_message(mail::CloseMailBody{});
    }

		auto attachments = message.getAttachments();
    for (auto &attachment : attachments)
    {
      file_extension extension { std::filesystem::path{attachment.m_name} };
      auto result = emit_message(
        mail::Attachment{.name = attachment.m_name, .size = attachment.m_size, .extension = extension});
      if (result == continuation::skip)
      {
        continue;
      }
      try
      {
        emit_message_back(data_source{std::string((const char*)attachment.m_raw_data.get(), attachment.m_size), extension});
      }
      catch (const std::exception&)
      {
        emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Failed to process attachment", attachment.m_name)));
      }
      emit_message(mail::CloseAttachment{});
    }
	emit_message(mail::CloseMail{});
	}
}

namespace
{

void libbfio_stream_initialize(libbfio_handle_t** handle, std::shared_ptr<std::istream> stream)
{
	log_scope();
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
	log_scope();
	bfio_handle handle;
	bfio_error bfio_err;

	libbfio_stream_initialize(&handle, stream);
	throw_if(libbfio_handle_open(handle, LIBBFIO_OPEN_READ, &bfio_err) != 1, "libbfio_handle_open failed");

	pff_file file;
	pff_error error{nullptr};
	throw_if (libpff_file_initialize(&file, &error) != 1, "libpff_file_initialize failed");
    throw_if (libpff_file_open_file_io_handle(file, handle, LIBBFIO_OPEN_READ, &error) != 1, "libpff_file_open_file_io_handle failed");

	pff_item root = nullptr;
	throw_if (libpff_file_get_root_folder(file, &root, &error) != 1, "libpff_file_get_root_folder failed");
	Folder root_folder(std::move(root));
  unsigned int mail_counter = 0;
	emit_message(document::Document{.metadata = []() { return attributes::Metadata{}; }});
  parse_internal(root_folder, 0, mail_counter);
	emit_message(document::CloseDocument{});
}

PSTParser::PSTParser() = default;

continuation PSTParser::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	log_scope(msg);

    if (!msg->is<data_source>())
        return emit_message(std::move(msg));

    auto& data = msg->get<data_source>();
    data.assert_not_encrypted();

    if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
        return emit_message(std::move(msg));

    log_entry();
    try
    {
        std::shared_ptr<std::istream> stream = data.istream();
        message_counters counters;
        auto counting_callbacks = make_counted_message_callbacks(emit_message, counters);
        scoped::stack_push<context> context_guard{impl().m_context_stack, context{counting_callbacks}};
        impl().parse(stream);
        if (counters.all_failed())
            throw make_error("No items were successfully processed", errors::uninterpretable_data{});
    }
    catch (const std::exception& e)
    {
        std::throw_with_nested(make_error("PST parsing failed"));
    }
    return continuation::proceed;
}

} // namespace docwire
