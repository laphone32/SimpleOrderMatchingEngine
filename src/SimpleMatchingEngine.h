#ifndef __ORDERMATCHINGENGINE_SIMPLEMATCHINGENGINE
#define __ORDERMATCHINGENGINE_SIMPLEMATCHINGENGINE

#include <unordered_map>

#include "Types.h"
#include "MatchingEngine.h"
#include "OrderBook.h"
#include "Results.h"

namespace OrderMatchingEngine {

class SimpleMatchingEngine : public MatchingEngine {
public:
    SimpleMatchingEngine(PriceType openPrice = 0) : orderBook(openPrice) {};

    void submitNewOrder(const OrderData &orderData) override {
        resultsOfLastOperation.clear();

        if ((orderData.orderType != OrderType::Limit && orderData.orderType != OrderType::Market)
            || (orderData.side != Side::Bid && orderData.side != Side::Ask)
                ) {
            resultsOfLastOperation.append(ExecutionType::Rejected, orderData);
        } else {
            auto *order = new Order(orderData);
            order->exchangeId = ++ lastExchangeId;
            exchangeIdMap.insert({order->exchangeId, order});
            resultsOfLastOperation.append(ExecutionType::New, orderData, order->exchangeId);
            orderBook.newOrder(*order, resultsOfLastOperation);
        }
    }

    void submitCancel(ExchangeIdType exchangeId) override {
        resultsOfLastOperation.clear();

        auto found = exchangeIdMap.find(exchangeId);
        if (found == exchangeIdMap.end()) {
            resultsOfLastOperation.append(ExecutionType::Rejected, OrderData(), exchangeId);
        } else {
            auto &order = *(found->second);
            if (order.leftQuantity <= 0) {
                resultsOfLastOperation.append(ExecutionType::Rejected, order);
            } else {
                order.cancelAck();
                orderBook.cancel(order);
                resultsOfLastOperation.append(ExecutionType::Canceled, order);
            }
        }
    }

    ResultsType getResultsOfLastOperation() override {
        return resultsOfLastOperation.toResultsType();
    }

private:
    ExchangeIdType lastExchangeId = 0;
    std::unordered_map<ExchangeIdType, Order *> exchangeIdMap;

    OrderBook orderBook;

    Results resultsOfLastOperation;
};

}

#endif
