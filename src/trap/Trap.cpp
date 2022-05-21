//
// Created by 屠昊天 on 2022/5/20.
//

#include "Trap.h"
#include "../devices/render.h"


extern "C" void onTrap() {
    Render::print("TRAP!");
}
