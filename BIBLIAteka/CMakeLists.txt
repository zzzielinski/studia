cmake_minimum_required(VERSION 3.16)

project(BIBLIAteka LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatyczne MOC/UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Znalezienie Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Pliki źródłowe
add_executable(BIBLIAteka MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Linkowanie z Qt
target_link_libraries(BIBLIAteka
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Instalacja plików danych do bundle Resources/data
if(APPLE)
    set(RESOURCE_DIR "${CMAKE_BINARY_DIR}/BIBLIAteka.app/Resources/data")
    file(MAKE_DIRECTORY ${RESOURCE_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/data/spisksiazek.txt DESTINATION ${RESOURCE_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/data/uzytkownicy.txt DESTINATION ${RESOURCE_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/data/wypozyczenia.txt DESTINATION ${RESOURCE_DIR})
endif()
