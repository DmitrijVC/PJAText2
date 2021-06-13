#pragma once

#include <iostream>
#include <regex>
#include <iterator>
#include <functional>

#include "engine.h"


namespace __Helpers {
	namespace Info {
		/**
		 * @brief Create the StringStream with the flag name prefix.
		 *
		 * @param flag - target Flag's instance
		 * @return newly created StringStream
		 */
		auto flag_string_stream(const Flag& flag) {
			StringStream ss;
			ss << "<" << flag.name << "> ";

			return ss;
		}

		/**
		 * @brief Create a StringStream with the flag's name prefix and values of Vec<T> as a visual structure.
		 *
		 * @tparam T - Type of values in the "collection" argument's Vec
		 * @param flag - target Flag instance
		 * @param collection - target Vec with values to add into the visual structure
		 * @return newly created StringStream
		 */
		template <typename T>
		auto flag_string_stream_structure(const Flag& flag, const Vec<T>& collection) {
			auto ss = flag_string_stream(flag);

			if (collection.empty()) {
				ss << "{ }";
			}
			else {
				ss << "{\n";
				for (const T& word : collection) {
					ss << "    \"" << word << "\",\n";
				}
				ss << "}";
			}

			return ss;
		}
	}

	namespace Regex {
		/**
		 * @brief Counts the number of matches returned from the regex.
		 *
		 * @param target - String instance to check with the regex
		 * @param regex_str - Regex in String representation
		 * @return number of the found matches
		 */
		usize count_matches(const String& target, const String& regex_str) {
			std::regex regex(regex_str);

			auto begin = std::sregex_iterator(
				target.begin(),
				target.end(),
				regex
			);

			return std::distance(begin, std::sregex_iterator());
		}

		/**
		 * @brief Converts all matches returned from the regex into a vector of strings.
		 *
		 * @param target - String instance to check with the regex
		 * @param regex_str - Regex in String representation
		 * @return Vec<String> collection with all the found matches
		 */
		auto matches_to_vec(const String& target, const String& regex_str) {
			auto result_vec = Vec<String>();
			std::regex regex(regex_str);
			std::smatch match;

			auto matches_begin = std::sregex_iterator(
				target.begin(),
				target.end(),
				regex
			);

			auto matches_end = std::sregex_iterator();

			for (std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
				result_vec.push_back((*i).str());
			}

			return result_vec;
		}

		/**
		 * @brief Grabs all the "words" in a String.
		 *
		 * @param target - String instance to grab the "words"
		 * @return Vec<String> collection with all the found "words"
		 */
		Vec<String> get_words(const String& target) {
			return __Helpers::Regex::matches_to_vec(
				target,
				"(?!\\s)[\\S]+"
			);
		}
	}

	namespace Comparators {
		/**
		 * @brief Returns the "greater comparator" of one type values with or without considering their size.
		 *
		 * @tparam T - Type of the values that will be compared
		 * @param as_size - Should the comparator consider the greater value by their .size() method
		 * @return The lambda comparator of two borrowed const values of the same type
		 */
		template <typename T>
		std::function<bool(const T&, const T&)> get_default(const bool as_size) {
			if (as_size) {
				return [](const T& left, const T& right) {
					return left.size() < right.size();
				};
			}

			return [](const T& left, const T& right) {
				return left < right;
			};
		}

		/**
		 * @brief Returns the "reverse comparator" of one type values with or without considering their size.
		 *
		 * @tparam T - Type of the values that will be compared
		 * @param as_size - Should the comparator consider the lower value by their .size() method
		 * @return The lambda comparator of two borrowed const values of the same type
		 */
		template <typename T>
		std::function<bool(const T&, const T&)> get_reverse(const bool as_size) {
			if (as_size) {
				return [](const T& left, const T& right) {
					return left.size() > right.size();
				};
			}

			return [](const T& left, const T& right) {
				return left > right;
			};
		}
	}

	namespace Strings {
		/**
		 * @brief Checks if two Strings are anagrams.
		 *
		 * @param first - Copy of the first String
		 * @param second - Copy of the second String
		 * @return true - if the two Strings are anagrams
		 * @return false - if the two Strings aren't anagrams
		 */
		bool are_anagrams(String first, String second) {
			if (first.size() != second.size()) {
				return false;
			}

			std::sort(first.begin(), first.end());
			std::sort(second.begin(), second.end());

			return first == second;
		}

		/**
		 * @brief Checks if two Strings are palindromes.
		 *
		 * @param first - const reference of the first String
		 * @param second - Copy of the second String
		 * @return true - if the two Strings are palindromes
		 * @return false - if the two Strings aren't palindromes
		 */
		bool are_palindromes(const String& first, String second) {
			if (first.size() != second.size()) {
				return false;
			}

			std::reverse(second.begin(), second.end());

			return first == second;
		}
	}
}


