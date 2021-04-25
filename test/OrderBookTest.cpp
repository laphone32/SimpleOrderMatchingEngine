
#include "gtest/gtest.h"
#include "OrderBook.h"
#include "TestFixtures.h"
#include "Types.h"

using namespace OrderMatchingEngine;

class OrderBookTest : public TestFixtures {
protected:
    OrderBookTest() : orderBook(100) {};

    OrderBook orderBook;

    Order bidOrder100 = getOrder(OrderType::Limit, 100, 10, Side::Bid);
    Order bidOrder98 = getOrder(OrderType::Limit, 98, 10, Side::Bid);
    Order askOrder98 = getOrder(OrderType::Limit, 98, 6, Side::Ask);
    Order askOrder100 = getOrder(OrderType::Limit, 100, 9, Side::Ask);
    Order askOrder102 = getOrder(OrderType::Limit, 102, 3, Side::Ask);
};

TEST_F(OrderBookTest, init) {}

TEST_F(OrderBookTest, newBidOrder) {
    orderBook.newOrder(bidOrder100, results);
    ASSERT_EQ(results.size(), 0);
}

TEST_F(OrderBookTest, newAskOrder) {
    orderBook.newOrder(askOrder100, results);
    ASSERT_EQ(results.size(), 0);
}

TEST_F(OrderBookTest, newOrder_AskQueue) {
    orderBook.newOrder(askOrder100, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(askOrder102, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(bidOrder98, results);
    ASSERT_EQ(results.size(), 0);
}

TEST_F(OrderBookTest, newOrder_BidQueue) {
    orderBook.newOrder(bidOrder100, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(askOrder102, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(bidOrder98, results);
    ASSERT_EQ(results.size(), 0);
}

TEST_F(OrderBookTest, OrderMatch_Ask) {
    orderBook.newOrder(bidOrder100, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(askOrder100, results);
    ASSERT_EQ(results.size(), 2);

    checkResponse(0, bidOrder100.orderData, ExecutionType::Trade, 100, 9);
    checkResponse(1, askOrder100.orderData, ExecutionType::Trade, 100, 9);
}

TEST_F(OrderBookTest, OrderMatch_Bid) {
    orderBook.newOrder(askOrder100, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(bidOrder100, results);
    ASSERT_EQ(results.size(), 2);

    checkResponse(0, askOrder100.orderData, ExecutionType::Trade, 100, 9);
    checkResponse(1, bidOrder100.orderData, ExecutionType::Trade, 100, 9);
}

TEST_F(OrderBookTest, OrderMatch_PartiallFill) {
    orderBook.newOrder(askOrder100, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(askOrder98, results);
    ASSERT_EQ(results.size(), 0);

    orderBook.newOrder(bidOrder100, results);
    ASSERT_EQ(results.size(), 4);

    checkResponse(0, askOrder98.orderData, ExecutionType::Trade, 100, 6);
    checkResponse(1, bidOrder100.orderData, ExecutionType::Trade, 100, 6);
    checkResponse(2, askOrder100.orderData, ExecutionType::Trade, 100, 4);
    checkResponse(3, bidOrder100.orderData, ExecutionType::Trade, 100, 4);
}

