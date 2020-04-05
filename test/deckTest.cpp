//
// Created by richard on 8/4/19.
//


#include "../src/deck.h"
#include "../src/get_random_int.h"
#include "../src/player_impl.h"

#include "gtest/gtest.h"

TEST(Deck, Populate_with_correct_number_of_cards) {
    int numDecks = 6;
    deck testDeck(numDecks, 0);

    testDeck.populate();

    ASSERT_EQ(uint(numDecks * 52), testDeck.getNumCards());
}

TEST(Deck, Populate_with_correct_number_of_each_card) {
    int numDecks = 6;
    deck testDeck(numDecks, 0);

    testDeck.populate();

    std::map<std::string, int> cardCount;
    for (uint i = 0; i < testDeck.getNumCards(); i++) {
        cardCount[testDeck.getCard(i).print()]++;
    }

    int expectedUniqueCards = 13;
    int expectedUniqueCardsCount = numDecks * 52 / expectedUniqueCards;
    int actualUniqueCards = 0;

    for (auto &uniqueCard : cardCount) {
        actualUniqueCards++;
        EXPECT_EQ(expectedUniqueCardsCount, uniqueCard.second);
    }
    ASSERT_EQ(expectedUniqueCards, actualUniqueCards);
}

TEST(Deck, Shuffle_occurs_when_deck_is_at_shuffle_limit) {
    int numDecks = 1;
    int whenToShuffle = 13;
    deck testDeck(numDecks, whenToShuffle);

    BJHand testHand(0);

    testDeck.populate();
    testDeck.deal(testHand, numDecks * 52 - whenToShuffle);

    ASSERT_EQ(uint(numDecks * 52), testDeck.getNumCards());
}

TEST(Deck, Deal_to_hand_verify_correct_number_of_cards_dealt) {
    int numDecks = 1;
    deck testDeck(numDecks, 0);

    BJHand testHand(0);

    testDeck.populate();

    testDeck.deal(testHand, numDecks * 52);

    ASSERT_EQ(testHand.getNumCards(), numDecks * 52);
}

TEST(Deck, Deal_to_hand_verify_correct_number_of_each_card_in_hand) {
    int numDecks = 1;
    deck testDeck(numDecks, 0);

    BJHand testHand(0);

    testDeck.populate();

    testDeck.deal(testHand, numDecks * 52);

    std::map<std::string, int> cardCount;
    for (uint i = 0; i < testHand.getNumCards(); i++) {
        cardCount[testHand.getCard(i).print()]++;
    }

    int expectedUniqueCards = 13;
    int expectedUniqueCardsCount = numDecks * 52 / expectedUniqueCards;
    int actualUniqueCards = 0;

    for (auto &uniqueCard : cardCount) {
        actualUniqueCards++;
        ASSERT_EQ(expectedUniqueCardsCount, uniqueCard.second);
    }
    ASSERT_EQ(expectedUniqueCards, actualUniqueCards);
}

TEST(Deck, Deal_two_decks_to_two_player_hands) {
    int numDecks = 1;
    deck testDeck(numDecks, 0);

    PlayerImpl testPlayer;
    testPlayer.newHand(0);
    testPlayer.newHand(0);

    testDeck.populate();

    testDeck.deal(testPlayer, numDecks * 52);

    EXPECT_EQ(uint(numDecks * 52), testPlayer.getHand(0).getNumCards())
                        << "Deal 1 full deck to each player hand" << std::endl;
    EXPECT_EQ(uint(numDecks * 52), testPlayer.getHand(1).getNumCards())
                        << "Deal 1 full deck to each player hand" << std::endl;

    std::map<std::string, int> cardCount;
    for (BJHand pHand : testPlayer.getHands()) {
        for (uint i = 0; i < pHand.getNumCards(); i++) {
            cardCount[pHand.getCard(i).print()]++;
        }
    }

    int expectedUniqueCards = 13;
    int expectedUniqueCardsCount = numDecks * 52 * 2 / expectedUniqueCards;
    int actualUniqueCards = 0;

    for (auto &uniqueCard : cardCount) {
        actualUniqueCards++;
        ASSERT_EQ(expectedUniqueCardsCount, uniqueCard.second)
                                    << "Each hand should have all cards expected in deck" << std::endl;;
    }
    ASSERT_EQ(expectedUniqueCards, actualUniqueCards)
                                << "Each hand should have all cards expected in deck" << std::endl;;
}

TEST(Deck, addCount_callback_is_called_after_card_is_dealt_andthen_flipped) {
    deck testDeck(1, 0);
    testDeck.populate();

    BJHand testHand(0);

    EXPECT_EQ(0, testDeck.getCount());

    testDeck.deal(testHand, 1);
    while (testHand.getCard(0).getHighLowCount() == 0) {
        testHand.clear();
        testDeck.deal(testHand, 1);
    }

    // Deck is still count of 0 before flipping card
    EXPECT_EQ(0, testDeck.getCount());

    int expectedCount = testHand.getCard(0).getHighLowCount();
    testHand.flipCard(0);
    // Deck has count of card after flipping it
    EXPECT_EQ(expectedCount, testDeck.getCount());

    testHand.flipCard(0);
    // Deck count is zero again after flipping card
    EXPECT_EQ(0, testDeck.getCount());

}