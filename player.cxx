#include "player.hxx"

#include <experimental/iterator>
#include <utility>

Player::Player(std::string name)
    : cards_(), name_(std::move(name)), win_count_(), loss_count_(), blackjack_count_(), bust_count_() {}

auto Player::Init() -> void {
    cards_.clear();
    ace_index_.clear();
}

auto Player::AddCard(Card c) -> void {
    cards_.push_back(c);
    if (c.GetSuit() == Card::Suit::Ace) {
        ace_index_.push_back(cards_.size() - 1);
    }
}

auto Player::SetAce(std::size_t i, int new_value) noexcept -> bool {
    return cards_[ace_index_[i]].SetValue(new_value);
}

auto Player::Win() -> void {
    win_count_++;
}

auto Player::Lose() -> void {
    loss_count_++;
}

auto Player::Blackjack() -> void {
    blackjack_count_++;
}

auto Player::Bust() -> void {
    bust_count_++;
}

auto Player::GetName() const noexcept -> const std::string& {
    return name_;
}

auto Player::GetCurrentScore() const noexcept -> int {
    int result{};
    for (const auto& c : cards_) {
        result += c.GetValue();
    }
    return result;
}

auto Player::GetAceCount() const noexcept -> std::size_t {
    return ace_index_.size();
}

auto Player::GetWinCount() const noexcept -> int {
    return win_count_;
}

auto Player::GetLossCount() const noexcept -> int {
    return loss_count_;
}

auto Player::GetBlackjackCount() const noexcept -> int {
    return blackjack_count_;
}

auto Player::GetBustCount() const noexcept -> int {
    return bust_count_;
}

auto Player::WriteCurrentCards(std::ostream& os) const -> void {
    std::copy(std::begin(cards_), std::end(cards_), std::experimental::make_ostream_joiner(os, '\n'));
}
