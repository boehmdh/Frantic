cmake_minimum_required(VERSION 3.5)
project(Frantic) # your project name

set(CMAKE_CXX_STANDARD 20)

include_directories(sockpp/include)

find_package(wxWidgets COMPONENTS core base net REQUIRED)
include(${wxWidgets_USE_FILE})

# define a variable CLIENT_SOURCE_FILES that contains the paths to all source files required to compile the client executable
set(CLIENT_SOURCE_FILES
        src/client/main.cpp
        src/client/app/Frantic.cpp src/client/app/Frantic.h
        src/client/GameController.cpp src/client/GameController.h
        # UI
        src/client/windows/GameWindow.cpp src/client/windows/GameWindow.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        src/client/panels/ConnectionPanel.cpp src/client/panels/ConnectionPanel.h
        src/client/panels/MainGamePanel.cpp src/client/panels/MainGamePanel.h
        src/client/uiElements/InputField.cpp src/client/uiElements/InputField.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        # network
        src/client/network/ClientNetworkManager.cpp src/client/network/ClientNetworkManager.h
        src/client/network/ResponseListenerThread.cpp src/client/network/ResponseListenerThread.h
        # game state
        src/common/game_state/cards/card.cpp src/common/game_state/cards/card.h
        src/common/game_state/game_state.cpp src/common/game_state/game_state.h
        src/common/game_state/player/hand.cpp src/common/game_state/player/hand.h
        src/common/game_state/player/player.cpp src/common/game_state/player/player.h
        src/common/game_state/cards/draw_pile.cpp src/common/game_state/cards/draw_pile.h
        src/common/game_state/cards/discard_pile.cpp src/common/game_state/cards/discard_pile.h

        # client requests
        src/common/network/requests/client_request.cpp src/common/network/requests/client_request.h
        src/common/network/requests/play_card_request.cpp src/common/network/requests/play_card_request.h
        src/common/network/requests/draw_card_request.cpp src/common/network/requests/draw_card_request.h
        src/common/network/requests/fold_request.cpp src/common/network/requests/fold_request.h
        src/common/network/requests/join_game_request.cpp src/common/network/requests/join_game_request.h
        src/common/network/requests/start_game_request.cpp src/common/network/requests/start_game_request.h
        # server responses
        src/common/network/responses/server_response.cpp src/common/network/responses/server_response.h
        src/common/network/responses/request_response.cpp src/common/network/responses/request_response.h
        src/common/network/responses/full_state_response.cpp src/common/network/responses/full_state_response.h
        # serialization
        src/common/serialization/serializable.h
        src/common/serialization/value_type_helpers.h
        src/common/serialization/vector_utils.h
        src/common/serialization/serializable_value.h
        src/common/serialization/json_utils.h
        src/common/serialization/uuid_generator.h
        src/common/serialization/unique_serializable.cpp src/common/serialization/unique_serializable.h
        src/common/game_state/cards/event_card.h
        src/common/game_state/cards/event_card.cpp
        src/common/game_state/cards/event_draw_pile.h
        src/common/game_state/cards/event_draw_pile.cpp
        src/common/game_state/cards/event_discard_pile.h
        src/common/game_state/cards/event_discard_pile.cpp
        src/common/game_state/cards/event_card_collection.h
        src/common/game_state/cards/color.h
        src/common/game_state/cards/color.cpp
        src/common/game_state/cards/event_card_collection.cpp
        src/common/game_state/cards/card_action_type.cpp
        src/common/game_state/cards/card_action_type.h
)

