cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(noop_macro_cause_unused_var)

target_sources(app PRIVATE main.c)
