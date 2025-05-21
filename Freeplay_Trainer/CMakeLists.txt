cmake_minimum_required(VERSION 3.10)

project(Freeplay_Trainer)

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(Freeplay_Trainer Freeplay_Trainer.h)

target_link_libraries(Freeplay_Trainer PRIVATE nlohmann_json::nlohmann_json)