#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"
#include <stdlib.h>

#define TESTCARD "minion"
#define NUMTESTS 1000

int main() {
	//seed rand
	srand(time(NULL));

	//success and test counts
	int numSuccess = 0;
	int testCount = 0;

	int i, j;

	//initialize relevant card variables
	//most variables remain here for reusability, but unused variables are commented out
	//some of these values will change depending on the test and card effect choice
	int drawnCards = 0;
	int discarded = 0;
	int netCoins = 0;
	int shuffledCards = 0;
	//int netBuys = 1;
	int netActions = 1;
	//int netSupply = 0;

	//initialize cardEffect() variables
	int handPos = 0, choice1 = 0, choice2 = 0, choice3 = 0, bonus = 0;
	
	//variables for initializeGame()
	int seed;
	int numPlayers;
	int currentPlayer;

	//decides if an estate will be placed
	int placeEstate;

	//two game states used to compare card effects
	struct gameState preState, postState;

	//initialize card array
	//i am deciding to keep the card array consistent for each test
	int k[10] = {adventurer, minion, ambassador, gardens, mine, remodel, smithy, tribute, baron, great_hall};

	printf("----------------- Testing Card: %s -----------------\n\n", TESTCARD);

	//Randomize the gamestate
	for (i = 0; i < NUMTESTS; i++)
	{
		//randomize 1-4 players
		numPlayers = (rand() % 4) + 1;
		//randomize seed
		seed = rand() % 999999999;
		//randomize current player
		currentPlayer = rand() % numPlayers;

		//initialize a game using pregame state
		initializeGame(numPlayers, k, seed, &preState);

		//randomize deck size of all players
		for (j = 0; j < numPlayers; j++)
		{
			preState.deckCount[j] = rand() % 30;
		}

		//randomize hand count, 1-5, so that there will always be at least 1 card in hand
		//since playing baron is not possible if it was not in your hand
		preState.handCount[currentPlayer] = (rand() % 5) + 1;

		//randomize hand count of all players from 1-5
		for (j = 0; j < numPlayers; j++)
		{
			preState.handCount[j] = (rand() % 5) + 1;
		}

		//randomize player choice
		choice1 = rand() % 2;
		if (choice1 == 0)
		{
			//change choice variables based on card effects
			choice2 = 1;
			discarded = preState.handCount[currentPlayer];
			drawnCards = 4;
			netCoins = 0;
			netActions = 1;
		}
		else if (choice1 == 1)
		{
			//change choice variables based on card effects
			choice2 = 0;
			discarded = 0;
			drawnCards = 0;
			netCoins = 2;
			netActions = 1;
		}

		//copy pregame state over to post game
		memcpy(&postState, &preState, sizeof (struct gameState));

		//call the card
		cardEffect(minion, choice1, choice2, choice3, &postState, handPos, &bonus);

		//test handcount
		testCount++;
		if (postState.handCount[currentPlayer] == preState.handCount[currentPlayer] - discarded + drawnCards)
		{
			numSuccess++;
		}
		else
		{
			printf("TEST #%d FAILED: current player incorrect handcount\n", testCount);
		}

		//test deckcount
		testCount++;
		if (postState.deckCount[currentPlayer] == preState.deckCount[currentPlayer] - drawnCards + shuffledCards)
		{
			numSuccess++;
		}
		else
		{
			printf("TEST #%d FAILED: current player incorrect deckcount\n", testCount);
		}

		//test coins
		testCount++;
		if (postState.coins == preState.coins + netCoins)
		{
			numSuccess++;
		}
		else
		{
			printf("TEST #%d FAILED: incorrect coins\n", testCount);
		}

		//test actions
		testCount++;
		if (postState.numActions == preState.numActions + netActions)
		{
			numSuccess++;
		}
		else
		{
			printf("TEST #%d FAILED: incorrect actions\n", testCount);
		}

		//tests for players who are not the current player
		if (choice2 == 1)
		{
			for ( j = 0; j < postState.numPlayers; j++)
			{
				if (j != currentPlayer)
				{
					//if the player had at least 5 cards in hand
					if (preState.handCount[j] >= 5)
					{
						discarded = preState.handCount[j];
						drawnCards = 4;
					}
					else
					{
						discarded = 0;
						drawnCards = 0;
					}

					shuffledCards = 0;

					testCount++;
					if (postState.handCount[j] == preState.handCount[j] - discarded + drawnCards)
					{
						numSuccess++;
					}
					else
					{
						printf("TEST #%d FAILED: noncurrent player %d incorrect hand count\n", testCount, j + 1);
					}

					testCount++;
					if (postState.deckCount[j] == preState.deckCount[j] - drawnCards + shuffledCards)
					{
						numSuccess++;
					}
					else
					{
						printf("TEST #%d FAILED: noncurrent player %d incorrect deck count\n", testCount, j + 1);
					}
				}
			}
		}
		
	}

	printf("\n");

	printf("Tests completed for %s.\n", TESTCARD);
	printf("%d out of %d tests passed.\n\n", numSuccess, testCount);

	return 0;
}
/*
int main() {
	//success and fail counts
	int numSuccess = 0;
	int numFail = 0;

	int i;

	//initialize relevant card variables
	//most variables remain here for reusability, but unused variables are commented out
	//some of these values will change depending on the test and card effect choice
	int drawnCards = 0;
	int discarded = 0;
	int netCoins = 0;
	int shuffledCards = 0;
	int netActions = 1;
	//int netSupply = 0;

	//initialize cardEffect() variables
	int handPos = 0, choice1 = 0, choice2 = 0, choice3 = 0, bonus = 0;
	
	//variables for initializeGame()
	int seed = 999;
	int numPlayers = 3;
	int currentPlayer = 0;

	//two game states used to compare card effects
	struct gameState preState, postState;

	//initialize card array
	int k[10] = {adventurer, minion, ambassador, gardens, mine, remodel, smithy, tribute, baron, great_hall};

	//initialize a game using pregame state
	initializeGame(numPlayers, k, seed, &preState);

	printf("----------------- Testing Card: %s -----------------\n\n", TESTCARD);

	//*************************************************************************************************************
	// TEST 1: choice1 = 1 = +2 gold
	printf("-- TEST 1: choice1 = 1 = +2 coins --\n");

	//copy pregame state over to post game
	memcpy(&postState, &preState, sizeof (struct gameState));

	//change choice variables based on card effects
	choice1 = 1;
	netCoins = 2;

	//call the card
	cardEffect(minion, choice1, choice2, choice3, &postState, handPos, &bonus);

	printf("Test: deck count = %d, expected = %d\tStatus: ", postState.deckCount[currentPlayer], preState.deckCount[currentPlayer] - drawnCards + shuffledCards);
	if (postState.deckCount[currentPlayer] == preState.deckCount[currentPlayer] - drawnCards + shuffledCards)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: coins = %d, expected = %d\t\tStatus: ", postState.coins, preState.coins + netCoins);
	if (postState.coins == preState.coins + netCoins)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: actions = %d, expected = %d\t\tStatus: ", postState.numActions, preState.numActions + netActions);
	if (postState.numActions == preState.numActions + netActions)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	for ( i = 0; i < postState.numPlayers; i++)
	{
		printf("Test: player %d hand count = %d, expected = %d\tStatus: ", i + 1, postState.handCount[i], preState.handCount[i] - discarded + drawnCards);
		if (postState.handCount[i] == preState.handCount[i] - discarded + drawnCards)
		{
			printf("SUCCESS\n");
			numSuccess++;
		}
		else
		{
			printf("FAIL\n");
			numFail++;
		}
	}

	printf("\n");

	//*************************************************************************************************************
	// TEST 2: choice2 = 1, discard hand, redraw 4, no other player has 5 cards
	printf("-- TEST 2: choice2 = 1, discard hand, redraw 4, no other player has 5 cards --\n");

	//copy pregame state over to post game
	memcpy(&postState, &preState, sizeof (struct gameState));

	//change choice variables based on card effects
	choice1 = 0;
	choice2 = 1;
	netCoins = 0;
	discarded = preState.handCount[currentPlayer];
	drawnCards = 4;

	//change other player's hands to a number that is not 5
	for ( i = 0; i < postState.numPlayers; i++)
	{
		if (i != currentPlayer)
		{
			postState.handCount[i] = 4;
		}
	}

	//call the card
	cardEffect(minion, choice1, choice2, choice3, &postState, handPos, &bonus);

	printf("Test: player %d hand count = %d, expected = %d\tStatus: ", currentPlayer + 1, postState.handCount[currentPlayer], preState.handCount[currentPlayer] - discarded + drawnCards);
	if (postState.handCount[currentPlayer] == preState.handCount[currentPlayer] - discarded + drawnCards)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: player %d deck count = %d, expected = %d\tStatus: ", currentPlayer + 1, postState.deckCount[currentPlayer], preState.deckCount[currentPlayer] - drawnCards + shuffledCards);
	if (postState.deckCount[currentPlayer] == preState.deckCount[currentPlayer] - drawnCards + shuffledCards)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: player %d coins = %d, expected = %d\t\tStatus: ", currentPlayer + 1, postState.coins, preState.coins + netCoins);
	if (postState.coins == preState.coins + netCoins)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: player %d actions = %d, expected = %d\t\tStatus: ", currentPlayer + 1, postState.numActions, preState.numActions + netActions);
	if (postState.numActions == preState.numActions + netActions)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	//tests for players who are not the current player
	for ( i = 0; i < postState.numPlayers; i++)
	{
		if (i != currentPlayer)
		{
			discarded = 0;
			drawnCards = 0;
			shuffledCards = 0;

			printf("Test: player %d hand count = %d, expected = %d\tStatus: ", i + 1, postState.handCount[i], preState.handCount[i] - discarded + drawnCards);
			if (postState.handCount[i] == preState.handCount[i] - discarded + drawnCards)
			{
				printf("SUCCESS\n");
				numSuccess++;
			}
			else
			{
				printf("FAIL\n");
				numFail++;
			}

			printf("Test: player %d deck count = %d, expected = %d\tStatus: ", i + 1, postState.deckCount[i], preState.deckCount[i] - drawnCards + shuffledCards);
			if (postState.deckCount[i] == preState.deckCount[i] - drawnCards + shuffledCards)
			{
				printf("SUCCESS\n");
				numSuccess++;
			}
			else
			{
				printf("FAIL\n");
				numFail++;
			}
		}
	}

	printf("\n");

	//*************************************************************************************************************
	// TEST 3: choice2 = 1, discard hand, redraw 4, all other player has 5 cards
	printf("-- TEST 3: choice2 = 1, discard hand, redraw 4, all other player has 5 cards --\n");

	//copy pregame state over to post game
	memcpy(&postState, &preState, sizeof (struct gameState));

	//change choice variables based on card effects
	choice1 = 0;
	choice2 = 1;
	netCoins = 0;

	//change other player's hand counts to 5
	for ( i = 0; i < postState.numPlayers; i++)
	{
		if (i != currentPlayer)
		{
			postState.handCount[i] = 5;
		}
	}

	//call the card
	cardEffect(minion, choice1, choice2, choice3, &postState, handPos, &bonus);

	printf("Test: player %d hand count = %d, expected = %d\tStatus: ", currentPlayer + 1, postState.handCount[currentPlayer], preState.handCount[currentPlayer] - discarded + drawnCards);
	if (postState.handCount[currentPlayer] == preState.handCount[currentPlayer] - discarded + drawnCards)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: player %d deck count = %d, expected = %d\tStatus: ", currentPlayer + 1, postState.deckCount[currentPlayer], preState.deckCount[currentPlayer] - drawnCards + shuffledCards);
	if (postState.deckCount[currentPlayer] == preState.deckCount[currentPlayer] - drawnCards + shuffledCards)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: player %d coins = %d, expected = %d\t\tStatus: ", currentPlayer + 1, postState.coins, preState.coins + netCoins);
	if (postState.coins == preState.coins + netCoins)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	printf("Test: player %d actions = %d, expected = %d\t\tStatus: ", currentPlayer + 1, postState.numActions, preState.numActions + netActions);
	if (postState.numActions == preState.numActions + netActions)
	{
		printf("SUCCESS\n");
		numSuccess++;
	}
	else
	{
		printf("FAIL\n");
		numFail++;
	}

	

	printf("\n");

	printf("Tests completed for %s.\n", TESTCARD);
	printf("%d out of %d tests passed.\n\n", numSuccess, numSuccess + numFail);

	return 0;
}
*/