#pragma once

#include "type_aliases.h"
#include "instruction.h"


/**
 * @brief Abstract class for creating Commands specified by certain Flag.
 */
struct Command {

	/**
	 * @brief Virtual method for getting the Flag's caller (ex: -f)
	 *
	 * @return Flag's caller
	 */
	virtual String caller() const = 0;

	/**
	 * @brief Virtual method for getting the Flag's alias (ex: --flag)
	 *
	 * @return Flag's alias
	 */
	virtual String alias() const = 0;

	/**
	 * @brief Virtual method that will execute in Engine on Flags validation process.
	 * Error Output will panic and end the validation process with the Output's message. (No output on empty message)
	 * Ok Output will be added to the Engine's Outputs. (Ignored on empty message)
	 *
	 * @return Output
	 */
	virtual Output validate(const Flag&, Instruction&, Operations&) const = 0;

	/**
	 * @brief Virtual method that will execute in Engine on Flags Execution process.
	 * Error Output won't end the execution process, but will be added to the Engine's Outputs. (Ignored on empty message)
	 * Ok Output will be added to the Engine's Outputs. (Ignored on empty message)
	 *
	 * @return Output
	 */
	virtual Output execute(const Flag&, Operations&) const = 0;
};


/**
 * @brief Class that is a collection of the Commands.
 */
class CommandsHolder {
private:
	HashMap<u32, String> callers;
	HashMap<u32, String> aliases;
	HashMap<u32, Command*> commands;
	u32 id;

public:
	CommandsHolder() {
		callers = HashMap<u32, String>();
		aliases = HashMap<u32, String>();
		commands = HashMap<u32, Command*>();
		id = 0;
	}

	/**
	 * @brief Adds a pointer of a Command and related Flag's caller and alias.
	 *
	 * @param caller
	 * @param alias
	 * @param command_ptr
	 */
	void add(Command* command_ptr) {
		callers[id] = command_ptr->caller();
		aliases[id] = command_ptr->alias();
		commands[id] = command_ptr;

		id++;
	}

	/**
	 * @brief Get the Command by it's alias.
	 *
	 * @param alias - Name of the Flag's alias
	 * @return Option<Command*>(Some) - If the command was found
	 * @return Option<Command*>(None) - If the command was not found
	 */
	Option<Command*> get_by_alias(const String& alias) {
		for (auto& pair : aliases) {
			if (pair.second == alias) {
				return Option<Command*>::some(commands[pair.first]);
			}
		}

		return Option<Command*>::none();
	}

	/**
	 * @brief Get the Command by it's caller.
	 *
	 * @param alias - Name of the Flag's caller
	 * @return Option<Command*>(Some) - If the command was found
	 * @return Option<Command*>(None) - If the command was not found
	 */
	Option<Command*> get_by_caller(const String& caller) {
		for (auto& pair : callers) {
			if (pair.second == caller) {
				return Option<Command*>::some(commands[pair.first]);
			}
		}

		return Option<Command*>::none();
	}

	/**
	 * @brief Checks if the Command exists by it's caller and alias.
	 *
	 * @param caller - Name of the Flag's caller
	 * @param alias - Name of the Flag's alias
	 * @return true - If Command was found
	 * @return false - If Command was not found
	 */
	bool exists(const String& caller, const String& alias) {
		bool found_caller = false;

		for (auto& pair : callers) {
			if (pair.second == caller) {
				found_caller = true;
			}
		}

		if (!found_caller) {
			return false;
		}

		for (auto& pair : aliases) {
			if (pair.second == alias) {
				return true;
			}
		}

		return false;
	}
};
