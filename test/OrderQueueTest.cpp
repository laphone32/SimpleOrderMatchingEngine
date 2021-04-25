
#include "gtest/gtest.h"
#include "OrderQueue.h"
#include "TestFixtures.h"
#include "Types.h"

using namespace OrderMatchingEngine;

class OrderQueueTest : public TestFixtures {
protected:
    OrderQueueTest() : orderQueue(priceCompare, openPrice) {};

    struct {
        PriceType operator() (PriceType lhs, PriceType rhs) {
            return lhs - rhs;
        };
    } priceCompare;

    static constexpr const PriceType openPrice = 300;
    OrderQueue<decltype(priceCompare)> orderQueue;
};

TEST_F(OrderQueueTest, init) {}

TEST_F(OrderQueueTest, newOrder) {
    auto &order = getOrder();
    orderQueue.insert(order);

    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order);
}

TEST_F(OrderQueueTest, remove) {
    auto &order = getOrder();
    orderQueue.insert(order);

    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order);

    orderQueue.remove(order);
    EXPECT_EQ(orderQueue.size(), 0);
}

TEST_F(OrderQueueTest, sortByOrderTypeFirst) {
    auto &marketOrder1 = getOrder(OrderType::Market);
    auto &limitOrder1 = getOrder(OrderType::Limit);
    auto &marketOrder2 = getOrder(OrderType::Market);

    orderQueue.insert(marketOrder1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), marketOrder1);

    orderQueue.insert(limitOrder1);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), marketOrder1);

    orderQueue.insert(marketOrder2);
    EXPECT_EQ(orderQueue.size(), 3);
    checkOrder(orderQueue.best(), marketOrder1);

    orderQueue.remove(marketOrder1);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), marketOrder2);

    orderQueue.remove(limitOrder1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), marketOrder2);
}

TEST_F(OrderQueueTest, marketSortByTime) {
    auto &order1 = getOrder(OrderType::Market);
    auto &order2 = getOrder(OrderType::Market);
    auto &order3 = getOrder(OrderType::Market);

    orderQueue.insert(order1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order2);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order3);
    EXPECT_EQ(orderQueue.size(), 3);
    checkOrder(orderQueue.best(), order1);

    orderQueue.remove(order1);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order2);
}

TEST_F(OrderQueueTest, limitSortByPriceThenTime) {
    auto &order1 = getOrder(OrderType::Limit, 100);
    auto &order2 = getOrder(OrderType::Limit, 99);
    auto &order3 = getOrder(OrderType::Limit, 101);

    orderQueue.insert(order1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order2);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order3);
    EXPECT_EQ(orderQueue.size(), 3);
    checkOrder(orderQueue.best(), order3);

    orderQueue.remove(order3);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order1);
}

TEST_F(OrderQueueTest, sort) {
    auto &order1 = getOrder(OrderType::Limit, 100);
    auto &order2 = getOrder(OrderType::Limit, 99);
    auto &order3 = getOrder(OrderType::Limit, 101);
    auto &order4 = getOrder(OrderType::Market);
    auto &order5 = getOrder(OrderType::Limit, 102);

    orderQueue.insert(order1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order2);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order3);
    EXPECT_EQ(orderQueue.size(), 3);
    checkOrder(orderQueue.best(), order3);

    orderQueue.insert(order4);
    EXPECT_EQ(orderQueue.size(), 4);
    checkOrder(orderQueue.best(), order4);

    orderQueue.insert(order5);
    EXPECT_EQ(orderQueue.size(), 5);
    checkOrder(orderQueue.best(), order4);

    orderQueue.remove(order3);
    EXPECT_EQ(orderQueue.size(), 4);
    checkOrder(orderQueue.best(), order4);

    orderQueue.remove(order4);
    EXPECT_EQ(orderQueue.size(), 3);
    checkOrder(orderQueue.best(), order5);

    orderQueue.remove(order5);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order1);
}

