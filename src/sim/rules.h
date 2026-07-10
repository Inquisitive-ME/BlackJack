#ifndef BJ_SIM_RULES_H
#define BJ_SIM_RULES_H

namespace bj {

// A player decision. Order matches the future observation's legal-action mask.
enum class Action { Hit, Stand, Double, Split, Surrender };

// Table rules. Defaults are a common Vegas 6-deck shoe game.
struct RulesConfig {
    int    decks             = 6;
    double penetration       = 0.75;  // reshuffle after this fraction is dealt
    bool   hitSoft17         = true;  // dealer hits soft 17 (H17)
    double blackjackPays     = 1.5;   // 3:2 natural
    bool   doubleAfterSplit  = true;  // DAS
    int    maxSplits         = 3;     // up to 4 hands
    bool   surrenderAllowed  = false; // late surrender
    bool   resplitAces       = false;
};

} // namespace bj

#endif // BJ_SIM_RULES_H
