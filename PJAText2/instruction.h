#pragma once

#include "type_aliases.h"
#include "wrappers.h"
#include "operations.h"


/**
 * @brief Result enum of the Output class
 * Ok - Output success
 * Err - Output error
 * Undefined - Output should be ignored
 */
enum class Result {
	Ok,
	Err,
	Undefined
};


/**
 * @brief Wrapper around the Command's result message.
 * Holds a Result enum indicating in success or failure and result message as a String.
 */
struct Output {
private:
	Result res;
	String msg;

	Output(const Result& result, const String& message) {
		res = result;
		msg = message;
	}

public:
	/**
	 * @brief Constructs a new empty Output object
	 */
	Output() {
		res = Result::Undefined;
		msg = "";
	}

	/**
	 * @brief Constructs a new Output object with a message and success result
	 *
	 * @param message - result message
	 * @return Output object
	 */
	static Output new_ok(const String message) {
		return Output(Result::Ok, message);
	}

	/**
	 * @brief Constructs a new Output object with a message and error result
	 *
	 * @param message - result message
	 * @return Output object
	 */
	static Output new_err(const String message) {
		return Output(Result::Err, message);
	}

	/**
	 * @brief Checks if the Output is holding a success message
	 *
	 * @return true
	 * @return false
	 */
	bool is_ok() {
		return res == Result::Ok;
	}

	/**
	 * @brief Checks if the Output is holding a error message
	 *
	 * @return true
	 * @return false
	 */
	bool is_err() {
		return res == Result::Err;
	}

	/**
	 * @brief Checks if the Output is empty
	 *
	 * @return true
	 * @return false
	 */
	bool is_undefined() {
		return res == Result::Undefined;
	}

	/**
	 * @brief Gets the message
	 *
	 * @return Result message
	 */
	String get_message() {
		return msg;
	}
};


/**
 * @brief Flag structure for the Instruction class.
 * It holds the name of the flag provided to the Instruction,
 * argument after the flag's name,
 * position of the flag relative to the other flags,
 * and the mod value which is reserved for changing the functionality
 */
struct Flag {
	String name;
	String arg;
	usize pos;
	i32 mod = 0;

	Flag(const String name, const String argument, usize position) {
		this->name = name;
		arg = argument;
		pos = position;
	}

	Flag(const String name, usize position) {
		Flag(name, "", position);
	}

	Flag() {
		Flag("", "", SIZE_MAX);
	}

	/**
	 * @brief Checks if the argument was not provided
	 *
	 * @return true
	 * @return false
	 */
	bool is_empty() {
		return arg.empty();
	}

	/**
	 * @brief Checks if the argument was provided
	 *
	 * @return true
	 * @return false
	 */
	bool exists() {
		return !name.empty();
	}

	/**
	 * @brief Removes the tailing white space from the argument
	 */
	void parse() {
		if (!arg.empty()) {
			arg.pop_back();
		}
	}

	/**
	 * @brief Checks if one of the provided names equals to the flag's name
	 *
	 * @param names - Vector of strings with the names to check
	 * @return true - If the Flag's name was in the names
	 * @return false - If the Flag's name was not in the names
	 */
	bool name_in(const Vec<String>& names) {
		for (const String& name_outer : names) {
			if (name == name_outer) return true;
		}

		return false;
	}
};


/**
 * @brief Class for parsing and holding the raw arguments into Flag objects
 */
struct Instruction {
private:
	Vec<Flag> flags;

	Instruction(const Vec<Flag> flags) {
		this->flags = flags;
	}

public:
	/**
	 * @brief Creates the Instruction object from the vector of strings holding raw flags.
	 *
	 * @param vec_s - Vector of raw flags in String
	 * @return Instruction object
	 */
	static Instruction from_vec_string(const Vec<String> vec_s) {
		usize position = 0;
		Vec<Flag> flags = Vec<Flag>();
		Flag flag = Flag();

		for (int i = 0; i < vec_s.size(); i++) {
			String arg = vec_s.at(i);

			if (arg.empty()) continue;

			bool is_flag = arg.rfind("-", 0) == 0;

			if (is_flag) {
				if (flag.exists()) {
					flag.parse();
					flags.push_back(flag);
					flag = Flag();
				}

				flag.name = arg;
				flag.pos = position++;
			}
			else {
				if (flag.exists()) {
					flag.arg.append(arg).append(" ");
				}
			}

			if (i + 1 == vec_s.size()) {
				flag.parse();
				flags.push_back(flag);
			}
		}

		return Instruction(flags);
	}

	/**
	 * @brief Gets the flag object by it's name.
	 *
	 * @param name - name of the flag
	 * @return Option<Flag>(Some) - If the Flag was found
	 * @return Option<Flag>(None) - If the Flag was not found
	 */
	Option<Flag> get_flag(const String name) const {
		for (Flag flag : flags) {
			if (flag.name == name) {
				return Option<Flag>::some(flag);
			}
		}

		return Option<Flag>::none();
	}

	/**
	 * @brief Gets the flag object by it's position.
	 *
	 * @param index - target position
	 * @return Option<Flag>(Some) - If the Flag was found
	 * @return Option<Flag>(None) - If the Flag was not found
	 */
	Option<Flag> get_flag(const usize index) const {
		for (Flag flag : flags) {
			if (flag.pos == index) {
				return Option<Flag>::some(flag);
			}
		}

		return Option<Flag>::none();
	}

	/**
	 * @brief Gets the Flag pointer by it's position.
	 *
	 * @param index - target position
	 * @return Flag* - nullptr if not found
	 */
	Flag* get_flag_ptr(const usize index) {
		for (Flag& flag : flags) {
			if (flag.pos == index) {
				return &flag;
			}
		}

		return nullptr;
	}

	/**
	 * @brief Checks if the Flag exists in the Instruction by it's caller, or alias.
	 *
	 * @param caller - Flag's caller
	 * @param alias - Flag's alias
	 * @return true - If found
	 * @return false - If not found
	 */
	bool flag_exists(String caller, String alias) const {
		for (Flag flag : flags) {
			if (flag.name == caller || flag.name == alias) {
				return true;
			}
		}

		return false;
	}

	/**
	 * @brief Gets the amount of all Flags in the Instruction.
	 *
	 * @return number of flags in the Instruction
	 */
	usize size() const {
		return flags.size();
	}

	/**
	 * @brief Gets all the flags by the const reference.
	 *
	 * @return const Vec<Flag>&
	 */
	const Vec<Flag>& get_flags() const {
		return flags;
	}
};
