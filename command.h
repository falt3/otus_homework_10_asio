#pragma once

#include <ctime>
#include <sstream>
#include <memory>
#include <vector>

/**
 * @brief Класс блока команд
 * 
 */
struct BlockCommands {
public:
    /**
     * @brief Функция представления блока команд в строку
     * 
     * @return [out] std::string    строка представления блока команд
     */
    std::string print() {
        std::string str;
        for (auto it = commands.begin(); it < commands.end(); ++it) {
            str += *it;

            if (it != commands.end() - 1)
                str += ", ";
        } 
        return str;       
    }

    /**
     * @brief Функция добавления команды в блок
     * 
     * @param [in] str  команда представлена в виде строки
     */
    void addCommand(std::string& str) {
        commands.push_back(std::move(str));
        if (commands.size() == 1)
            m_time = std::time(nullptr); 
    }

    /**
     * @brief Функция возвращает размер блока команд
     * 
     * @return [out] int размер блока
     */
    int size() {return commands.size();}

    /**
     * @brief Фукнция возвращает время создания блока
     * 
     * @return [out] std::time_t    время
     */
    std::time_t time() { return m_time; };

private:
    std::vector<std::string> commands;      ///< вектор комманд
    std::time_t m_time;                     ///< время записи первой команды в блок
};
