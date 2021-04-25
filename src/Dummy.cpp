
#include <algorithm>
#include <iostream>
#include <type_traits>

#include "Types.h"
#include "SimpleMatchingEngine.h"

using namespace OrderMatchingEngine;

int main (int argc, char **argv) {

    SimpleMatchingEngine simpleMatchingEngine;

    OrderData bidOrder = {"bid01", Side::Bid, OrderType::Limit, 100, 10};
    simpleMatchingEngine.submitNewOrder(bidOrder);

    auto ret = simpleMatchingEngine.getResultsOfLastOperation();
    /* expect an order ack */
    if (ret.size() <= 0 || ret[0].executionType != ExecutionType::New) {
        std::cout << "new bid order failed" << std::endl;
        return -1;
    }

    /* bid order placement success, record exchange id for later cancel */
    auto bidOrderExchangeId = ret[0].exchangeId;

    /* submit another ask order for fill */
    OrderData askOrder = {"ask01", Side::Ask, OrderType::Market, 0, 4};
    simpleMatchingEngine.submitNewOrder(askOrder);
    ret = simpleMatchingEngine.getResultsOfLastOperation();

    /* expect a ask order ack and 2 trades */
    if (ret.size() != 3 || ret[0].executionType != ExecutionType::New || ret[1].executionType != ExecutionType::Trade || ret[2].executionType != ExecutionType::Trade) {
        std::cout << "new ask order for fill failed" << std::endl;
        return -1;
    }

    /* filled as expected, cancel the rest bid order */
    simpleMatchingEngine.submitCancel(bidOrderExchangeId);
    ret = simpleMatchingEngine.getResultsOfLastOperation();

    /* expect a cancel ack */
    if (ret.size() != 1 || ret[0].executionType != ExecutionType::Canceled) {
        std::cout << "cancel bid order failed" << std::endl;
        return -1;
    }

    /* issue a duplicated cancel for reject */
    simpleMatchingEngine.submitCancel(bidOrderExchangeId);
    ret = simpleMatchingEngine.getResultsOfLastOperation();

    /* expect a cancel reject */
    if (ret.size() != 1 || ret[0].executionType != ExecutionType::Rejected) {
        std::cout << "cancel reject failed" << std::endl;
        return -1;
    }


    std::cout << "Success!" << std::endl;
    return 0;
}

