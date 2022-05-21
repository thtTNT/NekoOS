//
// Created by 屠昊天 on 2022/5/20.
//


#pragma once

#pragma section .trap
extern "C" void onTrap() __attribute__((section(".trap")));