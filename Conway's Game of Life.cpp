#include <vector>

#include "raylib.h"

/**
 * @brief Represents the Game of Life simulation.
 */
class GameOfLife {
 private:
  enum State {
    DRAW,     ///< The game is in draw mode.
    SIMULATE  ///< The game is in simulation mode.
  };

  /**
   * @brief Represents a single square in the Game of Life grid.
   */
  class Square {
   public:
    Rectangle rectangle_;  ///< The position and size of the square.
    bool is_alive_;        ///< Whether the square is alive or not.

    /**
     * @brief Constructs a new Square object.
     *
     * @param rec The position and size of the square.
     */
    Square(const Rectangle& rec) : rectangle_(rec), is_alive_(false) {}

    /**
     * @brief Switches the state of the square from alive to dead or vice versa.
     */
    void SwitchState() { is_alive_ ^= 1; }
  };

  std::vector<std::vector<Square>> squares_;  ///< The grid of squares.
  unsigned int window_dim_;                   ///< The size of the window.
  unsigned int size_;                         ///< The size of the grid.
  int previous_x_square_;  ///< The x-coordinate of the previously clicked
                           ///< square.
  int previous_y_square_;  ///< The y-coordinate of the previously clicked
                           ///< square.
  State state_;            ///< The current state of the game.

  /**
   * @brief Initializes the grid of squares.
   */
  void InitializeSquares() {
    squares_.resize(size_);
    int scale = window_dim_ / size_;
    for (std::size_t i = 0; i < squares_.size(); i++) {
      squares_[i].reserve(size_);
      for (std::size_t j = 0; j < squares_.size(); j++) {
        squares_[i].emplace_back(Rectangle{(float)i * scale, (float)j * scale,
                                           (float)scale, (float)scale});
      }
    }
  }

  /**
   * @brief Draws the grid of squares.
   */
  void DrawSquares() {
    for (std::size_t i = 0; i < squares_.size(); i++) {
      for (std::size_t j = 0; j < squares_[i].size(); j++) {
        Square& square = squares_[i][j];
        int size = window_dim_ / squares_.size() + 1;
        if (square.is_alive_) {
          DrawRectangle((int)square.rectangle_.x, (int)square.rectangle_.y,
                        size, size, BLUE);
        } else {
          DrawRectangleLines((int)square.rectangle_.x, (int)square.rectangle_.y,
                             size, size, BLUE);
        }
      }
    }
  }

  /**
   * @brief Handles user input.
   */
  void HandleInput() {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 mouse_position = GetMousePosition();
      int square_size = window_dim_ / size_;
      int x_square = (int)mouse_position.x / square_size;
      int y_square = (int)mouse_position.y / square_size;
      if (x_square == previous_x_square_ && y_square == previous_y_square_)
        return;
      if (x_square < (int)size_ && y_square < (int)size_) {
        squares_[x_square][y_square].SwitchState();
        previous_x_square_ = x_square;
        previous_y_square_ = y_square;
      }
    }
  }

  /**
   * @brief Counts the number of alive neighbors for a given square.
   *
   * @param i The x-coordinate of the square.
   * @param j The y-coordinate of the square.
   * @return The number of alive neighbors.
   */
  int CountNeighbours(int i, int j) {
    int count = 0;
    for (int x = -1; x <= 1; x++) {
      for (int y = -1; y <= 1; y++) {
        if (x == 0 && y == 0) continue;
        int xi = i + x;
        int yj = j + y;
        if (xi >= 0 && xi < (int)squares_.size() && yj >= 0 &&
            yj < (int)squares_.size()) {
          if (squares_[xi][yj].is_alive_) {
            count++;
          }
        }
      }
    }
    return count;
  }

  /**
   * @brief Simulates one step of the Game of Life.
   */
  void Simulate() {
    for (std::size_t i = 0; i < squares_.size(); i++) {
      for (std::size_t j = 0; j < squares_.size(); j++) {
        int count = CountNeighbours(i, j);
        if (squares_[i][j].is_alive_) {
          if (count < 2 || count > 3) {
            squares_[i][j].is_alive_ = false;
          }
        } else {
          if (count == 3) {
            squares_[i][j].is_alive_ = true;
          }
        }
      }
    }
  }

 public:
  /**
   * @brief Constructs a new GameOfLife object.
   *
   * @param window_dim The size of the window.
   * @param size The size of the grid.
   */
  GameOfLife(unsigned int window_dim, unsigned int size)
      : window_dim_(window_dim),
        size_(size),
        previous_x_square_(-1),
        previous_y_square_(-1),
        state_(DRAW) {
    InitializeSquares();
  }

  /**
   * @brief Runs the Game of Life simulation.
   */
  void Run() {
    InitWindow(window_dim_, window_dim_, "Conway's Game of Life");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
      BeginDrawing();
      {
        ClearBackground(BLACK);
        if (state_ == DRAW) {
          HandleInput();
          DrawSquares();
          if (IsKeyPressed(KEY_ENTER)) {
            state_ = SIMULATE;
          }
        } else {
          Simulate();
          DrawSquares();
          if (IsKeyPressed(KEY_ENTER)) {
            state_ = DRAW;
          }
          WaitTime(0.1);
        }
      }
      EndDrawing();
    }
    CloseWindow();
  }
};

int main() {
  GameOfLife game(801, 20);
  game.Run();
}
