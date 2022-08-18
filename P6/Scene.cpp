/*******************************
    NYU Tandon - CS 3113
    Project6 - God-Me
       - Scene.cpp -
       By Cody Jiang
         8/18/2022
       zj2247@nyu.edu
*******************************/

#include "Scene.h"

void Scene::lose_life() {
    state.player_lives -= 1;
}