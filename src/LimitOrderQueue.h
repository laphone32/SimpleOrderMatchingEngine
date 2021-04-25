#ifndef __ORDERMATCHINGENGINE_LIMITORDERQUEUE
#define __ORDERMATCHINGENGINE_LIMITORDERQUEUE

#include <functional>
#include <set>
#include <utility>
#include "Order.h"
#include "Types.h"

namespace OrderMatchingEngine {

template <typename PriceCompare>
class LimitOrderQueue {
public:
    LimitOrderQueue(const PriceCompare &priceCompare_) : orderCompare(priceCompare_), queue(orderCompare) {}

    auto size() {
        return queue.size();
    }

    Order *best() {
        if (size() > 0){
            return *(queue.begin());
        } else {
            return nullptr;
        }
    }

    auto insert(Order &order) {
        queue.insert(&order);
    }

    auto remove(Order &order) {
        queue.erase(&order);
    }

private:
    struct OrderCompare {
        PriceCompare priceCompare;

        OrderCompare(const PriceCompare &priceCompare_) : priceCompare(priceCompare_) {};

        bool operator() (const Order *lhs, const Order *rhs) {
            auto &lOrderData = lhs->orderData;
            auto &rOrderData = rhs->orderData;

            auto priceCompareRet = priceCompare(lOrderData.price, rOrderData.price);
            if (!priceCompareRet) {
                return lhs->exchangeId < rhs->exchangeId;
            } else {
                return priceCompareRet > 0;
            }

            return false;
        }
    } orderCompare;


    std::set<Order *, decltype(orderCompare)> queue;
};

}
#endif
