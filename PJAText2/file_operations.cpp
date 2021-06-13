#include "type_aliases.h"
#include <iostream>


namespace File {
	/**
	 * @brief Checks if a specific file exists.
	 *
	 * @param file_name - name of the file to check
	 * @return true - If the file exists
	 * @return false - If the file not exists
	 */
	inline bool exists(const String& file_name) {
		IFStream file_stream(file_name);
		return file_stream.good();
	}

	/**
	 * @brief Reads from the specific file, without checking for any errors.
	 *
	 * @param file_name - name of the file to read
	 * @return Content of the file as a String
	 */
	inline String read_unchecked(const String& file_name) {
		IFStream file_stream(file_name);
		auto content = String();

		do {
			String line;
			getline(file_stream, line);

			content.append(line).append("\n");
		} while (!file_stream.eof());

		file_stream.close();
		return content;
	}

	/**
	 * @brief Overwrites everything in the specific file
	 *
	 * @param file_name - name of the file to write
	 * @param content - new content of the file
	 */
	inline void write_unchecked(const String& file_name, const String& content) {
		OFStream file_stream(file_name, OFStream::trunc);
		file_stream << content;
		file_stream.close();
	}

	/**
	 * @brief Get the size of the specific file
	 *
	 * @param file_name - name of the file to write
	 * @return Size of the file
	 */
	inline usize get_size(const String& file_name) {
		IFStream file_stream(file_name, std::ios::binary);

		file_stream.seekg(0, std::ios::end);
		usize size = file_stream.tellg();

		file_stream.close();
		return size;
	}
}
