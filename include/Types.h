#ifndef __ORDERMATCHINGENGINE_TYPE
#define __ORDERMATCHINGENGINE_TYPE

#include <string>
#include <vector>

namespace OrderMatchingEngine {

using ClientIdType = std::string;
using ExchangeIdType = unsigned long long;

using SideType = char;
enum class Side : SideType {
    None = 0,
    Bid = '1',
    Ask = '2'
};

using OrderTypeType = char;
enum class OrderType : OrderTypeType {
    None = 0,
    Market = '1',
    Limit = '2'
};

using PriceType = double;
using QuantityType = unsigned long;

using ExecutionTypeType = char;
enum class ExecutionType : ExecutionTypeType {
    None = 0,
    New = '0',
    Canceled = '4',
    Rejected = '8',
    Trade = 'F'
};

struct OrderData {
    OrderData() : OrderData("", Side::None, OrderType::None, 0, 0) {};

    OrderData(ClientIdType clientId_, Side side_, OrderType orderType_, PriceType price_, QuantityType quantity_) : clientId(clientId_), side(side_), orderType(orderType_), price(price_), quantity(quantity_) {}

    OrderData(const OrderData &orderData) : OrderData(orderData.clientId, orderData.side, orderData.orderType, orderData.price, orderData.quantity) {}

    ClientIdType clientId;
    Side side;
    OrderType orderType;
    PriceType price;
    QuantityType quantity;
};

struct Response {
    Response(const OrderData &orderData_, ExecutionType executionType_ = ExecutionType::Rejected, ExchangeIdType exchangeId_ = 0, PriceType fillPrice_ = 0, PriceType fillQuantity_ = 0) : orderData(orderData_), exchangeId(exchangeId_), executionType(executionType_), fillPrice(fillPrice_), fillQuantity(fillQuantity_) {}

    OrderData orderData;
    ExchangeIdType exchangeId;
    ExecutionType executionType;
    PriceType fillPrice;
    QuantityType fillQuantity;
};
using ResultsType = std::vector<Response>;


};
#endif
