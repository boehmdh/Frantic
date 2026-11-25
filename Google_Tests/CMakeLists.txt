# 'Google_Tests is the subproject name
project(Google_Tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TEST_SOURCE_FILES
        ../src/common/game_state/cards/event_card.h
        ../src/common/game_state/cards/event_card.cpp
        ../src/common/game_state/cards/event_draw_pile.h
        ../src/common/game_state/cards/event_draw_pile.cpp
        ../src/common/game_state/cards/event_discard_pile.h
        ../src/common/game_state/cards/event_discard_pile.cpp
        ../src/common/game_state/cards/event_card_collection.h
        ../src/common/game_state/cards/color.h
        ../src/common/game_state/cards/color.cpp
        ../src/common/game_state/cards/event_card_collection.cpp
        ../src/common/game_state/cards/card_action_type.h
        cardTest.cpp
        handTest.cpp
        playerTest.cpp
        deckOrderTest.cpp
        stateBroadcastConsistencyTest.cpp
        eventCardTests.cpp
        equalityCardTest.cpp
)

add_executable(Google_Tests_run ${TEST_SOURCE_FILES})

target_compile_definitions(Google_Tests_run PRIVATE FRANTIC_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

target_link_libraries(Google_Tests_run
  PRIVATE
    gtest
    gtest_main
    Frantic-lib
)

if(COVERAGE)
  target_link_libraries(Google_Tests_run PRIVATE coverage_config)
endif()


