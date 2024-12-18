cmake_minimum_required(VERSION 3.20)
project(PlaylistManager)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add main executable and source files
add_executable(PlaylistManager
        src/main.cpp
        src/YouTubeAuth.cpp
        src/SpotifyAuth.cpp
)

# Include directory for custom header files
target_include_directories(PlaylistManager PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Find and include Drogon framework
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(PlaylistManager PRIVATE Drogon::Drogon)

# Find and link Boost.Beast (header-only) and OpenSSL libraries
find_package(OpenSSL REQUIRED)
target_link_libraries(PlaylistManager PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Find and link nlohmann_json
find_package(nlohmann_json REQUIRED)
target_link_libraries(PlaylistManager PRIVATE nlohmann_json)

# Optional: Output build messages for clarity
message(STATUS "Building PlaylistManager with Drogon, Boost.Beast, OpenSSL, and nlohmann_json")

# Enable testing
enable_testing()

# Add test target
add_executable(PlaylistManagerTests
        tests/test_main.cpp
)

# Link GoogleTest to the test executable
find_package(GTest REQUIRED)
target_link_libraries(PlaylistManagerTests PRIVATE GTest::gtest GTest::gtest_main)

# Include test directories (if needed for headers or mocks)
target_include_directories(PlaylistManagerTests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add the tests to CTest
add_test(NAME PlaylistManagerTests COMMAND PlaylistManagerTests)