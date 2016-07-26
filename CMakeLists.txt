cmake_minimum_required(VERSION 3.5)

project(boinc-rpc-cpp)
set (VERSION 1.0)

find_package (PkgConfig REQUIRED)
find_package (Boost REQUIRED)

pkg_check_modules (GLIBMM REQUIRED glibmm-2.4)
pkg_check_modules (LIBXMLMM REQUIRED libxml++-2.6)

add_subdirectory(src)
