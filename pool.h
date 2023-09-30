#pragma once

#include <iostream>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>

#include "interpret.h"
#include "observer.h"

/**
 * @brief Класс управления потоками и общей для них очереью задач
 * 
 */
class PoolThread : public Subscriber<BlockCommands> {
public:
    using FuncThread = std::function<void(std::shared_ptr<BlockCommands>&, int)>;

    PoolThread(int countThreads, FuncThread f);

    void update(std::shared_ptr<BlockCommands>& block) override;    
    void exit();

private:
    std::mutex m_mutex;                 ///< мьютекс для синхронизации доступа к очереди задач
    std::condition_variable m_cv;       ///< для управления потока (приостановить/запустить)
    std::queue<std::shared_ptr<BlockCommands>> m_blocks;    ///< очередь задач
    int m_flagExit  = 0;                ///< флаг для завершения работы потоков
    FuncThread m_func;                  ///< функция вызова для обработки задачи
    
    std::vector<std::thread> m_threads;
    void worker(int id);                ///< функция потока
};
