#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "interpret.h"




namespace ba = boost::asio;
using tcp = ba::ip::tcp;


//-----------------------------------------------------------
/**
 * @brief Класс обработчика динамического блока
 * 
 */
class InterpretatorDynamic : public Interpretator {
public:
    InterpretatorDynamic() : Interpretator(1) {}
    int input(std::string& line) override 
    {
        if (line == "{") {
            dinamicBlock++;
        }
        else if (line == "}") {
            if (dinamicBlock == 0) 
                return -1;
            dinamicBlock--;
            if (dinamicBlock == 0) {
                if (block->size() > 0) {
                    notify(block);
                    block.reset(new BlockCommands());
                }
            }
        }
        else {
            if (dinamicBlock > 0) 
                block->addCommand(line);
            else 
                return 1;
        }        
        return 0;
    }
};

/**
 * @brief Класс обработчика статического блока
 * 
 */
class InterpretatorStatic : public Interpretator {
public:
    InterpretatorStatic(int sizeBlock) : Interpretator(sizeBlock) {}
    int input(std::string& line) override 
    {
        block->addCommand(line);
        if (block->size() == m_sizeBlock) {            
            notify(block);
            block.reset(new BlockCommands());
        }
        return 0;
    }
};

//-----------------------------------------------------------
/**
 * @brief Класс соединения с клиентом
 * 
 */
class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(tcp::socket&& socket) : m_socket(std::move(socket)) { m_interpretators.reserve(2); }
    void addInterpretator(std::shared_ptr<Interpretator> inter) {
        m_interpretators.push_back(inter);
    }
    void read() {
        ba::async_read_until(m_socket, m_buffer, "\n",
            [self=shared_from_this()](boost::system::error_code err, std::size_t length){
                self->readyRead(err, length);
            }   
        );
    }
   
private:
    void readyRead(boost::system::error_code err, std::size_t length) {
        if (err) { // закрытие сокета
            // std::cout << "readyRead error: " << err.value() << " " << err.message()
            //     << " : " << err.category().name() << "\n";
            return;
        }
        if (length > 0) {
            std::string str{ba::buffer_cast<const char *>(m_buffer.data()), length-1};
            for (auto& el : m_interpretators) {
                if (el->input(str) == 0) break;
            }
        }
        m_buffer.consume(length);

        read();
    };
    tcp::socket m_socket;
    ba::streambuf m_buffer;
    std::vector<std::shared_ptr<Interpretator>> m_interpretators;
};