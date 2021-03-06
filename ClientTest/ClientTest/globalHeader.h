#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/Network.hpp>
#include"../../AsterServer/AsterServer/protocol.h"

#include<vector>
#include<queue>
#include<unordered_set>
#include<unordered_map>

using namespace std;

constexpr int CLIENTWIDHT = 1000;
constexpr int CLIENTHEIGHT = 1000;

constexpr int MAX_BUF_SIZE = 255;

constexpr auto WORLD_WIDTH = 100;
constexpr auto WORLD_HEIGHT = 100;

constexpr auto VIEW_RANGE = 70;