//
// Created by richard on 4/13/20.
//

#ifndef BLACKJACK_COMMAND_LINE_DEALER_H
#define BLACKJACK_COMMAND_LINE_DEALER_H

#include "dealer_impl.h"

class CommandLineDealer: public DealerImpl {
private:
//    void outputHand() const{
//        std::string string_hand;
//        for(uint i = 0; i < getNumCards(); i++) {
//            if(getCard(i).isFaceUp()) {
//                std::cout << getCard(i).print() << " ";
//            }
//        };
//    }
    void printDealerHit(card cardToAdd) {
        if (cardToAdd.isFaceUp()) {
            std::cout << "Dealer hits, and receives " << cardToAdd.print() << std::endl;
            std::cout << "Dealer hand: " << DealerImpl::print() << std::endl;
        }
    };

public:
    CommandLineDealer() : DealerImpl() {std::cout << "Create Command Line Dealer " << std::endl;};

    void copy_to_hand(card cardToAdd) override{
        DealerImpl::copy_to_hand(cardToAdd);
        printDealerHit(cardToAdd);
    };

    void move_to_hand(card &cardToMove) override{
        DealerImpl::move_to_hand(cardToMove);
        printDealerHit(cardToMove);
    };

    bool isBusted() const override{
        bool tempIsBusted = DealerImpl::isBusted();
        if(tempIsBusted){
            std::cout << "Dealer Busted" << std::endl;
        } else{
            std::cout << "Dealer hand: " << DealerImpl::print() << std::endl;
        }
        return tempIsBusted;
    };

    bool isBlackJack() const override{
        bool tempIsBlackJack = DealerImpl::isBlackJack();
        if(tempIsBlackJack){
            std::cout << "Dealer has BlackJack" << std::endl;
        }
        return tempIsBlackJack;
    };

    void flipCard(int cardIndex) override{
        DealerImpl::flipCard(cardIndex);
        std:: cout << "Dealer Hand: " << DealerImpl::print() << std::endl;
    };
};


#endif //BLACKJACK_COMMAND_LINE_DEALER_H
