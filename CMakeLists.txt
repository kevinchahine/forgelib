cmake_minimum_required(VERSION 3.20)

set(LIBRARY_NAME forgelib)
project(${LIBRARY_NAME})

# --- Source Files ---

set(FEATURE_EXTRACTOR
	forge/feature_extractor/AttackChecker.cpp
	forge/feature_extractor/AttackChecker.h
	forge/feature_extractor/Attackers.cpp
	forge/feature_extractor/AttackersDefinitions.h
	forge/feature_extractor/Attackers.h
	forge/feature_extractor/KingAttacker.cpp
	forge/feature_extractor/KingAttacker.h
	forge/feature_extractor/KingAttackers.cpp
	forge/feature_extractor/KingAttackersDefinitions.h
	forge/feature_extractor/KingAttackers.h
	forge/feature_extractor/PinDefinitions.h
	forge/feature_extractor/Pin.h
	forge/feature_extractor/Threats.cpp
	forge/feature_extractor/ThreatsDefinitions.h
	forge/feature_extractor/Threats.h
)

set(CORE
	forge/core/BitBoard.cpp
	forge/core/BitBoard.h
	forge/core/Board.cpp
	forge/core/Board.h
	forge/core/BoardSquare.cpp
	forge/core/BoardSquare.h
	forge/core/Color.cpp
	forge/core/Color.h
	forge/core/Direction.cpp
	forge/core/Direction.h
	forge/core/FiftyMoveRule.cpp
	forge/core/FiftyMoveRule.h
	forge/core/game_history.cpp
	forge/core/game_history.h
	forge/core/GameState.cpp
	forge/core/GameState.h
	forge/core/HashCombine.h
	forge/core/IntBoard.cpp
	forge/core/IntBoard.h	
	forge/core/MoveCounter.h
	forge/core/Move.cpp
	forge/core/Move.h
	forge/core/MovePositionPair.cpp
	forge/core/MovePositionPair.h
	forge/core/Movers.h
	forge/core/Node.cpp
	forge/core/Node.h
	forge/core/Piece.cpp
	forge/core/Piece.h
	forge/core/Position.cpp
	forge/core/Position.h
	forge/core/MoveGenerator2.cpp
	forge/core/MoveGenerator2_Definitions.h
	forge/core/MoveGenerator2.h
	forge/core/MoveList.cpp
	forge/core/MoveList.h	
)

set(TIME
	forge/time/clock.cpp
	forge/time/clock.h
	forge/time/stopwatch.cpp
	forge/time/stopwatch.h
	forge/time/timer.cpp
	forge/time/timer.h
)

# --- Make this a library (not an app) ---

add_library(${LIBRARY_NAME} STATIC
	${FEATURE_EXTRACTOR}	
	${CORE}
	${TIME}
)

# --- Create Folders for VS ---

source_group(feature_extractor FILES ${FEATURE_EXTRACTOR})
source_group(core FILES ${CORE})
source_group(time FILES ${TIME})

include_directories(.)

target_include_directories(${LIBRARY_NAME} PUBLIC guten)

# --- Compiler Flags ---
set(CMAKE_CXX_STANDARD 17)												# TODO: Do we need both?
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 17)
set_target_properties(${LIBRARY_NAME} PROPERTIES CXX_STANDARD 17)
target_compile_features(${LIBRARY_NAME} PRIVATE cxx_std_17)

# --- Set Preprocessor Macros ---
add_compile_definitions(NOMINMAX)	# same as '#define NOMINMAX'

# --------------------------- Libraries ---------------------------------------

# --- Guten ---
message(STATUS "\n----- Guten -----\n")

target_link_libraries(${PROJECT_NAME} PUBLIC guten)

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})	# auto include headers for linking projects 
