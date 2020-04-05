//
// Created by richard on 12/31/19.
//

#include "../src/blackjack_database.h"
#include "gtest/gtest.h"

TEST(database, basic_smoke_test_to_create_database) {
    blackjack_database testdb;
    testdb.create();
    ASSERT_TRUE(true);
}