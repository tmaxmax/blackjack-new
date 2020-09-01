#include <algorithm>
#include <utility>

#include "player.hxx"

void BasePlayer::Win() {
    win_count_++;
}

void BasePlayer::Surrender() {
    surrender_count_++;
}

void BasePlayer::Blackjack() {
    blackjack_count_++;
}

void BasePlayer::Bust() {
    bust_count_++;
}

int BasePlayer::GetCurrentScore() const noexcept {
    int res{};
    for (const auto& c : cards_) {
        res += c.GetValue();
    }
    return res;
}

void BasePlayer::WriteCurrentCards(std::ostream& os) {
    for (auto& c : cards_) {
        os << c << '\n';
    }
}

void BasePlayer::AddCard(Card c) {
    cards_.push_back(c);
}

std::size_t BasePlayer::GetCardCount() const noexcept {
    return cards_.size();
}

std::size_t BasePlayer::GetAceCount() const noexcept {
    std::size_t ret{};
    for (const auto& c : cards_) {
        if (c.GetSuit() == Card::Suit::Ace) {
            ret++;
        }
    }
    return ret;
}

bool BasePlayer::SetAce(int value) noexcept {
    int ace_count{};
    bool was_set{};
    for (auto& c : cards_) {
        if (c.GetValue() == 0) {
            if (!was_set) {
                c.SetValue(value);
                was_set = true;
            } else {
                ace_count++;
            }
        }
    }
    return ace_count > 0;
}

void BasePlayer::Lose() {
    loss_count_++;
}

int BasePlayer::GetWinCount() const noexcept {
    return win_count_;
}

int BasePlayer::GetLossCount() const noexcept {
    return loss_count_;
}

int BasePlayer::GetBustCount() const noexcept {
    return bust_count_;
}

int BasePlayer::GetBlackjackCount() const noexcept {
    return blackjack_count_;
}

int BasePlayer::GetSurrenderCount() const noexcept {
    return surrender_count_;
}

void BasePlayer::ClearCards() {
    cards_.clear();
}

Player::Player(std::string name)
    : BasePlayer(), name_(std::move(name)) {}

const std::string& Player::GetName() const noexcept {
    return name_;
}