/**
 * @brief Commands that are responsible for extending Engine functionality.
 * • Not flexible.
 * • Shouldn't be modified.
 * • Have to be added automatically in the Engine constructor.
 */
namespace BaseCommands {

	/**
	 * @brief Command responsible for getting the content of the source file.
	 */
	struct SourceFile : Command
	{
		static const String CALLER_VALUE;
		static const String ALIAS_VALUE;

		String caller() const override {
			return CALLER_VALUE;
		}

		String alias() const override {
			return ALIAS_VALUE;
		}

		/**
		 * @brief Checks if the Flag's argument is present, or the file exists.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output(Error) - If Flag doesn't have an argument, or the argument is invalid
		 * @return Output(Ok) - If succeeded
		 */
		Output validate(const Flag& flag, Instruction&, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			if (flag.arg.empty()) {
				ss << "This flag requires an argument!";
				return Output::new_err(ss.str());
			}

			if (!File::exists(flag.arg)) {
				ss << "Provided file doesn't exists!";
				return Output::new_err(ss.str());
			}

			operations.file_in = flag.arg;
			operations.source = File::read_unchecked(operations.file_in);

			return Output::new_ok("");
		}

		Output execute(const Flag&, Operations&) const override {
			return Output::new_ok("");
		}
	};


	/**
	 * @brief Command responsible for getting the flags from a file.
	 * Only the representation is needed. No functionality.
	 */
	struct InputFile : Command {
		static const String CALLER_VALUE;
		static const String ALIAS_VALUE;

		String caller() const override {
			return CALLER_VALUE;
		}

		String alias() const override {
			return ALIAS_VALUE;
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		Output execute(const Flag& flag, Operations& operations) const override {
			return Output::new_ok("");
		}
	};


	/**
	 * @brief Command responsible for saving Output into a file.
	 */
	struct OutputFile : Command {
		static const String CALLER_VALUE;
		static const String ALIAS_VALUE;

		String caller() const override {
			return CALLER_VALUE;
		}

		String alias() const override {
			return ALIAS_VALUE;
		}

		/**
		 * @brief Checks if the flag has an argument, and saves the file name.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output(Error) - If Flag doesn't have an argument
		 * @return Output(Ok) - If succeeded
		 */
		Output validate(const Flag& flag, Instruction&, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			if (flag.arg.empty()) {
				ss << "This flag requires an argument!";
				return Output::new_err(ss.str());
			}

			operations.file_out = flag.arg;
			return Output::new_ok("");
		}

		Output execute(const Flag& flag, Operations& operations) const override {
			return Output::new_ok("");
		}
	};

	const String SourceFile::CALLER_VALUE = "-f";
	const String SourceFile::ALIAS_VALUE = "--file";
	const String InputFile::CALLER_VALUE = "-i";
	const String InputFile::ALIAS_VALUE = "--input";
	const String OutputFile::CALLER_VALUE = "-o";
	const String OutputFile::ALIAS_VALUE = "--output";
}


/**
 * @brief Executable commands returning non empty Output.
 * • Flexible.
 * • Can be modified.
 * • Have to be added manually by the Engine::add function.
 */
namespace OperationalCommands {

	/**
	 * @brief Command responsible for counting new lines in the source file
	 */
	struct CountLines : Command
	{
		String caller() const override {
			return "-n";
		}

		String alias() const override {
			return "--newlines";
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Counts new lines in the source file.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a number of new lines in the source file
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			ss << "New lines: " << std::count(
				operations.source.begin(),
				operations.source.end(),
				'\n'
			);

			return Output::new_ok(ss.str());
		}
	};


	/**
	 * @brief Command responsible for counting the amount of digits in the source file.
	 */
	struct CountDigits : Command
	{
		char digits[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

		String caller() const override {
			return "-d";
		}

		String alias() const override {
			return "--digits";
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Counts digits in the source file.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a number of digits in the source file
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			ss << "Digits: " << std::count_if(
				operations.source.begin(),
				operations.source.end(),
				[this](const char ch) {
				return is_digit(ch);
			}
			);

			return Output::new_ok(ss.str());
		}

		/**
		 * @brief Checks if the specific char is a digit.
		 *
		 * @param ch - target char
		 * @return true - if the char is a digit
		 * @return false - if the char is not a digit
		 */
		bool is_digit(const char ch) const {
			return std::find(
				std::begin(digits),
				std::end(digits),
				ch
			) != std::end(digits);
		}
	};


