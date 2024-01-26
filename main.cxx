#include "game.hxx"

int main() {
    Game game;

    if (game.RegisterPlayer()) {
        game.Play();
        return 0;
    }

    return 1;
}
