#include"player.h"
#include<raylib.h>
#include<iostream>

Player::Player() {}

void Player::init(int x, int y) {
    this->x = x;
    this->y = y;

    this->height = 70;
    this->width = 35;
    this->speed = 4;
}

void Player::draw() {
    DrawRectangle(this->x, this->y, this->width, this->height, RAYWHITE);
}

void Player::control() {
    if (IsKeyDown(KEY_LEFT)) {
        this->x -= this->speed;
    }

    if (IsKeyDown(KEY_RIGHT)) {
        this->x += this->speed;
    }
}
