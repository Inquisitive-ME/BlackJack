//
// Created by richard on 8/4/19.
//


#include "../src/deck.h"
#include "../src/getRandomInt.cpp"

#include "GUnit/GTest.h"

GTEST("Deck", "Test populate method")
{
  int numDecks = 6;
  deck testDeck(numDecks, 0);

  testDeck.populate();

  SHOULD("Number of cards should be number of decks * 52")
  {
    EXPECT_EQ(uint(numDecks * 52), testDeck.getNumCards());
  }

  SHOULD("Have correct number of each card")
  {
    std::map<std::string, int> cardCount;
    for(uint i = 0; i < testDeck.getNumCards(); i++)
    {
      cardCount[testDeck.getCard(i).print()]++;
    }

    int expectedUniqueCards = 13;
    int expectedUniqueCardsCount = numDecks * 52 / expectedUniqueCards;
    int actualUniqueCards = 0;

    for(auto& uniqueCard : cardCount)
    {
      actualUniqueCards++;
      EXPECT_EQ(expectedUniqueCardsCount, uniqueCard.second);
    }
    EXPECT_EQ(expectedUniqueCards, actualUniqueCards);
  }
}

GTEST("Deck", "Test that shuffle occurs when deck is at shuffle limit")
{
  int numDecks = 1;
  int whenToShuffle = 13;
  deck testDeck(numDecks, whenToShuffle);

  BJHand testHand(0);

  testDeck.populate();
  testDeck.deal(testHand, numDecks * 52 - whenToShuffle);

  EXPECT_EQ(uint(numDecks * 52), testDeck.getNumCards());

}
GTEST("Deck", "Deal from deck to hand")
{
  int numDecks = 1;
  deck testDeck(numDecks, 0);

  BJHand testHand(0);

  testDeck.populate();

  testDeck.deal(testHand, numDecks * 52);

  SHOULD("Have entire deck transfered to hand")
  {
    std::map<std::string, int> cardCount;
    for(uint i = 0; i < testHand.getNumCards(); i++)
    {
      cardCount[testHand.getCard(i).print()]++;
    }

    int expectedUniqueCards = 13;
    int expectedUniqueCardsCount = numDecks * 52 / expectedUniqueCards;
    int actualUniqueCards = 0;

    for(auto& uniqueCard : cardCount)
    {
      actualUniqueCards++;
      EXPECT_EQ(expectedUniqueCardsCount, uniqueCard.second);
    }
    EXPECT_EQ(expectedUniqueCards, actualUniqueCards);
  }
}

// deal to hand
GTEST("Deck", "Deal from deck to player hands")
{
  int numDecks = 1;
  deck testDeck(numDecks, 0);

  player testPlayer;
  testPlayer.newHand(0);
  testPlayer.newHand(0);

  testDeck.populate();
  testDeck.deal(testPlayer, numDecks * 52);

  SHOULD("two decks should be split between player hands")
  {
    EXPECT_EQ(uint(numDecks * 52), testPlayer.getHand(0).getNumCards());
    EXPECT_EQ(uint(numDecks * 52), testPlayer.getHand(1).getNumCards());
  }

  SHOULD("Hands should have all cards expected in deck")
  {
    std::map<std::string, int> cardCount;
    for(BJHand pHand : testPlayer.getHands())
    {
      for(uint i = 0; i < pHand.getNumCards(); i++)
      {
        cardCount[pHand.getCard(i).print()]++;
      }
    }

    int expectedUniqueCards = 13;
    int expectedUniqueCardsCount = numDecks * 52 * 2 / expectedUniqueCards;
    int actualUniqueCards = 0;

    for(auto& uniqueCard : cardCount)
    {
      actualUniqueCards++;
      EXPECT_EQ(expectedUniqueCardsCount, uniqueCard.second);
    }
    EXPECT_EQ(expectedUniqueCards, actualUniqueCards);
  }
}

GTEST("Deck", "Test addCount callback")
{
  deck testDeck(1,0);
  testDeck.populate();

  BJHand testHand(0);

  EXPECT_EQ(0, testDeck.getCount());

  testDeck.deal(testHand, 1);
  while (testHand.getCard(0).getHighLowCount() == 0)
  {
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