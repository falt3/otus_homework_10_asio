#include "interpret.h"
#include <iostream>

/**
 * @brief Функция обработки входной строки в команды
 * 
 * @param[in] line     строка команд
 */
void Interpretator::input(std::string& line) 
{
    if (line == "{") {
        if (dinamicBlock == 0 && block->size() > 0) {
            // непредвиденное окончание обычного блока и начало динамического
            notify(block);
            block.reset(new BlockCommands());
        }
        dinamicBlock++;
    }
    else if (line == "}") {
        if (dinamicBlock == 0) 
            return;
        dinamicBlock--;
        if (dinamicBlock == 0) {
            if (block->size() > 0) {
                notify(block);
                block.reset(new BlockCommands());
            }
        }
    }
    else {
        block->addCommand(line);
        if (dinamicBlock == 0) {
            if (block->size() == maxSize) {
                notify(block);
                block.reset(new BlockCommands());
            }
        }
    }
}