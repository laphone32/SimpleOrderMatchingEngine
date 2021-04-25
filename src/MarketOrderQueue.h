#ifndef __ORDERMATCHINGENGINE_MARKETORDERQUEUE
#define __ORDERMATCHINGENGINE_MARKETORDERQUEUE

#include <list>
#include "Order.h"
#include "Types.h"

namespace OrderMatchingEngine {

class MarketOrderQueue {
public:
    auto size() {
        return queue.size();
    }

    Order *best() {
        if (size() > 0) {
            return queue.front();
        } else {
            return nullptr;
        }
    }

    auto insert(Order &order) {
        queue.push_back(&order);
    }

    auto remove(Order &order) {
        queue.remove(&order);
    }

private:
    std::list<Order *> queue;
};

}
#endif
