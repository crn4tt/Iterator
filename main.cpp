cmake_minimum_required(VERSION 3.14)
project(Patient_Queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ────────────────────────────────
# 1. Подтягиваем libpqxx
# ────────────────────────────────
include(FetchContent)

FetchContent_Declare(
  libpqxx
  GIT_REPOSITORY https://github.com/jtv/libpqxx.git
  GIT_TAG        7.9.0            # при желании поменяй версию
)

# Собираем и подключаем
FetchContent_MakeAvailable(libpqxx)  # <-- ОБЯЗАТЕЛЬНО!

# ────────────────────────────────
# 2. Исходники твоего проекта
# ────────────────────────────────
set(SOURCES
    src/DataBaseWorker.cpp
    src/Patient.cpp
    src/Visit.cpp
    src/PatientQueue.cpp
    src/Doctor.cpp
    src/main.cpp
)

add_executable(Patient_Queue ${SOURCES})

# ────────────────────────────────
# 3. Инклуды
# ────────────────────────────────
target_include_directories(Patient_Queue
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include   # твои собственные хедеры
)

# Заголовки libpqxx добавлять НЕ нужно –
# цель `pqxx` уже экспортирует свой include/ каталог.

# ────────────────────────────────
# 4. Линковка
# ────────────────────────────────
target_link_libraries(Patient_Queue
  PRIVATE
    pqxx   # построенная FetchContent-ом libpqxx
    pq     # системная libpq из пакета libpq-dev
)