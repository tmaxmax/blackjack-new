#ifndef BLACKJACK_PLAYER_HXX
#define BLACKJACK_PLAYER_HXX

#include <ostream>
#include <string>
#include <vector>

#include "card.hxx"

class Game;

class BasePlayer {
public:
    void AddCard(Card);
    void ClearCards();
    // SetAce sets the value of the first unset ace found, returning true if there are aces left.
    bool SetAce(int) noexcept;
    void Win();
    void Lose();
    void Surrender();
    void Blackjack();
    void Bust();

    [[nodiscard]] int GetCurrentScore() const noexcept;
    [[nodiscard]] std::size_t GetCardCount() const noexcept;
    [[nodiscard]] std::size_t GetAceCount() const noexcept;

    [[nodiscard]] int GetWinCount() const noexcept;
    [[nodiscard]] int GetLossCount() const noexcept;
    [[nodiscard]] int GetBustCount() const noexcept;
    [[nodiscard]] int GetBlackjackCount() const noexcept;
    [[nodiscard]] int GetSurrenderCount() const noexcept;
    // WriteCurrentCards writes a string representation of all the cards the player has in the stream.
    void WriteCurrentCards(std::ostream&);

protected:
    std::vector<Card> cards_;

private:
    int win_count_;
    int loss_count_;
    int bust_count_;
    int blackjack_count_;
    int surrender_count_;
};

class Player : public BasePlayer {
public:
    Player(std::string);

    [[nodiscard]] const std::string& GetName() const noexcept;
private:
    std::string name_;
};

class Dealer : public BasePlayer {
};

#endif //BLACKJACK_PLAYER_HXX
