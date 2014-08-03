//GlobalGameState.h

// Controls the current state the game is in, which should be used in every class, that needs to 
// change dependant on the game state

// State change responsability (I FEEL THIS IS BAD )
// Title -> Start Lobby = Input ( Press any key )
// Start Lobby -> Race = MenuTimer ( 30 second timer )

// Race -> End Race = PlayerController ( Passed Finish Line )
// End Race -> Race = MenuTimer ( 30 second timer )
// Race -> End Race = PlayerController ( Passed Finish Line )
// End Race -> Race = MenuTimer ( 30 second timer )
// Race -> Final Lobby = PlayerController ( Passed Finish Line )

// Final Lobby -> Reset = Input ( Press any key )

#ifndef INCLUDE_GUARD_GLOBAL_GAME_STATE_H
#define INCLUDE_GUARD_GLOBAL_GAME_STATE_H

namespace GlobalGameState
{
	namespace SimulationStates
	{
		enum
		{
			INIT		= 0,
			RACE		= 1,
			END_RACE	= 2,
			RESET		= 3
		};
	}

	//Arcade states
	enum 
	{
		TITLE		= 0,
		START_LOBBY = 1,
		RACE		= 2,
		END_RACE	= 3,
		FINAL_LOBBY = 4,
		RESET		= 5
	};

	extern int g_globalGameState;
	extern int g_globalRaceNumber;
};
	
#endif