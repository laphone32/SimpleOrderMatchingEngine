#ifndef __ORDERMATCHINGENGINE_SIMPLEMATCHINGENGINE
#define __ORDERMATCHINGENGINE_SIMPLEMATCHINGENGINE

#include <unordered_map>

#include "Types.h"
#include "MatchingEngine.h"
#include "OrderBook.h"

namespace OrderMatchingEngine {

class SimpleMatchingEngine : public MatchingEngine {
public:
    SimpleMatchingEngine(PriceType openPrice = 0) : orderBook(openPrice) {};


    void submitNewOrder(const OrderData &orderData) override {
        resultsOfLastOperation.clear();

        if ((orderData.orderType != OrderType::Limit && orderData.orderType != OrderType::Market)
            || (orderData.side != Side::Bid && orderData.side != Side::Ask)
                ) {
            appendResult(ExecutionType::Rejected, orderData);
        } else {
            auto *order = new Order(orderData);
            order->exchangeId = ++ lastExchangeId;
            exchangeIdMap.insert({order->exchangeId, order});
            appendResult(ExecutionType::New, orderData, order->exchangeId);
            orderBook.newOrder(*order, resultsOfLastOperation);
        }
    }

    void submitCancel(ExchangeIdType exchangeId) override {
        resultsOfLastOperation.clear();

        auto found = exchangeIdMap.find(exchangeId);
        if (found == exchangeIdMap.end()) {
            appendResult(ExecutionType::Rejected, OrderData(), exchangeId);
        } else {
            auto &order = *(found->second);
            if (order.leftQuantity <= 0) {
                appendResult(ExecutionType::Rejected, order);
            } else {
                order.cancelAck();
                orderBook.cancel(order);
                appendResult(ExecutionType::Canceled, order);
            }
        }
    }

    ResultsType getResultsOfLastOperation() override {
        return resultsOfLastOperation;
    }

private:
    void appendResult(ExecutionType executionType, const OrderData &orderData, ExchangeIdType exchangeId = 0) {
        resultsOfLastOperation.emplace_back(orderData, executionType, exchangeId);
    }

    void appendResult(ExecutionType executionType, const Order &order) {
        appendResult(executionType, order.orderData, order.exchangeId);
    }

    ExchangeIdType lastExchangeId = 0;
    std::unordered_map<ExchangeIdType, Order *> exchangeIdMap;

    OrderBook orderBook;

    ResultsType resultsOfLastOperation;
};

}

#endif
