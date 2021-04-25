#ifndef __ORDERMATCHINGENGINE_ORDER
#define __ORDERMATCHINGENGINE_ORDER

#include <chrono>
#include "Types.h"

namespace OrderMatchingEngine {

struct Order {
    Order(const OrderData &orderData): orderData(orderData), leftQuantity(orderData.quantity) {}

    auto cancelAck() {
        leftQuantity = 0;
    }

    auto fill(QuantityType fillQuantity) {
        leftQuantity -= fillQuantity;
    }

    OrderData orderData;
    ExchangeIdType exchangeId;
    QuantityType leftQuantity;
};

}

#endif
