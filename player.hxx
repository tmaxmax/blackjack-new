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
    void Init();

    void AddCard(Card);
    // SetAce sets the value of the ith ace, counting from 0. It returns true if the value was successfully set.
    bool SetAce(std::size_t, int) noexcept;
    void Win();
    void Lose();
    void Blackjack();
    void Bust();
    void Surrender();

    [[nodiscard]] const std::string& GetName() const noexcept;
    [[nodiscard]] int GetCurrentScore() const noexcept;
    [[nodiscard]] std::size_t GetAceCount() const noexcept;

    [[nodiscard]] int GetWinCount() const noexcept;
    [[nodiscard]] int GetLossCount() const noexcept;
    [[nodiscard]] int GetBlackjackCount() const noexcept;
    [[nodiscard]] int GetBustCount() const noexcept;
    [[nodiscard]] int GetSurrenderCount() const noexcept;
    // WriteCurrentCards writes a string representation of all the cards the player has in the stream.
    void WriteCurrentCards(std::ostream&) const;

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
