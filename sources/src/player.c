/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <player.h>
#include <sprite.h>
#include <map.h>
#include <window.h>
#include <misc.h>
#include <constant.h>

struct player {
	int x, y;
	enum direction direction;
	int bombs;
	int lives;
	int range;
	int keys;
	int timer;
	int vulnerable;//vulnerability setting

};

struct player* player_init(int bombs) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->direction = NORTH;
	player->bombs = bombs;
	player->lives=7;
	player->range=1;
	player->keys=3;
	player->vulnerable=1;

	return player;
}

int player_dec_lives(struct player* player) {
	assert(player);//decreases nb of lives
	player->lives--;
}

int player_inc_lives(struct player* player) {
	assert(player);//increase nb of lives
	player->lives++;
}


int player_dec_range(struct player* player) {
	assert(player);//decreases player range
		player->range--;
}


int player_inc_range(struct player* player) {
	assert(player);//increases player range
	player->range++;
}

int player_dec_keys(struct player* player) {
	assert(player);//decreases player keys
	player->keys--;
}

int player_inc_keys(struct player* player) {
	assert(player);//increases player keys
  player->keys++;
}

int player_get_lives(struct player* player) {
	assert(player);//returns nb of lives
	return player->lives;
}

int player_get_range(struct player* player) {
	assert(player);//returns player range
	return player->range;
}
int player_get_keys(struct player* player) {
	assert(player);//returns player keys
	return player->keys;
}
int player_get_timer(struct player* player) {
	assert(player);//returns player timer for vulnerability test
	return player->timer;
}
void player_set_timer(struct player* player,int timer){
	assert(player);
	player->timer=timer;
}
int player_is_vulnerable(struct player* player){
	assert(player);
	return player->vulnerable;
}
void player_set_vulnerability(struct player* player,int v){
	assert(player);
	player->vulnerable=v;
}
void player_set_position(struct player *player, int x, int y) {
	assert(player);
	player->x = x;
	player->y = y;
}


void player_free(struct player* player) {
	assert(player);
	free(player);
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->direction = way;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->bombs;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	player->bombs += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	player->bombs -= 1;
}

static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (map_is_inside(map, x, y)==0)
		return 0;
	if(move_is_blocked(map,x,y)==1) //if the cell is not accessible
		return 0;
	switch (map_get_cell_type(map, x, y)){
	case CELL_SCENERY:
		return 1;
		break;

	case CELL_BOX:
		return 1;
		break;

	case CELL_BONUS:
		break;

	case CELL_MONSTER:
		break;

	case CELL_DOOR:
		return 1;
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->direction) {

	case NORTH:

		if (player_move_aux(player, map, x, y - 1)) {
			if (map_get_cell_type(map,x,y-1)==CELL_BOX){
				if (map_is_inside(map, x, y-2)==0)
					break;
				if(map_get_cell_type(map,x,y-2)==CELL_EMPTY){
					int compose_type=map_get_compose_cell_type(map,x,y-1);
					map_set_cell_type(map, x, y-1, CELL_EMPTY);
					map_set_cell_type(map, x, y-2, CELL_BOX+compose_type);
				}else break;
			}//allows player to move one box
			player->y--;
			move = 1;
		}
		break;

	case SOUTH:

		if (player_move_aux(player, map, x, y + 1)) {
			if (map_get_cell_type(map,x,y+1)==CELL_BOX){
				if (map_is_inside(map, x, y+2)==0)
					break;
				if(map_get_cell_type(map,x,y+2)==CELL_EMPTY){
					int compose_type=map_get_compose_cell_type(map,x,y+1);
					map_set_cell_type(map, x, y+1, CELL_EMPTY);
					map_set_cell_type(map, x, y+2, CELL_BOX+compose_type);
				}else break;
			}//allows player to move one box
			player->y++;
			move = 1;
		}
		break;

	case WEST:

		if (player_move_aux(player, map, x - 1, y)) {
			if (map_get_cell_type(map,x-1,y)==CELL_BOX){
				if (map_is_inside(map, x-2, y)==0)
					break;
				if (map_get_cell_type(map,x-2,y)==CELL_EMPTY){
					int compose_type=map_get_compose_cell_type(map,x-1,y);
					map_set_cell_type(map, x-1, y, CELL_EMPTY);
					map_set_cell_type(map, x-2, y, CELL_BOX+compose_type);
				}else break;
			}//allows player to move one box
			player->x--;
			move = 1;
		}
		break;

	case EAST:

		if (player_move_aux(player, map, x + 1, y)) {
			if (map_get_cell_type(map,x+1,y)==CELL_BOX){
				if (map_is_inside(map, x+2, y)==0)
					break;
				if(map_get_cell_type(map,x+2,y)==CELL_EMPTY){
					int compose_type=map_get_compose_cell_type(map,x+1,y);
					map_set_cell_type(map, x+1, y, CELL_EMPTY);
					map_set_cell_type(map, x+2, y, CELL_BOX+compose_type);
				}else break; //allows player to move one box
			}
			player->x++;
			move = 1;
		}
		break;
	}

	if (move) {
		if (map_get_cell_type(map,x,y)==CELL_BOMB_TTL4){
			return move;
		}
		if (map_get_cell_type(map,x,y)==CELL_PRINCESS){
			return move;
		  }

		if (map_get_cell_type(map,x,y)==CELL_BOMB_TTL3){
			return move;
		}
		if (map_get_cell_type(map,x,y)==CELL_BOMB_TTL2){
			return move;
		}
		if (map_get_cell_type(map,x,y)==CELL_BOMB_TTL1){
			return move;
		}
		if (map_get_cell_type(map,x,y)==CELL_BOMB_TTL0){

			return move;
		}
   if(map_get_cell_type(map,x,y)==CELL_MONSTER){
		 return move;
	 }
		if (map_get_cell_type(map,x,y)==CELL_DOOR){
			return move;
		}
		map_set_cell_type(map, x, y, CELL_EMPTY);
	}
	return move;
}//allows player to move on said cell

void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);//displays player on current map
}
