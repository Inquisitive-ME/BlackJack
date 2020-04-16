//
// Created by richard on 8/4/19.
//


#include "../src/deck.h"
#include "../src/get_random_int.h"
#include "../src/player_impl.h"
#include "test_mock_classes.h"

#include "gtest/gtest.h"

TEST(Deck, Populate_with_correct_number_of_cards) {
    uint numDecks = 6;
    deck testDeck(numDecks, 0);

    testDeck.populate();

    ASSERT_EQ(uint(numDecks * 52), testDeck.getNumCards());
}

TEST(Deck, Populate_with_correct_number_of_each_card) {
    uint numDecks = 6;
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

TEST(Deck, Shuffle_occurs_when_deal_face_down_and_deck_is_at_shuffle_limit) {
    uint numDecks = 1;
    uint whenToShuffle = 13;
    deck testDeck(numDecks, whenToShuffle);

    BJHand testHand(0);

    testDeck.populate();
    testDeck.dealFaceDown(testHand, numDecks * 52 - whenToShuffle);

    ASSERT_EQ(uint(numDecks * 52), testDeck.getNumCards());
}

TEST(Deck, Shuffle_occurs_when_deal_face_up_hand_and_deck_is_at_shuffle_limit) {
    uint numDecks = 1;
    uint whenToShuffle = 13;
    deck testDeck(numDecks, whenToShuffle);

    BJHand testHand(0);

    testDeck.populate();
    testDeck.dealFaceUp(testHand, numDecks * 52 - whenToShuffle);

    ASSERT_EQ(uint(numDecks * 52), testDeck.getNumCards());
}

TEST(Deck, Shuffle_occurs_when_deal_face_up_player_and_deck_is_at_shuffle_limit) {
    uint numDecks = 1;
    uint whenToShuffle = 13;
    deck testDeck(numDecks, whenToShuffle);

    std::unique_ptr<PlayerInterface> testPlayer = std::make_unique<PlayerImpl>();
    testPlayer->newHand(0);

    testDeck.populate();
    testDeck.dealFaceUp(testPlayer, numDecks * 52 - whenToShuffle);

    ASSERT_EQ(uint(numDecks * 52), testDeck.getNumCards());
}

TEST(Deck, Deal_facedown_to_hand_verify_correct_number_of_cards_dealt) {
    uint numDecks = 1;
    deck testDeck(numDecks, 0);

    BJHand testHand(0);

    testDeck.populate();

    testDeck.dealFaceDown(testHand, numDecks * 52);

    ASSERT_EQ(testHand.getNumCards(), numDecks * 52);
}

TEST(Deck, Deal_facedown_to_hand_verify_correct_number_of_each_card_in_hand) {
    uint numDecks = 1;
    deck testDeck(numDecks, 0);

    BJHand testHand(0);

    testDeck.populate();

    testDeck.dealFaceDown(testHand, numDecks * 52);

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

TEST(Deck, Deal_faceup_to_two_player_hands){
    uint numDecks = 1;
    deck testDeck(numDecks, 0);

    std::unique_ptr<PlayerInterface> testPlayer = std::make_unique<PlayerImpl>();
    testPlayer->newHand(0);
    testPlayer->newHand(0);

    testDeck.populate();

    testDeck.dealFaceUp(testPlayer, 1);

    EXPECT_TRUE(testPlayer->getHand(0).getCard(0).isFaceUp());
    EXPECT_TRUE(testPlayer->getHand(1).getCard(0).isFaceUp());
}

TEST(Deck, Deal_faceup_two_decks_to_two_player_hands) {
    uint numDecks = 1;
    deck testDeck(numDecks, 0);

    std::unique_ptr<PlayerInterface> testPlayer = std::make_unique<PlayerImpl>();
    testPlayer->newHand(0);
    testPlayer->newHand(0);

    testDeck.populate();

    testDeck.dealFaceUp(testPlayer, numDecks * 52);

    EXPECT_EQ(uint(numDecks * 52), testPlayer->getHand(0).getNumCards())
                        << "Deal 1 full deck to each player hand" << std::endl;
    EXPECT_EQ(uint(numDecks * 52), testPlayer->getHand(1).getNumCards())
                        << "Deal 1 full deck to each player hand" << std::endl;

    std::map<std::string, int> cardCount;
    for (int handNumber = 0; handNumber < testPlayer->getNumHands(); handNumber++) {
        for (uint i = 0; i < testPlayer->getHand(handNumber).getNumCards(); i++) {
            cardCount[testPlayer->getHand(handNumber).getCard(i).print()]++;
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

    testDeck.dealFaceDown(testHand, 1);
    while (testHand.getCard(0).getHighLowCount() == 0) {
        testHand.clear();
        testDeck.dealFaceDown(testHand, 1);
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

TEST(Deck, hit_player_hand) {
    deck testDeck(1, 0);
    testDeck.populate();

    std::unique_ptr<PlayerInterface> testPlayer = std::make_unique<PlayerImpl>();
    testPlayer->newHand(0);

    testDeck.hitPlayerHand(testPlayer, 0);

    EXPECT_TRUE(testPlayer->getNumHands() == 1);
    EXPECT_TRUE(testPlayer->getHand(0).getTotal() > 0);
    EXPECT_TRUE(testPlayer->getHand(0).getNumCards() == 1);
}