
#include "gtest/gtest.h"
#include "SimpleMatchingEngine.h"
#include "TestFixtures.h"
#include "Types.h"

using namespace OrderMatchingEngine;

class SimpleMatchingEngineTest : public TestFixtures {
protected:
    SimpleMatchingEngine simpleMatchingEngine;

    auto checkResultSize(size_t expectSize) {
        results = simpleMatchingEngine.getResultsOfLastOperation();
        ASSERT_EQ(results.size(), expectSize);
    }
};

TEST_F(SimpleMatchingEngineTest, init) {}

TEST_F(SimpleMatchingEngineTest, newOrder) {
    OrderData orderData{"orderData", Side::Bid, OrderType::Limit, 1000, 10};
    simpleMatchingEngine.submitNewOrder(orderData);
    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::New);
}

TEST_F(SimpleMatchingEngineTest, newOrderReject) {
    OrderData orderData;
    simpleMatchingEngine.submitNewOrder(orderData);
    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::Rejected);
}

TEST_F(SimpleMatchingEngineTest, cancelOrder) {
    OrderData orderData{"orderData", Side::Ask, OrderType::Limit, 1000, 10};
    simpleMatchingEngine.submitNewOrder(orderData);
    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::New);

    auto exchangeId = results[0].exchangeId;
    simpleMatchingEngine.submitCancel(exchangeId);

    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::Canceled);
}

TEST_F(SimpleMatchingEngineTest, cancelReject_UnknownExchangeId) {
    OrderData orderData{"orderData", Side::Ask, OrderType::Market, 100, 5};
    OrderData emptyOrderData;
    simpleMatchingEngine.submitNewOrder(orderData);
    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::New);

    auto exchangeId = results[0].exchangeId + 1;
    simpleMatchingEngine.submitCancel(exchangeId);
    checkResultSize(1);
    checkResponse(0, emptyOrderData, ExecutionType::Rejected);
}

TEST_F(SimpleMatchingEngineTest, cancelReject_DoubleCancel) {
    OrderData orderData{"orderData", Side::Bid, OrderType::Market, 200, 7};
    simpleMatchingEngine.submitNewOrder(orderData);
    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::New);

    auto exchangeId = results[0].exchangeId;
    simpleMatchingEngine.submitCancel(exchangeId);
    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::Canceled);

    simpleMatchingEngine.submitCancel(exchangeId);
    checkResultSize(1);
    checkResponse(0, orderData, ExecutionType::Rejected);
}

TEST_F(SimpleMatchingEngineTest, fill) {
    OrderData bidOrderData{"bidOrderData", Side::Bid, OrderType::Market, 200, 1};
    simpleMatchingEngine.submitNewOrder(bidOrderData);
    checkResultSize(1);
    checkResponse(0, bidOrderData, ExecutionType::New);

    OrderData askOrderData{"askOrderData", Side::Ask, OrderType::Limit, 100, 3};
    simpleMatchingEngine.submitNewOrder(askOrderData);
    checkResultSize(3);
    checkResponse(0, askOrderData, ExecutionType::New);
    checkResponse(1, bidOrderData, ExecutionType::Trade, 100, 1);
    checkResponse(2, askOrderData, ExecutionType::Trade, 100, 1);

    OrderData bidOrderData2{"bidOrderData2", Side::Bid, OrderType::Limit, 105, 1};
    simpleMatchingEngine.submitNewOrder(bidOrderData2);
    checkResultSize(3);
    checkResponse(0, bidOrderData2, ExecutionType::New);
    checkResponse(1, askOrderData, ExecutionType::Trade, 105, 1);
    checkResponse(2, bidOrderData2, ExecutionType::Trade, 105, 1);
}

TEST_F(SimpleMatchingEngineTest, cancelPartialFill) {
    OrderData bidOrderData{"bidOrderData", Side::Bid, OrderType::Limit, 200, 1};
    simpleMatchingEngine.submitNewOrder(bidOrderData);
    checkResultSize(1);
    checkResponse(0, bidOrderData, ExecutionType::New);

    OrderData askOrderData{"askOrderData", Side::Ask, OrderType::Limit, 100, 3};
    simpleMatchingEngine.submitNewOrder(askOrderData);
    checkResultSize(3);
    checkResponse(0, askOrderData, ExecutionType::New);
    checkResponse(1, bidOrderData, ExecutionType::Trade, 100, 1);
    checkResponse(2, askOrderData, ExecutionType::Trade, 100, 1);
    auto askExchangeId = results[0].exchangeId;
    simpleMatchingEngine.submitCancel(askExchangeId);

    checkResultSize(1);
    checkResponse(0, askOrderData, ExecutionType::Canceled);
}

TEST_F(SimpleMatchingEngineTest, cancelFullFilled) {
    OrderData bidOrderData{"bidOrderData", Side::Bid, OrderType::Limit, 190, 3};
    simpleMatchingEngine.submitNewOrder(bidOrderData);
    checkResultSize(1);
    checkResponse(0, bidOrderData, ExecutionType::New);
    auto bidExchangeId = results[0].exchangeId;

    OrderData askOrderData{"askOrderData", Side::Ask, OrderType::Limit, 101, 3};
    simpleMatchingEngine.submitNewOrder(askOrderData);
    checkResultSize(3);
    checkResponse(0, askOrderData, ExecutionType::New);
    checkResponse(1, bidOrderData, ExecutionType::Trade, 101, 3);
    checkResponse(2, askOrderData, ExecutionType::Trade, 101, 3);
    auto askExchangeId = results[0].exchangeId;
    simpleMatchingEngine.submitCancel(askExchangeId);
    checkResultSize(1);
    checkResponse(0, askOrderData, ExecutionType::Rejected);

    simpleMatchingEngine.submitCancel(bidExchangeId);
    checkResultSize(1);
    checkResponse(0, bidOrderData, ExecutionType::Rejected);
}

