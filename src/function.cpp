#include "function.h"

void PvPOrRAid(int &current_time, int end_time, bool &is_raid, bool is_PvP,
               int &battle_start_time, int &battle_end_time,
               std::vector<int> &num_down_pac, std::vector<int> &down_BW,
               std::vector<std::string> &act) {
  int window_size = 40;
  int window_overlap = 30;
  battle_start_time = current_time;

  for (current_time; current_time < battle_start_time + 30; current_time++) {
    if ((num_down_pac[current_time] > 30 && num_down_pac[current_time] < 65) &&
        (down_BW[current_time] > 35000 & down_BW[current_time] < 50000)) {
      is_raid = true;
    }
  }
  if (is_raid == false) {
    is_PvP = true;
  }

  if (is_PvP == true) {
    for (current_time; current_time <= end_time; current_time++) {
      if ((num_down_pac[current_time] > 30 &&
           num_down_pac[current_time] < 65) &&
          (down_BW[current_time] > 35000 & down_BW[current_time] < 50000)) {
        battle_end_time = current_time - 1;
        break;
      }
    }

    for (int i = battle_start_time; i <= battle_end_time; i++) {
      act[i] = "PvP";
    }
    return;
  }
  if (is_raid == true) {
    int idle_not_found_time = 0;
    while (current_time <= end_time) {
      current_time++;
      idle_not_found_time++;
      if ((num_down_pac[current_time] > 30 &&
           num_down_pac[current_time] < 65) &&
          (down_BW[current_time] > 35000 & down_BW[current_time] < 50000)) {
        if (idle_not_found_time >= 40) {
          battle_end_time = current_time;
          for (int i = battle_start_time; i <= battle_end_time; i++) {
            act[i] = "Raid";
          }
          return;
        }
        idle_not_found_time = 0;
      }
    }
  }
}