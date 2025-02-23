cmake_minimum_required(VERSION 3.30)
project(embedded C)

set(CMAKE_C_STANDARD 11)

add_executable(embedded main.c
        arm_task_scheduler.c
        pc_task_scheduler_with_data_persistence.c
        ../../Users/Bernát/AppData/Roaming/JetBrains/CLion2024.3/extensions/com.intellij.database/schema/tmp.c)
