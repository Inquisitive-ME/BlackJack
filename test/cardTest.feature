Feature: Card
  Verify value, count and print statements of card class

  Scenario Outline: createNewCardWithRank
    Given Create new card with rank <rank>
    Then Card value should be <value>
    And Card isAce <isAce>

  Examples:
    | rank  | value | isAce |
    | 1     | 1     | true  |
    | 2     | 2     | false |
    | 3     | 3     | false |
    | 4     | 4     | false |

