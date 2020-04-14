/*
 * BJDealer.h
 *
 *  Created on: May 23, 2018
 *      Author: richard
 */

#ifndef DEALER_H_
#define DEALER_H_

#include "dealer_interface.h"

class DealerImpl : public DealerInterface {
public:
    DealerImpl();

    const bool isHitting() const override;

    std::vector<card> getFaceUpCards() override;

    virtual ~DealerImpl();
};

#endif /* DEALER_H_ */
