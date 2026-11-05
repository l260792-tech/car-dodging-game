#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

const int WINDOW_W = 800;
const int WINDOW_H = 700;
const float ROAD_LEFT = 180.f;
const float ROAD_WIDTH = 440.f;
const float LANE_W = ROAD_WIDTH / 3.f;

enum class State { Menu, Instructions, Playing, Paused, GameOver };
enum class Difficulty { Easy, Medium, Hard };

struct Enemy {
    sf::RectangleShape body;
    float speed;
    bool passed;
};

float laneCenter(int lane) {
    return ROAD_LEFT + LANE_W * (lane + 0.5f);
}

void centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f,
                   bounds.top + bounds.height / 2.f);
    text.setPosition(x, y);
}

void saveHighScore(int score, int& highScore) {
    if (score > highScore) {
        highScore = score;
        std::ofstream file("highscore.txt");
        if (file) file << highScore;
    }
}

int loadHighScore() {
    std::ifstream file("highscore.txt");
    int value = 0;
    if (file) file >> value;
    return value;
}

void resetGame(int& lane, float& playerY, std::vector<Enemy>& enemies,
               float& spawnTimer, float& roadOffset, int& score) {
    lane = 1;
    playerY = WINDOW_H - 125.f;
    enemies.clear();
    spawnTimer = 0.f;
    roadOffset = 0.f;
    score = 0;
}

float trafficSpeed(Difficulty difficulty, int score) {
    float base = 260.f;
    if (difficulty == Difficulty::Easy) base = 210.f;
    if (difficulty == Difficulty::Hard) base = 330.f;
    return base + std::min(score * 1.5f, 220.f);
}

float spawnInterval(Difficulty difficulty, int score) {
    float interval = 1.15f;
    if (difficulty == Difficulty::Easy) interval = 1.45f;
    if (difficulty == Difficulty::Hard) interval = 0.85f;
    return std::max(0.42f, interval - score * 0.002f);
}

void drawRoad(sf::RenderWindow& window, float offset) {
    sf::RectangleShape grass(sf::Vector2f(WINDOW_W, WINDOW_H));
    grass.setFillColor(sf::Color(35, 115, 55));
    window.draw(grass);

    sf::RectangleShape road(sf::Vector2f(ROAD_WIDTH, WINDOW_H));
    road.setPosition(ROAD_LEFT, 0.f);
    road.setFillColor(sf::Color(48, 48, 54));
    window.draw(road);

    sf::RectangleShape edge(sf::Vector2f(8.f, WINDOW_H));
    edge.setFillColor(sf::Color::White);
    edge.setPosition(ROAD_LEFT, 0.f);
    window.draw(edge);
    edge.setPosition(ROAD_LEFT + ROAD_WIDTH - 8.f, 0.f);
    window.draw(edge);

    sf::RectangleShape dash(sf::Vector2f(5.f, 42.f));
    dash.setFillColor(sf::Color(235, 235, 235));
    for (int divider = 1; divider <= 2; ++divider) {
        for (int y = -60; y < WINDOW_H; y += 90) {
            dash.setPosition(ROAD_LEFT + LANE_W * divider - 2.5f,
                             static_cast<float>(y) + offset);
            window.draw(dash);
        }
    }
}

void drawCar(sf::RenderWindow& window, const sf::RectangleShape& car,
             sf::Color windshieldColor) {
    window.draw(car);
    sf::RectangleShape windshield(sf::Vector2f(25.f, 24.f));
    windshield.setFillColor(windshieldColor);
    windshield.setPosition(car.getPosition().x + 7.f,
                           car.getPosition().y + 13.f);
    window.draw(windshield);

    sf::RectangleShape stripe(sf::Vector2f(5.f, 48.f));
    stripe.setFillColor(sf::Color(245, 245, 245, 180));
    stripe.setPosition(car.getPosition().x + 17.f,
                       car.getPosition().y + 38.f);
    window.draw(stripe);
}

