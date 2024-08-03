#include <vector>

#include "raylib.h"

/**
 * @brief Represents a single square in the Game of Life grid.
 */
class Square {
 public:
  Rectangle rectangle;  ///< The position and size of the square.
  bool is_alive;        ///< Whether the square is alive or not.

  /**
   * @brief Constructs a new Square object.
   *
   * @param rec The position and size of the square.
   */
  Square(const Rectangle& rec) : rectangle(rec), is_alive(false) {}

  /**
   * @brief Switches the state of the square from alive to dead or vice versa.
   */
  void switch_state() { is_alive ^= 1; }
};

/**
 * @brief Represents the Game of Life simulation.
 */
class GameOfLife {
 private:
  enum State {
    DRAW,     ///< The game is in draw mode.
    SIMULATE  ///< The game is in simulation mode.
  };

  std::vector<std::vector<Square>> squares;  ///< The grid of squares.
  unsigned int size;                         ///< The size of the grid.
  unsigned int window_dim;                   ///< The size of the window.
  int previous_x_square;  ///< The x-coordinate of the previously clicked
                          ///< square.
  int previous_y_square;  ///< The y-coordinate of the previously clicked
                          ///< square.
  State state;            ///< The current state of the game.

  /**
   * @brief Initializes the grid of squares.
   */
  void initialize_squares() {
    squares.resize(size);
    int scale = window_dim / size;
    for (std::size_t i = 0; i < squares.size(); i++) {
      squares[i].reserve(size);
      for (std::size_t j = 0; j < squares.size(); j++) {
        squares[i].emplace_back(Rectangle{(float)i * scale, (float)j * scale,
                                          (float)scale, (float)scale});
      }
    }
  }

  /**
   * @brief Draws the grid of squares.
   */
  void draw_squares() {
    for (std::size_t i = 0; i < squares.size(); i++) {
      for (std::size_t j = 0; j < squares[i].size(); j++) {
        Square& square = squares[i][j];
        int size = window_dim / squares.size() + 1;
        if (square.is_alive) {
          DrawRectangle((int)square.rectangle.x, (int)square.rectangle.y, size,
                        size, BLUE);
        } else {
          DrawRectangleLines((int)square.rectangle.x, (int)square.rectangle.y,
                             size, size, BLUE);
        }
      }
    }
  }

  /**
   * @brief Handles user input.
   */
  void handle_input() {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 mouse_position = GetMousePosition();
      int square_size = window_dim / size;
      int x_square = (int)mouse_position.x / square_size;
      int y_square = (int)mouse_position.y / square_size;
      if (x_square == previous_x_square && y_square == previous_y_square)
        return;
      if (x_square < size && y_square < size) {
        squares[x_square][y_square].switch_state();
        previous_x_square = x_square;
        previous_y_square = y_square;
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
  int count_neighbours(int i, int j) {
    int count = 0;
    for (int x = -1; x <= 1; x++) {
      for (int y = -1; y <= 1; y++) {
        if (x == 0 && y == 0) continue;
        int xi = i + x;
        int yj = j + y;
        if (xi >= 0 && xi < (int)squares.size() && yj >= 0 &&
            yj < (int)squares.size()) {
          if (squares[xi][yj].is_alive) {
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
  void simulate() {
    for (std::size_t i = 0; i < squares.size(); i++) {
      for (std::size_t j = 0; j < squares.size(); j++) {
        int count = count_neighbours(i, j);
        if (squares[i][j].is_alive) {
          if (count < 2 || count > 3) {
            squares[i][j].is_alive = false;
          }
        } else {
          if (count == 3) {
            squares[i][j].is_alive = true;
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
      : window_dim(window_dim),
        size(size),
        previous_x_square(-1),
        previous_y_square(-1),
        state(DRAW) {
    initialize_squares();
  }

  /**
   * @brief Runs the Game of Life simulation.
   */
  void run() {
    InitWindow(window_dim, window_dim, "Conway's Game of Life");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
      BeginDrawing();
      {
        ClearBackground(BLACK);
        if (state == DRAW) {
          handle_input();
          draw_squares();
          if (IsKeyPressed(KEY_ENTER)) {
            state = SIMULATE;
          }
        } else {
          simulate();
          draw_squares();
          if (IsKeyPressed(KEY_ENTER)) {
            state = DRAW;
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
  game.run();

  return EXIT_SUCCESS;
}
