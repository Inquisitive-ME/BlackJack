Feature: Card

 Scenario Outline: createNewCardWithRank
    Given Create new card with rank <rank>
    Then Card value should be  <value>
  Examples:
    | rank  | value |
    | 1     | 1     |
    | 2     | 2     |