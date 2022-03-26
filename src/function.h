#ifndef FUNCTION_H
#define FUNCTION_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void PvPOrRAid(int &current_time, int end_time, bool &is_raid, bool is_PvP,
               int &battle_start_time, int &battle_end_time,
               std::vector<int> &num_down_pac, std::vector<int> &down_BW,
               std::vector<std::string> &act);

#endif