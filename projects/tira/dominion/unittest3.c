#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"
#include <stdlib.h>

#define TESTCARD "ambassador"

//I followed the cardtest4.c example given to us, but many of my methods, formatting/comments, and variable name choices are my own.
int main() {
	//success and fail counts
	int numSuccess = 0;
	int numFail = 0;

	int i, j, m;
	int testCount = 0;

	//initialize relevant card variables
	//most variables remain here for reusability, but unused variables are commented out
	//some of these values will change depending on the test and card effect choice
	int drawnCards = 0;
	int discarded = 0;
	//int netCoins = 0;
	//int shuffledCards = 0;
	//int netActions = 1;
	int netSupply = 0;
	int gainedCards = 0;

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

	//set handCount of all other players equal to 5, since they have not taken their turn yet, they have 0 cards in hand.
	for ( i = 0; i < preState.numPlayers; i++)
	{
		if (i != currentPlayer)
		{
			preState.handCount[i] = 5;
		}
	}


	printf("----------------- Testing Card: %s -----------------\n\n", TESTCARD);

	//*************************************************************************************************************
	// TEST #: choice1 = 0 to preState.handCount[currentPlayer], choice2 = 0 to 3
	for (i = 0; i < preState.handCount[currentPlayer]; i++)
	{
		for (j = 0; j < 4; j++)
		{
			//copy pregame state over to post game
			memcpy(&postState, &preState, sizeof (struct gameState));

			printf("-- TEST %d: choice1 = %d, choice2 = %d --\n", testCount + 1, i, j);

			//change choice variables based on card effects
			choice1 = i;
			choice2 = j;
			netSupply = choice2;
			discarded = choice2;

			//call the card
			cardEffect(ambassador, choice1, choice2, choice3, &postState, handPos, &bonus);

			drawnCards = 0;
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

			//reset discarded to 0 for non-currentPlayer players
			discarded = 0;
			for ( m = 1; i < postState.numPlayers; m++)
			{
				printf("Test: player %d hand count = %d, expected = %d\tStatus: ", i + 1, postState.handCount[m], preState.handCount[m] - discarded + drawnCards);
				if (postState.handCount[m] == preState.handCount[m] - discarded + drawnCards)
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

			//check discard piles  of non-currentPlayer players
			gainedCards = 1;
			for ( m = 1; i < postState.numPlayers; m++)
			{
				printf("Test: player %d discard count = %d, expected = %d\tStatus: ", i + 1, postState.discardCount[m], preState.discardCount[m] + discarded + gainedCards);
				if (postState.discardCount[m] == preState.discardCount[m] + discarded + gainedCards)
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

			printf("Test: supply count = %d, expected = %d\tStatus: ", postState.supplyCount[preState.hand[currentPlayer][choice1]], preState.supplyCount[preState.hand[currentPlayer][choice1]] + netSupply);
			if (postState.supplyCount[preState.hand[currentPlayer][choice1]] == preState.supplyCount[preState.hand[currentPlayer][choice1]] + netSupply)
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

			testCount++;
		}
	}

	printf("Tests completed for %s.\n", TESTCARD);
	printf("%d out of %d tests passed.\n\n", numSuccess, numSuccess + numFail);

	return 0;
}