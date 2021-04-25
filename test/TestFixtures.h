#ifndef __ORDERMATCHINGENGINE_TESTFIXTURES
#define __ORDERMATCHINGENGINE_TESTFIXTURES

#include "gtest/gtest.h"
#include "Order.h"
#include "Types.h"

namespace OrderMatchingEngine {

class TestFixtures : public ::testing::Test {
protected:
    void SetUp() override {};

    ExchangeIdType exchangeId = 0;
    auto &&getOrder(OrderType orderType = OrderType::Limit, PriceType price = 100, QuantityType quantity = 1, Side side = Side::Bid) {
        OrderData orderData("x", side, orderType, price, quantity);
        auto *order = new Order(orderData);
        order->exchangeId = ++ exchangeId;
        return *order;
    }

    auto checkOrderData(const OrderData & orderData, const OrderData &expectOrderData) {
        EXPECT_EQ(orderData.orderType, expectOrderData.orderType);
        EXPECT_EQ(orderData.clientId, expectOrderData.clientId);
        EXPECT_EQ(orderData.price, expectOrderData.price);
        EXPECT_EQ(orderData.quantity, expectOrderData.quantity);
        EXPECT_EQ(orderData.side, expectOrderData.side);
    }

    auto checkOrder(const Order &order, const Order &expectOrder) {
        checkOrderData(order.orderData, expectOrder.orderData);

        EXPECT_EQ(order.exchangeId, expectOrder.exchangeId);
        EXPECT_EQ(order.leftQuantity, expectOrder.leftQuantity);
    }

    auto checkOrder(Order *order, const Order &expectOrder) {
        ASSERT_NE(order, nullptr);
        checkOrder(*order, expectOrder);
    }

    auto checkResponse(int resultIndex, const OrderData &expectOrderData, ExecutionType expectExecutionType, PriceType expectFillPrice = 0, QuantityType expectFillQuantity = 0) {
        ASSERT_LT(resultIndex, results.size());
        auto &response = results[resultIndex];
        EXPECT_EQ(response.executionType, expectExecutionType);
        EXPECT_EQ(response.fillPrice, expectFillPrice);
        EXPECT_EQ(response.fillQuantity, expectFillQuantity);

        checkOrderData(response.orderData, expectOrderData);
    }

    ResultsType results;
};

}
#endif
