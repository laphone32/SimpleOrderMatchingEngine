#ifndef __ORDERMATCHINGENGINE_ORDERBOOK
#define __ORDERMATCHINGENGINE_ORDERBOOK

#include "OrderQueue.h"
#include "Results.h"
#include "Types.h"

namespace OrderMatchingEngine {

class OrderBook {
public:
    OrderBook(PriceType openPrice) : bidOrderQueue(bidPriceCompare, openPrice), askOrderQueue(askPriceCompare, openPrice) {}

    auto newOrder(Order &order, Results &results) {
        switch (order.orderData.side) {
        case Side::Bid:
            askOrderQueue.fill(order, results);
            if (order.leftQuantity > 0) {
                bidOrderQueue.insert(order);
            }
            break;
        case Side::Ask:
            bidOrderQueue.fill(order, results);
            if (order.leftQuantity > 0) {
                askOrderQueue.insert(order);
            }
            break;
        default:
            break;
        }
    }

    auto cancel(Order &order) {
        switch (order.orderData.side) {
        case Side::Bid:
            bidOrderQueue.remove(order);
            break;
        case Side::Ask:
            askOrderQueue.remove(order);
            break;
        default:
            break;
        }
    }

private:

    struct {
        PriceType operator() (PriceType lhs, PriceType rhs) const {
            return lhs - rhs;
        };
    } bidPriceCompare;

    struct {
        PriceType operator() (PriceType lhs, PriceType rhs) const {
            return rhs - lhs;
        };
    } askPriceCompare;


    OrderQueue<decltype(bidPriceCompare)> bidOrderQueue;
    OrderQueue<decltype(askPriceCompare)> askOrderQueue;
};

}

#endif
