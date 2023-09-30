/**
 * @file main.cpp
 * @author Lipatkin Dmitry
 * @brief 
 * @version 0.1
 * @date 2023-09-30
 * 
 * @copyright Copyright (c) 2023
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "pool.h"
#include "connection.h"

//-----------------------------------------------------------

void server(tcp::acceptor &acceptor, std::shared_ptr<Interpretator> interpS, int& countConnection) 
{
    // std::cout << "accept\n";
    acceptor.async_accept(
        [&acceptor, interpS, &countConnection] 
        (const boost::system::error_code& err,  tcp::socket socket) {
            // std::cout << "new connection: "<< socket.local_endpoint().address().to_string() 
            //           << ":" << socket.local_endpoint().port() << "\n";
            
            if (!err) {
                ++countConnection;

                /// ожидание завершения соединения
                socket.async_wait(tcp::socket::wait_error, 
                    [interpS, &countConnection](const boost::system::error_code& error) {
                        --countConnection;
                        if (error) {
                            // std::cout << "error: " << error.value() << " " << error.category().name()
                            //     << " " << error.message() << " " << "\n";
                            if (countConnection == 0) 
                                interpS->closeBlock();
                        }
                    }
                );

                // Новое соединение 
                const std::shared_ptr<Connection> connection { new Connection(std::move(socket)) };

                /// Обработчик для динамического блока
                auto interpD = std::shared_ptr<Interpretator>(new InterpretatorDynamic());
                interpD->addSubscribers(*interpS);    // копирование подписчиков из писателя

                connection->addInterpretator(interpD);
                connection->addInterpretator(interpS);
                connection->read();
            }

            server(acceptor, interpS, countConnection);
        }
    );
}

//----------------------------------------------------------------------------
/**
 * @brief Функция вывода в консоль
 */
void ff_console(std::shared_ptr<BlockCommands>& block, int /*id*/) 
{
    std::cout << "bulk: " << block->print() << "\n";    
};


/**
 * @brief Функция вывода в файл
 */
void ff_file(std::shared_ptr<BlockCommands>& block, int id) 
{
    std::ostringstream fileName;
    fileName << "./bulk" << block->time() << "_" << id << ".log";
    std::fstream fs(fileName.str(), std::fstream::app);
    if (fs.is_open()) {
        fs << "bulk: " << block->print() << "\n";
        fs.close();
    }
};

//-----------------------------------------------------------


int main(int argc, const char* argv[]) 
{    
    int port = 9000;
    int sizeBlock = 3;
    if (argc > 1)
        port = std::stoi(argv[1]);
    if (argc > 2) 
        sizeBlock = std::stoi(argv[2]);
    // std::cout << "port: " << port << " size: " << sizeBlock << std::endl;

    /// количество текущих соединений
    int countConnection = 0;

    /// Пул потоков (1 поток) для вывода в консоль с очередью
    auto poolThreadConsole = std::shared_ptr<PoolThread>(new PoolThread(1, ff_console));
    /// Пул потоков (2 потока) для записи в файл с очередью
    auto poolThreadFile = std::shared_ptr<PoolThread>(new PoolThread(2, ff_file));
    /// Обработчик для статического блока, он один на все соединения
    auto interpret = std::shared_ptr<Interpretator>(new InterpretatorStatic(sizeBlock));
    interpret->addSubscriber(poolThreadConsole);
    interpret->addSubscriber(poolThreadFile);

    ba::io_context io_context;
    tcp::acceptor acceptor {io_context, tcp::endpoint(tcp::v4(), port)};

    server(acceptor, interpret, countConnection);

    io_context.run();


    poolThreadConsole->exit();
    poolThreadFile->exit();

    return 0;
}