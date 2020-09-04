#ifndef BLACKJACK_PLAYER_HXX
#define BLACKJACK_PLAYER_HXX

#include <ostream>
#include <string>
#include <vector>

#include "card.hxx"

class Game;

class Player {
public:
    explicit Player(std::string);

    // Init initializes the player for a new game.
    auto Init() -> void;

    auto AddCard(Card) -> void;
    // SetAce sets the value of the ith ace, counting from 0. It returns true if the value was successfully set.
    auto SetAce(std::size_t, int) noexcept -> bool;
    auto Win() -> void;
    auto Lose() -> void;
    auto Blackjack() -> void;
    auto Bust() -> void;
    auto Surrender() -> void;

    [[nodiscard]] auto GetName() const noexcept -> const std::string&;
    [[nodiscard]] auto GetCurrentScore() const noexcept -> int;
    [[nodiscard]] auto GetAceCount() const noexcept -> std::size_t;

    [[nodiscard]] auto GetWinCount() const noexcept -> int;
    [[nodiscard]] auto GetLossCount() const noexcept -> int;
    [[nodiscard]] auto GetBlackjackCount() const noexcept -> int;
    [[nodiscard]] auto GetBustCount() const noexcept -> int;
    [[nodiscard]] auto GetSurrenderCount() const noexcept -> int;
    // WriteCurrentCards writes a string representation of all the cards the player has in the stream.
    auto WriteCurrentCards(std::ostream&) const -> void;

private:
    std::vector<Card> cards_;
    std::vector<std::size_t> ace_index_;
    std::string name_;
    int win_count_;
    int loss_count_;
    int blackjack_count_;
    int bust_count_;
    int surrender_count_;
};

#endif //BLACKJACK_PLAYER_HXX