	/**
	 * @brief Command responsible for counting the amount of numbers in the source file.
	 */
	struct CountNumbers : Command
	{
		String caller() const override {
			return "-dd";
		}

		String alias() const override {
			return "--numbers";
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Counts numbers (as words) in the source file.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a number of numbers in the source file
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			ss << "Numbers: " << __Helpers::Regex::count_matches(
				operations.source,
				"(^|\\s)[0-9]+(?!\\w)"
			);

			return Output::new_ok(ss.str());
		}
	};


	/**
	 * @brief Command responsible for counting the amount of chars in the source file.
	 */
	struct CountChars : Command
	{
		String caller() const override {
			return "-c";
		}

		String alias() const override {
			return "--chars";
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Counts chars in the source file.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a number of chars in the source file
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			ss << "Chars: " << operations.source.length() - 1;

			return Output::new_ok(ss.str());
		}
	};


	/**
	 * @brief Command responsible for counting the amount of words in the source file.
	 */
	struct CountWords : Command
	{
		String caller() const override {
			return "-w";
		}

		String alias() const override {
			return "--words";
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Counts words in the source file.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a number of words in the source file
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			ss << "Words: " << __Helpers::Regex::count_matches(
				operations.source,
				"(?!\\s)[\\S]+"
			);

			return Output::new_ok(ss.str());
		}
	};


	/**
	 * @brief Command responsible for getting all anagrams from the source file.
	 */
	struct ShowAnagrams : Command {
		String caller() const override {
			return "-a";
		}

		String alias() const override {
			return "--anagrams";
		}

		/**
		 * @brief Checks if this flag is the last one, and if it contains an argument.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param inst - Instruction with all the Flags
		 * @param operations - Struct holding operational data
		 * @return Output(Error) - If it's not the last one flag, or doesn't contain an argument
		 * @return Output(Ok) - If succeeded
		 */
		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			if (inst.get_flag(flag.pos + 1).is_some()) {
				ss << "This flag should be the last one";
				return Output::new_err(ss.str());
			}

			if (flag.arg.empty()) {
				ss << "This flag requires an argument!";
				return Output::new_err(ss.str());
			}

			return Output::new_ok("");
		}

		/**
		 * @brief Gets all the anagrams from the source file, and ereses the repeated values.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a structure of found anagrams
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto words_source = __Helpers::Regex::get_words(operations.source);
			auto words_flag = __Helpers::Regex::get_words(flag.arg);

			auto anagrams = Vec<String>();

			for (String& first : words_source) {
				for (String& second : words_flag) {
					if (__Helpers::Strings::are_anagrams(first, second)) {
						anagrams.push_back(first);
					}
				}
			}

			anagrams.erase(
				std::unique(anagrams.begin(), anagrams.end()),
				anagrams.end()
			);

			return Output::new_ok(
				__Helpers::Info::flag_string_stream_structure(flag, anagrams).str()
			);
		}
	};


	/**
	 * @brief Command responsible for getting all palindromes from the source file.
	 */
	struct ShowPalindromes : Command {
		String caller() const override {
			return "-p";
		}

		String alias() const override {
			return "--palindromes";
		}

		/**
		 * @brief Checks if this flag is the last one, and if it contains an argument.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param inst - Instruction with all the Flags
		 * @param operations - Struct holding operational data
		 * @return Output(Error) - If it's not the last one flag, or doesn't contain an argument
		 * @return Output(Ok) - If succeeded
		 */
		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			if (inst.get_flag(flag.pos + 1).is_some()) {
				ss << "This flag should be the last one";
				return Output::new_err(ss.str());
			}

			if (flag.arg.empty()) {
				ss << "This flag requires an argument!";
				return Output::new_err(ss.str());
			}

			return Output::new_ok("");
		}

		/**
		 * @brief Gets all the palindromes from the source file, and ereses the repeated values.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a structure of found palindromes
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto words_source = __Helpers::Regex::get_words(operations.source);
			auto words_flag = __Helpers::Regex::get_words(flag.arg);

			auto palindromes = Vec<String>();

			for (String& first : words_source) {
				for (String& second : words_flag) {
					if (__Helpers::Strings::are_palindromes(first, second)) {
						palindromes.push_back(first);
					}
				}
			}

			palindromes.erase(
				std::unique(palindromes.begin(), palindromes.end()),
				palindromes.end()
			);

			return Output::new_ok(
				__Helpers::Info::flag_string_stream_structure(flag, palindromes).str()
			);
		}
	};


	/**
	 * @brief Command responsible for getting all words from the source file.
	 */
	struct ShowWords : Command
	{
		static const String CALLER_VALUE;
		static const String ALIAS_VALUE;

		String caller() const override {
			return CALLER_VALUE;
		}