# define a variable SERVER_SOURCE_FILES that contains the paths to all source files required to compile the server executable
set(SERVER_SOURCE_FILES
        src/server/main.cpp
        src/server/game_instance.cpp src/server/game_instance.h
        src/server/game_instance_manager.cpp src/server/game_instance_manager.h
        src/server/player_manager.cpp src/server/player_manager.h
        src/server/server_network_manager.cpp src/server/server_network_manager.h
        # game state
        src/common/game_state/cards/card.cpp src/common/game_state/cards/card.h
        src/common/game_state/game_state.cpp src/common/game_state/game_state.h
        src/common/game_state/player/hand.cpp src/common/game_state/player/hand.h
        src/common/game_state/player/player.cpp src/common/game_state/player/player.h
        src/common/game_state/cards/draw_pile.cpp src/common/game_state/cards/draw_pile.h
        src/common/game_state/cards/discard_pile.cpp src/common/game_state/cards/discard_pile.h
        
        # client requests
        src/common/network/requests/client_request.cpp src/common/network/requests/client_request.h
        src/common/network/requests/play_card_request.cpp src/common/network/requests/play_card_request.h
        src/common/network/requests/draw_card_request.cpp src/common/network/requests/draw_card_request.h
        src/common/network/requests/fold_request.cpp src/common/network/requests/fold_request.h
        src/common/network/requests/join_game_request.cpp src/common/network/requests/join_game_request.h
        src/common/network/requests/start_game_request.cpp src/common/network/requests/start_game_request.h
        # server responses
        src/common/network/responses/server_response.cpp src/common/network/responses/server_response.h
        src/common/network/responses/request_response.cpp src/common/network/responses/request_response.h
        src/common/network/responses/full_state_response.cpp src/common/network/responses/full_state_response.h
        # serialization
        src/common/serialization/serializable.h
        src/common/serialization/value_type_helpers.h
        src/common/serialization/vector_utils.h
        src/common/serialization/serializable_value.h
        src/common/serialization/json_utils.h
        src/common/serialization/uuid_generator.h
        src/common/serialization/unique_serializable.cpp src/common/serialization/unique_serializable.h src/server/request_handler.h src/server/request_handler.cpp
        src/common/game_state/cards/event_card.h
        src/common/game_state/cards/event_card.cpp
        src/common/game_state/cards/event_draw_pile.h
        src/common/game_state/cards/event_draw_pile.cpp
        src/common/game_state/cards/event_discard_pile.h
        src/common/game_state/cards/event_discard_pile.cpp
        src/common/game_state/cards/event_card_collection.h
        src/common/game_state/cards/color.h
        src/common/game_state/cards/color.cpp
        src/common/game_state/cards/event_card_collection.cpp
        src/common/game_state/cards/card_action_type.cpp
        src/common/game_state/cards/card_action_type.h
)


# set source files for client-executable
add_executable(Frantic-client ${CLIENT_SOURCE_FILES})
# set compile directives for client-executable
target_compile_definitions(Frantic-client PRIVATE FRANTIC_CLIENT=1 RAPIDJSON_HAS_STDSTRING=1)
# link with wxWidgets
target_link_libraries(Frantic-client ${wxWidgets_LIBRARIES})
# Comment out if you don't want to print network-related messages into the console
#target_compile_definitions(Frantic-client PRIVATE PRINT_NETWORK_MESSAGES=1)

# set source files for server-executable
add_executable(Frantic-server ${SERVER_SOURCE_FILES})
# set compile directives for server-executable
target_compile_definitions(Frantic-server PRIVATE FRANTIC_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)
# Comment out if you don't want to print network-related messages into the console
#target_compile_definitions(Frantic-server PRIVATE PRINT_NETWORK_MESSAGES=1)

# build and link sockpp automatically
set(SOCKPP_BUILD_SHARED ON CACHE BOOL "" FORCE)
set(SOCKPP_BUILD_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(sockpp)

if(WIN32)
    target_link_libraries(Frantic-client sockpp-static wsock32 ws2_32)
    target_link_libraries(Frantic-server sockpp-static wsock32 ws2_32)
else()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    target_link_libraries(Frantic-client sockpp Threads::Threads)
    target_link_libraries(Frantic-server sockpp Threads::Threads)
endif()

# linking to sockpp

# copy assets (images) to binary directory
# file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})
add_custom_target (copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
)

add_dependencies(Frantic-client copy_assets)
add_dependencies(Frantic-server copy_assets)

set(CMAKE_CXX_FLAGS "--coverage")
# set source files for server-library
add_library(Frantic-lib ${SERVER_SOURCE_FILES})
# set compile directives for server-library
target_compile_definitions(Frantic-lib PRIVATE FRANTIC_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)
option(COVERAGE "Enable coverage" OFF)

if(COVERAGE)
  add_library(coverage_config INTERFACE)
  target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
  target_link_options(coverage_config INTERFACE --coverage)
endif()
add_subdirectory(Google_Tests)
#add_subdirectory(unit-tests)
