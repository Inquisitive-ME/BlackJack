Feature: Card
  Verify value, count and print statements of card class

  Scenario Outline: createNewCardWithRank
    Given Create new card with rank <rank>
    Then Card value should be <value>
    And High Low Count should be <HighLowCount>, Zen Count should be <ZenCount> and Omega II Count should be <OmegaIICount>
    And Print String should be <PrintString>
    And Card isAce <isAce>

  Examples:
    | rank          | value         | HighLowCount  | ZenCount      | OmegaIICount  | PrintString   | isAce         |
    | 1             | 1             | -1            | -1            | 0             | A             | true          |
    | 2             | 2             | 1             | 1             | 1             | 2             | false         |
    | 3             | 3             | 1             | 1             | 1             | 3             | false         |
    | 4             | 4             | 1             | 2             | 2             | 4             | false         |
    | 5             | 5             | 1             | 2             | 2             | 5             | false         |
    | 6             | 6             | 1             | 2             | 2             | 6             | false         |
    | 7             | 7             | 0             | 1             | 1             | 7             | false         |
    | 8             | 8             | 0             | 0             | 0             | 8             | false         |
    | 9             | 9             | 0             | 0             | -1            | 9             | false         |
    | 10            | 10            | -1            | -2            | -2            | 10            | false         |
    | 11            | 10            | -1            | -2            | -2            | J             | false         |
    | 12            | 10            | -1            | -2            | -2            | Q             | false         |
    | 13            | 10            | -1            | -2            | -2            | K             | false         |