		String alias() const override {
			return ALIAS_VALUE;
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Gets all the words from the source file, and sorts it.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a structure of found words
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto words = __Helpers::Regex::get_words(operations.source);

			std::sort(
				words.begin(),
				words.end(),
				__Helpers::Comparators::get_default<String>(flag.mod == 1)
			);

			return Output::new_ok(
				__Helpers::Info::flag_string_stream_structure(flag, words).str()
			);
		}
	};


	/**
	 * @brief Command responsible for getting all words in reverse order from the source file.
	 */
	struct ShowWordsReverse : Command {
		static const String CALLER_VALUE;
		static const String ALIAS_VALUE;

		String caller() const override {
			return CALLER_VALUE;
		}

		String alias() const override {
			return ALIAS_VALUE;
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Gets all the words in reverse order from the source file, and sorts it.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a structure of found words
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto words = __Helpers::Regex::get_words(operations.source);

			std::sort(
				words.begin(),
				words.end(),
				__Helpers::Comparators::get_reverse<String>(flag.mod == 1)
			);

			return Output::new_ok(
				__Helpers::Info::flag_string_stream_structure(flag, words).str()
			);
		}
	};


	/**
	 * @brief Command responsible for showing a size of the source file in specific units.
	 */
	struct ShowFileSize : Command {
		String units[4] = { "B", "KB", "MB", "GB" };

		String caller() const override {
			return "-si";
		}

		String alias() const override {
			return "--size";
		}

		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			return Output::new_ok("");
		}

		/**
		 * @brief Gets a size of the source file and calculates the best unit for representation.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param operations - Struct holding operational data
		 * @return Output with a size of the source file
		 */
		Output execute(const Flag& flag, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			auto unit = String();
			f32 size = File::get_size(operations.file_in);

			for (const String& _unit : units) {
				if (size >= 1000) {
					size /= 1000;
				}
				else {
					unit = _unit;
					break;
				}
			}

			f32 size_rounding = (i32)(size * 100 + 0.5);
			size = (f32)size_rounding / 100;

			ss << size << " " << unit;

			return Output::new_ok(ss.str());
		}
	};

	const String ShowWords::CALLER_VALUE = "-s";
	const String ShowWords::ALIAS_VALUE = "--sorted";
	const String ShowWordsReverse::CALLER_VALUE = "-rs";
	const String ShowWordsReverse::ALIAS_VALUE = "--reverse-sorted";
}


/**
 * @brief Modifying commands changing functionality of the other commands.
 * • Flexible.
 * • Can be modified.
 * • Have to be added manually by the Engine::add function.
 */
namespace ModifyingCommands {

	/**
	 * @brief Command responsible for modifying ShowWords and ShowWordsReverse commands by changing their sort target to .size() method.
	 */
	struct WordsConsiderLength : Command
	{
		const String CALLER_VALUE = "-l";
		const String ALIAS_VALUE = "--by-length";

		String caller() const override {
			return "-l";
		}

		String alias() const override {
			return "--by-length";
		}

		/**
		 * @brief Checks if the next flag is a ShowWords or ShowWordsReverse and changes it's mod value to 1.
		 *
		 * @param flag - Flag instance of this specific command
		 * @param inst - Instruction with all the Flags
		 * @param operations - Struct holding operational data
		 * @return Output(Error) - If the next flag doesn't exists, or isn't a ShowWords or ShowWordsReverse
		 * @return Output(Ok) - If the next flag is the same, or if modification succeeded
		 */
		Output validate(const Flag& flag, Instruction& inst, Operations& operations) const override {
			auto ss = __Helpers::Info::flag_string_stream(flag);

			Flag* flag_after_ptr = inst.get_flag_ptr(flag.pos + 1);

			WordsConsiderLength::ALIAS_VALUE;

			if (flag_after_ptr != nullptr) {
				if (flag_after_ptr->name_in({
					CALLER_VALUE,
					ALIAS_VALUE
					})) {
					return Output::new_ok("");
				}

				if (!flag_after_ptr->name_in({
					OperationalCommands::ShowWordsReverse::CALLER_VALUE,
					OperationalCommands::ShowWordsReverse::ALIAS_VALUE,
					OperationalCommands::ShowWords::CALLER_VALUE,
					OperationalCommands::ShowWords::ALIAS_VALUE
					})) {
					ss << "Missing required flag after this one!";
					return Output::new_err(ss.str());
				}
				else {
					flag_after_ptr->mod = 1;
				}
			}
			else {
				ss << "This flag can't be the last one!";
				return Output::new_err(ss.str());
			}

			return Output::new_ok("");
		}

		Output execute(const Flag& flag, Operations& operations) const override {
			return Output::new_ok("");
		}
	};
}
