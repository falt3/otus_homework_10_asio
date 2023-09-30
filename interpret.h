#pragma once

#include <string>
#include <fstream>
#include <functional>

#include "command.h"
#include "observer.h"


/**
 * @brief Класс перевода входных данных в блоки команд
 * 
 */
class Interpretator : public Publisher<BlockCommands> {
public:
    /**
     * @brief Construct a new Interpretator object
     * 
     * @param maxSize_ размер блока команд
     */
    Interpretator(int sizeBlock) : m_sizeBlock(sizeBlock) {
        block = std::make_shared<BlockCommands>();
    }
    ~Interpretator() {         
        if (block->size() != 0 && dinamicBlock == 0) 
            notify(block);
    }
    virtual int input(std::string& line);
    int size() { return m_sizeBlock; }
    void closeBlock() {
        if (block->size() != 0 && dinamicBlock == 0) {
            notify(block);
            block.reset(new BlockCommands());
        }
    }

protected:
    int dinamicBlock    = 0;    ///< уровень вложенности 
    std::shared_ptr<BlockCommands> block;   ///< блок команд
    int m_sizeBlock;                ///< максимальный размер блока команд
};

