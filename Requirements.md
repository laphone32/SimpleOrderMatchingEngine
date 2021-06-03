# Requirements
- Built by a standard C++ compiler (<= C++14)
- Type representing an order object, with client ID, exchange ID, side, price, quantity.
- Type(s) representing responses from the matching engine (new order ack, new order reject, cancel ack, cancel reject, execution)
- Class implementing order matching capability
  - Accept new order requests and cancel outstanding orders.
  - Provide matching engine responses / notifications in logical order.
- Interface specification
  - `submitNew()`: enter a new order into the engine.
  - `submitCancel()`: cancel an outstanding order.
  - `getResultsOfLastOperation()`: access a container with matching engine responses pertaining to last `submit...()` operation.
- Rules of order handling
  - Order priority is price, then time of arrival.
  - Market order has priority higher than any limit order on the same side.
  - If a new market buy order is submitted, it is to be matched against existing sell orders, in order of priority, until either fully filled or no more sell orders.
  - If a new market sell order is submitted, it is to be matched against existing buy orders, in order of priority, until either fully filled or no more buy orders.
  - If a new limit buy order is submitted at price `P`, it is to be matched against existing sell orders at or below `P`, in order of priority, until either fully filled or no more sell orders at or below `P`. If new limit buy order cannot be fully matched, it is retained by the engine.
  - If a new limit sell order is submitted at price `P`, it is to be matched against existing buy orders at or above `P`, in order of priority, until either fully filled or no more buy orders at or above `P`. If new limit sell order cannot be fully matched, it is retained by the engine.
- Sample code that uses the above classes

# Properties
Besides the requirements above, some properties of the order matching engine are attached.
- Without any extra concepts involved such as symbol, TimeInforce... etc.
- Latest matching price is keeped as `LastPrice`, which could be initialised by open price, for the fill price of further market orders. Let **Order1** and **Order2** be given with **Order1** arrived earlier than **Order2**, the fill prices are as follows if it could be matched:

Order2 \ Order1| Limit `P1`|Market
---------------|------------|--------
**Limit `P2`** |`P1`|`P2`
**Market**|`P1`|`LastPrice` 