TEST_F(OrderQueueTest, marketTaker) {
    auto &order1 = getOrder(OrderType::Limit, 99);
    auto &order2 = getOrder(OrderType::Limit, 101);
    auto &order3 = getOrder(OrderType::Limit, 102);
    auto &order4 = getOrder(OrderType::Market);

    orderQueue.insert(order1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order2);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order2);

    orderQueue.insert(order3);
    EXPECT_EQ(orderQueue.size(), 3);
    checkOrder(orderQueue.best(), order3);

    orderQueue.insert(order4);
    EXPECT_EQ(orderQueue.size(), 4);
    checkOrder(orderQueue.best(), order4);

    auto &marketOrder = getOrder(OrderType::Market, 200, 2, Side::Ask);

    orderQueue.fill(marketOrder, results);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order2);

    ASSERT_EQ(results.size(), 4);
    checkResponse(0, order4.orderData, ExecutionType::Trade, openPrice, 1);
    checkResponse(1, marketOrder.orderData, ExecutionType::Trade, openPrice, 1);
    checkResponse(2, order3.orderData, ExecutionType::Trade, 102, 1);
    checkResponse(3, marketOrder.orderData, ExecutionType::Trade, 102, 1);
}

TEST_F(OrderQueueTest, limitTaker) {
    auto &order1 = getOrder(OrderType::Market);
    auto &order2 = getOrder(OrderType::Limit, 101);
    auto &order3 = getOrder(OrderType::Limit, 102);

    orderQueue.insert(order1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order2);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order1);

    orderQueue.insert(order3);
    EXPECT_EQ(orderQueue.size(), 3);
    checkOrder(orderQueue.best(), order1);

    auto &limitOrder = getOrder(OrderType::Limit, 80, 2, Side::Ask);

    orderQueue.fill(limitOrder, results);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order2);

    ASSERT_EQ(results.size(), 4);
    checkResponse(0, order1.orderData, ExecutionType::Trade, 80, 1);
    checkResponse(1, limitOrder.orderData, ExecutionType::Trade, 80, 1);
    checkResponse(2, order3.orderData, ExecutionType::Trade, 80, 1);
    checkResponse(3, limitOrder.orderData, ExecutionType::Trade, 80, 1);
}

TEST_F(OrderQueueTest, openPrice) {
    auto &order1 = getOrder(OrderType::Market);
    orderQueue.insert(order1);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order1);

    auto &order2 = getOrder(OrderType::Limit, 101);
    orderQueue.insert(order2);
    EXPECT_EQ(orderQueue.size(), 2);
    checkOrder(orderQueue.best(), order1);

    auto &order3 = getOrder(OrderType::Market, 1000, 2, Side::Ask);
    orderQueue.fill(order3, results);
    EXPECT_EQ(orderQueue.size(), 0);
    ASSERT_EQ(results.size(), 4);
    checkResponse(0, order1.orderData, ExecutionType::Trade, openPrice, 1);
    checkResponse(1, order3.orderData, ExecutionType::Trade, openPrice, 1);
    checkResponse(2, order2.orderData, ExecutionType::Trade, 101, 1);
    checkResponse(3, order3.orderData, ExecutionType::Trade, 101, 1);

    auto &order4 = getOrder(OrderType::Market, 100, 1);
    orderQueue.insert(order4);
    EXPECT_EQ(orderQueue.size(), 1);
    checkOrder(orderQueue.best(), order4);

    auto &order5 = getOrder(OrderType::Market, 1000, 1, Side::Ask);

    results.clear();
    orderQueue.fill(order5, results);
    EXPECT_EQ(orderQueue.size(), 0);
    ASSERT_EQ(results.size(), 2);
    checkResponse(0, order4.orderData, ExecutionType::Trade, 101, 1);
    checkResponse(1, order5.orderData, ExecutionType::Trade, 101, 1);
}

