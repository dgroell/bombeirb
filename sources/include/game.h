/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef GAME_H_
#define GAME_H_

#include <player.h>
#include <map.h>



// Abstract data type
struct game;

// Create a new game
struct game* game_new();

// Free a game
void game_free(struct game* game);

// Return the player of the current game
struct player* game_get_player(struct game* game);

// Return the current map
struct map* game_get_current_map(struct game* game);

struct monster** game_get_monsters(struct game * game);
int game_get_level(struct game * game);

void game_display(struct game* game);

// update
int game_update(struct game* game);
void game_set_monster(struct game* game,struct monster* monster,int index);
//void game_freebomb(struct game* game);
struct Bombs* game_get_first_bomb(struct game* game);
void game_set_first_bomb(struct game* game, struct Bombs* bomb);
void monster_init_map(struct monster* monster, struct map* map);

void game_insert_bomb(struct game* game, struct Bombs* bomb);
void game_dec_activebombs(struct game* game);

// Display the game on the screen

#endif /* GAME_H_ */
