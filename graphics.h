#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "globals.h"

void draw_text(Text &text) {
    Vector2 dimensions = MeasureTextEx(*text.font, text.str.c_str(), text.size * screen_scale, text.spacing);
    Vector2 pos = {
        (screen_size.x * text.position.x) - (0.5f * dimensions.x),
        (screen_size.y * text.position.y) - (0.5f * dimensions.y)
    };

    DrawTextEx(*text.font, text.str.c_str(), pos, dimensions.y, text.spacing, text.color);
}

void derive_graphics_metrics_from_loaded_level() {
    screen_size.x  = static_cast<float>(GetScreenWidth());
    screen_size.y = static_cast<float>(GetScreenHeight());

    cell_size = std::min(
        screen_size.x / static_cast<float>(current_level.columns),
        screen_size.y / static_cast<float>(current_level.rows)
    ) * CELL_SCALE;
    screen_scale = std::min(
        screen_size.x,
        screen_size.y
    ) / SCREEN_SCALE_DIVISOR;
    float level_width  = static_cast<float>(current_level.columns) * cell_size;
    float level_height = static_cast<float>(current_level.rows)    * cell_size;
    shift_to_center.x = (screen_size.x - level_width) * 0.5f;
    shift_to_center.y = (screen_size.y - level_height) * 0.5f;
}

void draw_menu() {
    draw_text(game_title);
    draw_text(game_subtitle);
}

void draw_defeat_screen() {
    draw_text(defeat_title);
}

void draw_game_overlay() {
    Text score = {
        "Score " + std::to_string(player_score),
        { 0.50f, 0.05f },
        48.0f
    };
    Text score_shadow = {
        "Score " + std::to_string(player_score),
        { 0.503f, 0.055f },
        48.0f,
        GRAY
    };

    draw_text(score_shadow);
    draw_text(score);
}

void draw_level() {
    for (size_t row = 0; row < current_level.rows; ++row) {
        for (size_t column = 0; column < current_level.columns; ++column) {

            Vector2 pos = {
                    shift_to_center.x + static_cast<float>(column) * cell_size,
                    shift_to_center.y + static_cast<float>(row) * cell_size
            };

            char cell = current_level.data[row * current_level.columns + column];
            // The first image layer
            switch (cell) {
                case AIR:
                case PLAYER:
                case COIN:
                case EXIT:
                    draw_image(air_image, pos, cell_size);
                    break;
                case WALL:
                    draw_image(wall_image, pos, cell_size);
                    break;
                case SPIKES:
                    draw_image(spikes_image, pos, cell_size);
                    break;
                case DOOR:
                    if (!is_door_open) {
                        draw_image(door_image, pos, cell_size);
                        break;
                    }
            }
            // The second image layer
            switch (cell) {
                case COIN:
                    draw_sprite(coin_sprite, pos, cell_size);
                    break;
                case EXIT:
                    draw_image(exit_image, pos, cell_size);
                    break;
                case JUMP_BOOST_POTION:
                    draw_image(jump_boost_potion_image, pos, cell_size);
                    break;
                case SKELETON:
                    draw_image(skeleton_image, pos, cell_size);
                    break;
                case SWORD:
                        draw_image(sword_image, pos, cell_size);
                    break;
                case LEVER1:
                     if (lever_activation1 == true) {
                        draw_image(lever_activated_image, pos, cell_size);
                        break;
                    }
                    draw_image(lever_unactivated_image, pos, cell_size);
                    break;
                case LEVER2:
                     if (lever_activation2 == true) {
                        draw_image(lever_activated_image, pos, cell_size);
                        break;
                    }
                    draw_image(lever_unactivated_image, pos, cell_size);
                    break;
                case LEVER3:
                     if (lever_activation3 == true) {
                        draw_image(lever_activated_image, pos, cell_size);
                        break;
                    }
                    draw_image(lever_unactivated_image, pos, cell_size);
                    break;
                case GRAAL:
                    draw_image(graal_image, pos, cell_size);
                    break;
                default:
                    break;
            }
        }
    }

    draw_player();
}

void draw_player() {
    Vector2 pos = {
        shift_to_center.x + player_pos.x * cell_size,
        shift_to_center.y + player_pos.y * cell_size
    };

    draw_sprite(player_sprite, pos, cell_size);
}

void draw_pause_menu() {
    draw_text(game_paused);
}

void draw_pause_hint_menu() {
    draw_text(game_paused_for_hint);
}

void create_victory_menu_background() {
    for (auto &ball : victory_balls) {
        ball.x  = rand_up_to(screen_size.x);
        ball.y  = rand_up_to(screen_size.y);
        ball.dx = rand_from_to(-VICTORY_BALL_MAX_SPEED, VICTORY_BALL_MAX_SPEED);
        ball.dx *= screen_scale;
        if (abs(ball.dx) < 0E-1) ball.dx = 1.0f;
        ball.dy = rand_from_to(-VICTORY_BALL_MAX_SPEED, VICTORY_BALL_MAX_SPEED);
        ball.dy *= screen_scale;
        if (abs(ball.dy) < 0E-1) ball.dy = 1.0f;
        ball.radius = rand_from_to(VICTORY_BALL_MIN_RADIUS, VICTORY_BALL_MAX_RADIUS);
        ball.radius *= screen_scale;
    }

    /* Clear both the front buffer and the back buffer to avoid ghosting of the game graphics. */
    ClearBackground(BLACK);
    EndDrawing();
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
    BeginDrawing();
}

void animate_victory_menu_background() {
    for (auto &ball : victory_balls) {
        ball.x += ball.dx;
        if (ball.x - ball.radius < 0 ||
            ball.x + ball.radius >= screen_size.x) {
            ball.dx = -ball.dx;
        }
        ball.y += ball.dy;
        if (ball.y - ball.radius < 0 ||
            ball.y + ball.radius >= screen_size.y) {
            ball.dy = -ball.dy;
        }
    }
}

void draw_victory_menu_background() {
    for (auto &ball : victory_balls) {
        DrawCircleV({ ball.x, ball.y }, ball.radius, VICTORY_BALL_COLOR);
    }
}

void draw_victory_menu() {
    DrawRectangle(
        0, 0,
        static_cast<int>(screen_size.x), static_cast<int>(screen_size.y),
        { 0, 0, 0, VICTORY_BALL_TRAIL_TRANSPARENCY }
    );

    animate_victory_menu_background();
    draw_victory_menu_background();

    draw_text(victory_title);
    draw_text(victory_subtitle);
}

#endif // GRAPHICS_H
