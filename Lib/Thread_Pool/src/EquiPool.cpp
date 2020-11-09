/**
* Author:    Andrea Casalino
* Created:   25.09.2020
*
* report any bug to andrecasa91@gmail.com.
**/

#include "../header/EquiPool.h"
#include "QueueStrategy.h"

namespace thpl::equi{

    class Pool::EquiQueue : public IPool::QueueStrategy {
    public:
        inline bool isEmpty() override{
            return this->queue.empty();
        };

        inline std::function<void(void)> pop() override{
            std::function<void(void)> temp = std::move(this->queue.front());
            this->queue.pop_front();
            return temp;
        };

        void push(const std::function<void(void)>& task){
            this->queue.push_back(task);
        };

    private:

        std::list<std::function<void(void)>> queue;

    };

    Pool::Pool(const std::size_t& poolSize)
        : IPool(poolSize, new EquiQueue()) {
        this->Q = static_cast<EquiQueue*>(this->getQueue());
    }

    Pool::~Pool(){
        delete this->Q;
    }

    void Pool::push(const std::function<void(void)>& newTask){
        {
            std::lock_guard<std::mutex>(this->insertionMtx);
            this->Q->push(newTask);
        }
        this->notifyTaskInsertion();
    }

}