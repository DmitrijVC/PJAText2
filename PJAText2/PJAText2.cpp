#include <iostream>

#include "Engine.h"
#include "app_commands.h"


/**
 * @brief Creates an Engine object with all the Commands included
 * 
 * @return Engine object
 */
Engine create_engine() {
    Engine engine = Engine();
    engine.add(OperationalCommands::CountChars())
        ->add(OperationalCommands::CountDigits())
        ->add(OperationalCommands::CountLines())
        ->add(OperationalCommands::CountNumbers())
        ->add(OperationalCommands::CountWords())
        ->add(OperationalCommands::ShowAnagrams())
        ->add(OperationalCommands::ShowFileSize())
        ->add(OperationalCommands::ShowPalindromes())
        ->add(OperationalCommands::ShowWords())
        ->add(OperationalCommands::ShowWordsReverse())
        ->add(ModifyingCommands::WordsConsiderLength());

    return engine;
}


/**
 * @brief Executes the Engine on flags provided from the arguments
 *
 * @param argc - amount of arguments
 * @param argv - arguments
 * 
 * @return int
 */
int main(int argc, char** argv)
{
    Vec<String> args = Vec<String>();
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    auto engine = create_engine();
    auto res = engine.execute(args);

    std::cout << res;

    return 0;
}
