#pragma once

#include <iostream>
#include <memory>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "interpret.h"



namespace ba = boost::asio;
using tcp = ba::ip::tcp;


//-----------------------------------------------------------

class InterpretatorDynamic : public Interpretator {
public:
    InterpretatorDynamic(int maxSize) : Interpretator(maxSize) {}
    int inputDynamic(std::string& line) {
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

//-----------------------------------------------------------

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(tcp::socket&& socket, std::unique_ptr<InterpretatorDynamic> interD,
        std::shared_ptr<Interpretator> inter) : 
        m_socket(std::move(socket)),
        m_interpretDyn(std::move(interD)),
        m_interpret(inter)
    { 
        std::cout << "Connection: constructor\n";
    }
    // ~Connection() {}
    void read() {
        ba::async_read_until(m_socket, m_buffer, "\n",
            [self=shared_from_this()](boost::system::error_code err, std::size_t length){
                self->readyRead(err, length);
            }   
        );
        // m_socket.async_receive()
    }
   
private:
    void readyRead(boost::system::error_code err, std::size_t length) {
        if (err) {
            std::cout << "readyRead error: " << err.value() << " " << err.message()
                << " : " << err.category().name() << "\n";
            return;
        }
        if (length > 0) {
            std::string str{ba::buffer_cast<const char *>(m_buffer.data()), length-1};
            // std::cout << "read " << length << " " << str << "\n";
            if (m_interpretDyn->inputDynamic(str) == 1) {
                std::cout << "staticBlock " << str << "\n";
                m_interpret->input(str);
            }
        }
        m_buffer.consume(length);

        read();
    };
    tcp::socket m_socket;
    ba::streambuf m_buffer;
    std::unique_ptr<InterpretatorDynamic> m_interpretDyn;
    std::shared_ptr<Interpretator> m_interpret;
};