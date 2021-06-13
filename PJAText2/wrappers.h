#pragma once

#include "type_aliases.h"


/**
 * @brief Wrapper to hold a value of some type, or not.
 * It can be (Some) - holds a value, or (None) - doesn't hold any value.
 *
 * @tparam T - Type of the value to hold
 */
template<typename T>
class Option {
private:
	Vec<T> value;

	Option(T value) {
		this->value = Vec<T>();
		this->value.push_back(value);
	}

	Option() {
		this->value = Vec<T>();
	}

public:
	/**
	 * @brief Creates an Option object with a certain value.
	 *
	 * @param value - value to hold
	 * @return Option object
	 */
	static Option some(T value) {
		return Option(value);
	}

	/**
	 * @brief Creates an empty Option object without a value.
	 *
	 * @return Option object
	 */
	static Option none() {
		return Option();
	}

	/**
	 * @brief Checks if the Option is not empty.
	 *
	 * @return true - If not empty
	 * @return false - If empty
	 */
	bool is_some() {
		return !is_none();
	}

	/**
	 * @brief Checks if the Option is empty.
	 *
	 * @return true - If empty
	 * @return false - If not empty
	 */
	bool is_none() {
		return value.empty();
	}

	/**
	 * @brief Gets the value, without checking
	 *
	 * @return Value of the specific type
	 */
	auto get_value() {
		return value[0];
	}
};