void drawLabel(sf::RenderWindow& window, sf::Font& font,
               const std::string& value, unsigned size,
               float x, float y, sf::Color color = sf::Color::White) {
    sf::Text text(value, font, size);
    text.setFillColor(color);
    centerText(text, x, y);
    window.draw(text);
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H),
                            "Group 5 - Car Dodging Game");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            return 1;
        }
    }

    State state = State::Menu;
    Difficulty difficulty = Difficulty::Medium;
    int lane = 1;
    float playerY = WINDOW_H - 125.f;
    float roadOffset = 0.f;
    float spawnTimer = 0.f;
    int score = 0;
    int highScore = loadHighScore();
    std::vector<Enemy> enemies;
    sf::Clock clock;

    sf::RectangleShape player(sf::Vector2f(40.f, 70.f));
    player.setFillColor(sf::Color(35, 145, 255));
    player.setOutlineThickness(2.f);
    player.setOutlineColor(sf::Color::White);

    while (window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (state == State::Menu) {
                    if (event.key.code == sf::Keyboard::Num1) difficulty = Difficulty::Easy;
                    if (event.key.code == sf::Keyboard::Num2) difficulty = Difficulty::Medium;
                    if (event.key.code == sf::Keyboard::Num3) difficulty = Difficulty::Hard;
                    if (event.key.code == sf::Keyboard::Enter) {
                        resetGame(lane, playerY, enemies, spawnTimer, roadOffset, score);
                        state = State::Playing;
                    }
                    if (event.key.code == sf::Keyboard::I) state = State::Instructions;
                } else if (state == State::Instructions) {
                    if (event.key.code == sf::Keyboard::Escape) state = State::Menu;
                } else if (state == State::Playing) {
                    if (event.key.code == sf::Keyboard::Left && lane > 0) --lane;
                    if (event.key.code == sf::Keyboard::Right && lane < 2) ++lane;
                    if (event.key.code == sf::Keyboard::P) state = State::Paused;
                } else if (state == State::Paused) {
                    if (event.key.code == sf::Keyboard::P) state = State::Playing;
                    if (event.key.code == sf::Keyboard::M) state = State::Menu;
                } else if (state == State::GameOver) {
                    if (event.key.code == sf::Keyboard::R) {
                        resetGame(lane, playerY, enemies, spawnTimer, roadOffset, score);
                        state = State::Playing;
                    }
                    if (event.key.code == sf::Keyboard::M) state = State::Menu;
                }
            }
        }

        if (state == State::Playing) {
            float speed = trafficSpeed(difficulty, score);
            roadOffset += speed * dt;
            if (roadOffset >= 90.f) roadOffset -= 90.f;

            spawnTimer += dt;
            if (spawnTimer >= spawnInterval(difficulty, score)) {
                spawnTimer = 0.f;
                int enemyLane = std::rand() % 3;
                Enemy enemy;
                enemy.body.setSize(sf::Vector2f(40.f, 70.f));
                enemy.body.setFillColor(sf::Color(220, 65, 65));
                enemy.body.setOutlineThickness(2.f);
                enemy.body.setOutlineColor(sf::Color::White);
                enemy.body.setPosition(laneCenter(enemyLane) - 20.f, -85.f);
                enemy.speed = speed;
                enemy.passed = false;
                enemies.push_back(enemy);
            }

            player.setPosition(laneCenter(lane) - 20.f, playerY);
            sf::FloatRect playerBounds = player.getGlobalBounds();

            for (auto& enemy : enemies) {
                enemy.body.move(0.f, enemy.speed * dt);
                if (!enemy.passed && enemy.body.getPosition().y > playerY + 70.f) {
                    enemy.passed = true;
                    ++score;
                    saveHighScore(score, highScore);
                }
                if (playerBounds.intersects(enemy.body.getGlobalBounds())) {
                    state = State::GameOver;
                    saveHighScore(score, highScore);
                }
            }

            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& enemy) {
                    return enemy.body.getPosition().y > WINDOW_H + 90.f;
                }), enemies.end());
        }

        window.clear();
        if (state == State::Menu) {
            drawRoad(window, 0.f);
            drawLabel(window, font, "CAR DODGING GAME", 42, 400.f, 150.f,
                      sf::Color(255, 220, 80));
            drawLabel(window, font, "GROUP 5  |  C++ AND SFML", 20, 400.f, 205.f);
            drawLabel(window, font, "1 - EASY     2 - MEDIUM     3 - HARD", 22, 400.f, 300.f);
            std::string selected = difficulty == Difficulty::Easy ? "EASY" :
                                  difficulty == Difficulty::Hard ? "HARD" : "MEDIUM";
            drawLabel(window, font, "Selected: " + selected, 22, 400.f, 345.f,
                      sf::Color(130, 230, 150));
            drawLabel(window, font, "ENTER - START GAME", 24, 400.f, 425.f);
            drawLabel(window, font, "I - INSTRUCTIONS", 22, 400.f, 470.f);
            drawLabel(window, font, "HIGH SCORE: " + std::to_string(highScore),
                      20, 400.f, 555.f);
        } else if (state == State::Instructions) {
            drawRoad(window, 0.f);
            drawLabel(window, font, "HOW TO PLAY", 38, 400.f, 150.f,
                      sf::Color(255, 220, 80));
            drawLabel(window, font, "LEFT / RIGHT ARROWS: CHANGE LANES", 20, 400.f, 260.f);
            drawLabel(window, font, "AVOID THE RED TRAFFIC CARS", 20, 400.f, 315.f);
            drawLabel(window, font, "EACH CAR PASSED ADDS ONE POINT", 20, 400.f, 370.f);
            drawLabel(window, font, "P: PAUSE DURING THE GAME", 20, 400.f, 425.f);
            drawLabel(window, font, "ESC: RETURN TO MENU", 20, 400.f, 480.f);
        } else {
            drawRoad(window, roadOffset);
            player.setPosition(laneCenter(lane) - 20.f, playerY);
            drawCar(window, player, sf::Color(100, 210, 255));
            for (const auto& enemy : enemies) {
                drawCar(window, enemy.body, sf::Color(255, 170, 170));
            }
            drawLabel(window, font, "SCORE: " + std::to_string(score),
                      22, 95.f, 35.f, sf::Color::Yellow);
            drawLabel(window, font, "BEST: " + std::to_string(highScore),
                      20, 95.f, 70.f);
            if (state == State::Paused) {
                drawLabel(window, font, "PAUSED", 48, 400.f, 300.f,
                          sf::Color::Yellow);
                drawLabel(window, font, "P - RESUME     M - MENU", 22, 400.f, 365.f);
            }
            if (state == State::GameOver) {
                sf::RectangleShape shade(sf::Vector2f(WINDOW_W, WINDOW_H));
                shade.setFillColor(sf::Color(0, 0, 0, 175));
                window.draw(shade);
                drawLabel(window, font, "GAME OVER", 48, 400.f, 260.f,
                          sf::Color(255, 100, 100));
                drawLabel(window, font, "FINAL SCORE: " + std::to_string(score),
                          26, 400.f, 330.f);
                drawLabel(window, font, "R - RESTART     M - MAIN MENU",
                          22, 400.f, 400.f);
            }
        }
        window.display();
    }
    return 0;
}
