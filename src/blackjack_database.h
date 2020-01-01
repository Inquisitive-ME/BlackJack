//
// Created by richard on 12/31/19.
//

#ifndef BLACKJACK_BLACKJACK_DATABASE_H
#define BLACKJACK_BLACKJACK_DATABASE_H

#include <sqlite3.h>

class blackjack_database{
public:
  blackjack_database();
  virtual ~blackjack_database();
  int create();
  void open(char * file_name);
};
#endif //BLACKJACK_BLACKJACK_DATABASE_H
