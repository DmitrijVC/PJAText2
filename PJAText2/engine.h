#pragma once

#include "type_aliases.h"
#include "instruction.h"
#include "command.h"
#include "file_operations.cpp"
#include "app_commands.h"


/**
 * @brief Core of the project.
 * Modular flag engine holding Flag specific commands, it's outputs, and fumctional structure.
 * Each execution is done on vector of Strings which is parsed into the Instruction class and then each Flag's command functionality validated and executed.
 */
class Engine {
private:
	CommandsHolder commands;
	Vec<Output> outputs;
	Operations operations;

	/**
	 * @brief Clears the Engine to prepare it for another execution.
	 */
	void clear() {
		outputs.clear();
		operations = Operations();
	}

	/**
	 * @brief Transforms all the Outputs into one String and returns it (or saves into an output file) and then cleans the Engine.
	 *
	 * @return All the Command's Outputs as an one String, or empty value if it has been saved into an output file
	 */
	String grab_output() {
		StringStream output_stream;

		for (Output output : outputs) {
			String message = output.get_message();

			if (message.empty()) {
				continue;
			}

			String result = "";
			if (output.is_ok()) {
				result = "[SUCCESS]";
			}
			else {
				result = "[ERROR]";
			}

			output_stream << result << ": " << message << "\n";
		}

		if (!operations.file_out.empty()) {
			File::write_unchecked(operations.file_out, output_stream.str());
			return "";
		}

		clear();

		return output_stream.str();
	}

	/**
	 * @brief Adds all the core commands
	 */
	void add_base_commands() {
		this->add(BaseCommands::SourceFile())
			->add(BaseCommands::InputFile())
			->add(BaseCommands::OutputFile());
	}

public:
	Engine() {
		commands = CommandsHolder();
		outputs = Vec<Output>();
		operations = Operations();

		add_base_commands();
	}

	/**
	 * @brief Creates a static copy of the provided Command's child and adds it's pointer into the Engine.
	 * If a specific Command exists or is invalid, it's being ignored.
	 *
	 * @tparam C - Type of the Command's child
	 * @param command_generic - Command's child
	 * @return Engine pointer
	 */
	template <typename C>
	Engine* add(C command_generic) {
		static C command = command_generic;
		Command* command_ptr = (Command*)(&command);

		if (command_ptr == nullptr) {
			return this;
		}

		if (commands.exists(
			command_ptr->caller(),
			command_ptr->alias()
		)) {
			return this;
		}

		commands.add(command_ptr);

		return this;
	}

	/**
	 * @brief Creates an Instruction out of Vec<String>, checks if the Flags are valid, executes the logic behind them and returns their Output.
	 *
	 * @param raw_args - Vector of Strings that holds flags and their arguments
	 * @return All the Flags outputs as a one String object
	 */
	String execute(const Vec<String>& raw_args) {
		auto inst = Instruction::from_vec_string(raw_args);
		auto validated_commands = HashMap<Command*, Flag>();

		if (inst.flag_exists(
			BaseCommands::InputFile::CALLER_VALUE,
			BaseCommands::InputFile::ALIAS_VALUE
		)) {
			if (inst.size() != 1) {
				outputs.push_back(
					Output::new_err("<ENGINE> Input file flag should be the only one!")
				);

				return grab_output();
			}

			auto flag_ptr = inst.get_flag_ptr(0);
			if (flag_ptr->is_empty()) {
				outputs.push_back(
					Output::new_err("<ENGINE> Input file flag requires an argument!")
				);

				return grab_output();
			}

			if (!File::exists(flag_ptr->arg)) {
				outputs.push_back(
					Output::new_err("<ENGINE> Input file flag has invalid file as an argument!")
				);

				return grab_output();
			}

			inst = Instruction::from_vec_string(
				__Helpers::Regex::get_words(
					File::read_unchecked(flag_ptr->arg)
				)
			);
		}

		for (Flag flag : inst.get_flags()) {
			Option<Command*> command_o = commands.get_by_caller(flag.name);

			if (command_o.is_none()) {
				command_o = commands.get_by_alias(flag.name);
			}

			if (command_o.is_some()) {
				auto command = command_o.get_value();

				auto output = command->validate(flag, inst, operations);
				if (output.is_err()) {
					outputs.push_back(
						output
					);

					operations.is_panicked = true;
					break;
				}

				validated_commands[command] = flag;

			}
			else {
				outputs.push_back(
					Output::new_err("<ENGINE> Invalid flag: [" + flag.name + "]")
				);

				operations.is_panicked = true;
				break;
			}
		}

		if (operations.file_in.empty() && operations.source.empty()) {
			outputs.push_back(
				Output::new_err("<ENGINE> Source file is invalid!")
			);

			operations.is_panicked = true;
		}

		if (operations.is_panicked) {
			return grab_output();
		}

		for (auto& pair : validated_commands) {
			auto output = pair.first->execute(pair.second, operations);

			if (!output.get_message().empty()) {
				outputs.push_back(output);
			}
		}

		return grab_output();
	}
};
