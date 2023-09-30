#include "pool.h"


/**
 * @brief Construct a new PoolThread::PoolThread object
 * 
 * @param [in] countThreads количество потоков, которые нужно создать
 * @param [in] f            функция для обработки задачи
 */
PoolThread::PoolThread(int countThreads, FuncThread f)
{
    m_func = f;
    m_threads.reserve(countThreads);
    for (int i = 0; i < countThreads; ++i) 
        m_threads.emplace_back(&PoolThread::worker, this, i);
}


/**
 * @brief Функция добавления нового блока команд в очередь
 * 
 * @param [in] block    новый блок команд
 */
void PoolThread::update(std::shared_ptr<BlockCommands> &block)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_blocks.push(block);
    m_cv.notify_one();
}


/**
 * @brief Функция завершения потоков
 */
void PoolThread::exit()
{
    m_flagExit = 1;
    m_cv.notify_all();

    for (std::size_t i = 0; i < m_threads.size(); ++i) {;
        m_threads[i].join();
    }
}


/**
 * @brief Рабочая функция потоков
 * 
 * @param [in] id   идентификатор потока
 */
void PoolThread::worker(int id)
{
    while(true) {        
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]() { return !m_blocks.empty() || m_flagExit; } );

        if (!m_blocks.empty()) 
        {
            auto block = m_blocks.front();
            m_blocks.pop();
            lock.unlock();

            m_func(block, id);

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);
        }
        else if (m_flagExit) break;
    }
}
