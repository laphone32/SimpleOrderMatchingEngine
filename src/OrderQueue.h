#ifndef __ORDERMATCHINGENGINE_ORDERQUEUE
#define __ORDERMATCHINGENGINE_ORDERQUEUE

#include "Order.h"
#include "Types.h"
#include "LimitOrderQueue.h"
#include "MarketOrderQueue.h"

namespace OrderMatchingEngine {

template <typename PriceCompare>
class OrderQueue {
public:
    OrderQueue(const PriceCompare &priceCompare_, PriceType defaultPrice_) : priceCompare(priceCompare_), lastPrice(defaultPrice_), limit(priceCompare_) {}

    auto size() {
        return market.size() + limit.size();
    }

    Order *best() {
        auto *ret = market.best();
        if (ret == nullptr) {
            ret = limit.best();
        }

        return ret;
    }

    auto insert(Order &order) {
        switch (order.orderData.orderType) {
        case OrderType::Limit:
            limit.insert(order);
            break;
        case OrderType::Market:
            market.insert(order);
            break;
        default:
            break;
        }
    }

    auto fill(Order &order, ResultsType &results) {
        switch (order.orderData.orderType) {
        case OrderType::Limit:
            tryFill(order, market, results, [&](auto &&){
                lastPrice = order.orderData.price;
                return true;
            });
            tryFill(order, limit, results, [&](auto &&stackOrder){
                auto ret = priceCompare(order.orderData.price, stackOrder.orderData.price) <= 0;
                if (ret) {
                    lastPrice = order.orderData.price;
                }
                return ret;
            });
            break;
        case OrderType::Market:
            tryFill(order, market, results, [&](auto &&){
                return true;
            });
            tryFill(order, limit, results, [&](auto &&stackOrder){
                lastPrice = stackOrder.orderData.price;
                return true;
            });
            break;
        default:
            break;
        }
    }

    auto remove(Order &order) {
        switch (order.orderData.orderType) {
        case OrderType::Limit:
            limit.remove(order);
            break;
        case OrderType::Market:
            market.remove(order);
            break;
        default:
            break;
        }
    }

private:
    template <typename F, typename Queue>
    auto tryFill(Order &order, Queue &&queue, ResultsType &results, F &&f) {
        while (order.leftQuantity > 0) {
            auto *bestStackOrder = queue.best();
            if (bestStackOrder != nullptr) {
                auto &stackOrder = *bestStackOrder;
                if (std::forward<F>(f)(stackOrder)) {
                    doFill(stackOrder, order, results);
                } else {
                    break;
                }

                if (stackOrder.leftQuantity <= 0) {
                    queue.remove(stackOrder);
                }
            } else {
                break;
            }
        }
    }

    auto doFill(Order &order1, Order &order2, ResultsType &results) {
        auto fillQuantity = std::min(order1.leftQuantity, order2.leftQuantity);
        order1.fill(fillQuantity);
        order2.fill(fillQuantity);
        results.emplace_back(order1.orderData, ExecutionType::Trade, order1.exchangeId, lastPrice, fillQuantity);
        results.emplace_back(order2.orderData, ExecutionType::Trade, order2.exchangeId, lastPrice, fillQuantity);
    }


    PriceCompare priceCompare;
    PriceType lastPrice;
    LimitOrderQueue<PriceCompare> limit;
    MarketOrderQueue market;
};

}
#endif
