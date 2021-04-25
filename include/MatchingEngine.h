#ifndef __ORDERMATCHINGENGINE_MATCHINGENGINE
#define __ORDERMATCHINGENGINE_MATCHINGENGINE

#include "Types.h"
#include <vector>

namespace OrderMatchingEngine {

struct MatchingEngine {
    virtual void submitNewOrder(const OrderData &orderData) = 0;

    virtual void submitCancel(ExchangeIdType exchangeId) = 0;

    virtual ResultsType getResultsOfLastOperation() = 0;
};

};
#endif
