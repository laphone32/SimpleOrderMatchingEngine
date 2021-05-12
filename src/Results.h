#ifndef __ORDERMATCHINGENGINE_RESULTS
#define __ORDERMATCHINGENGINE_RESULTS

#include "Order.h"
#include "Types.h"

namespace OrderMatchingEngine {

class Results {
public:
    auto &operator[](unsigned long index) {
        return results[index];
    }

    auto size() {
        return results.size();
    }

    auto clear() {
        results.clear();
    }

    auto operator =(const ResultsType &results_) {
        clear();
        results = results_;
    }

    auto append(ExecutionType executionType, const OrderData &orderData, ExchangeIdType exchangeId = 0) {
        results.emplace_back(orderData, executionType, exchangeId);
    }

    auto append(ExecutionType executionType, const Order &order) {
        append(executionType, order.orderData, order.exchangeId);
    }

    auto fill(const Order &order, PriceType lastPrice, QuantityType fillQuantity) {
        results.emplace_back(order.orderData, ExecutionType::Trade, order.exchangeId, lastPrice, fillQuantity);
    }

    auto toResultsType() {
        return results;
    }

private:
    ResultsType results;
};

}

#endif
