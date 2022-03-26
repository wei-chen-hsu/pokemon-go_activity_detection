#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "function.h"

// using namespace std;

void help_message() {
  std::cout << "how to compile: make"
            << "\n";
  std::cout << "how to run: "
               "./bin/activity-detection input/<file_name>.csv "
               "output/ActDetect_<FileName>.csv "
            << "\n";
  std::cout << "how to clean: make clean"
            << "\n";
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    help_message();
    return 0;
  }

  std::fstream fin(argv[1], std::ios::in);
  std::fstream fout(argv[2], std::ios::out);

  fout << "time"
       << ","
       << "activity"
       << "\n";

  /* get the end time of the trace */
  std::string readline;
  std::string str_end_time;
  getline(fin, readline);
  while (getline(fin, readline)) {
    std::string str;
    for (int i = 0; i < readline.size(); i++) {
      if (readline[i] == ',') {
        break;
      }
      str += readline[i];
    }
    if (str == "aggregate") {
      break;
    }
    str_end_time = str;
  }
  int end_time = stoi(str_end_time);

  fin.clear();                 // clear the buffer
  fin.seekg(0, std::ios::beg); // reset the reading position to beginning

  /* read the trace file and get the time-num_packet and time-band_width pairs
   */
  std::vector<int> num_down_pac(stoi(str_end_time) + 1, 0);
  std::vector<int> down_BW(stoi(str_end_time) + 1, 0);
  std::vector<std::string> act(stoi(str_end_time) + 1, "");

  getline(fin, readline);
  while (getline(fin, readline)) {
    std::string str_time;
    std::string str_pac;
    std::string str_BW;
    int num_comma = 0;

    /* put the number of downlink packet and the downlink BW into str */
    for (int i = 0; i < readline.size(); i++) {
      if (readline[i] == ',') {
        num_comma++;
        continue;
      }
      if (num_comma == 0) {
        str_time += readline[i];
        if (str_time == "a") {
          break;
        }
      }
      if (num_comma == 3) {
        str_pac += readline[i];
      }
      if (num_comma == 6) {
        str_BW += readline[i];
      }
    }

    // std::cout << stoi(str_time) << " / " << stoi(str_pac) << " / " <<
    // stoi(str_BW) << " | ";

    if (str_time != "a") { // don't read aggregate and average
      num_down_pac[stoi(str_time)] = stoi(str_pac);
      down_BW[stoi(str_time)] = stoi(str_BW);
    }
  }

  int window_size = 40;
  int window_overlap = 30;
  int window_strat_time = 0;

  while (window_strat_time + window_size <= end_time) {
    int num_idle_signal = 0;
    int current_time = 0;
    bool is_raid = false;
    bool is_PvP = false;
    int battle_start_time = 0;
    int battle_end_time = 0;

    // a window stride //
    for (int i = window_strat_time; i <= window_strat_time + window_size; i++) {
      if ((num_down_pac[i] > 30 && num_down_pac[i] < 65) &&
          (down_BW[i] > 35000 & down_BW[i] < 50000)) {
        num_idle_signal++;
      }
      if (num_down_pac[i] > 200 && down_BW[i] > 65000) {
        if (num_idle_signal >= 1 && num_idle_signal <= 4) {
          for (int j = window_strat_time; j < i; j++) {
            act[j] = "Idle";
          }
        }
        PvPOrRAid(i, end_time, is_raid, is_PvP, battle_start_time,
                  battle_end_time, num_down_pac, down_BW, act);
        window_strat_time = battle_end_time + 1;
        break;
      }
    }

    if (battle_end_time != 0) {
      continue;
    }

    if (num_idle_signal >= 1 && num_idle_signal <= 4) {
      for (int i = window_strat_time; i <= window_strat_time + window_size;
           i++) {
        act[i] = "Idle";
      }
    }

    // renew the window //
    window_strat_time += window_size - window_overlap;
  }

  if (window_strat_time != end_time) {
    for (int i = window_strat_time; i <= end_time; i++) {
      act[i] = "Idle";
    }
  }

  for (int i = 0; i < end_time; i++) {
    fout << i << "," << act[i] << "\n";
  }
  fout << end_time << "," << act[end_time];

  fin.close();
  fout.close();
  return 0;
}