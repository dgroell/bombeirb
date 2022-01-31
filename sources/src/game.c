/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <bomb.h>
#include <monster.h>
#include <map.h>


struct game {
	struct map** maps;       // the game's map
	short levels;        // nb maps of the game
	short level;
	struct player* player;
  struct Bombs* first_bomb;
	int active_bombs;	//number of active bombs
	struct monster* monsters[21];
	int paused;
};


//return the map of the level where the player is now
struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->level];
}

//initialize the monsters
void monster_init_map(struct monster* monster, struct map* map){
	int height = map_get_height(map);
	int width = map_get_width(map);
	for (int i = 0; i < width * height; i++){
			if (map_get_cell_type(map,(i-(i%width))/height,i%width) == CELL_MONSTER){
		monster_set_position(monster,(i-(i%width))/height,i%width);
		map_set_cell_type(map,(i-(i%width))/height,i%width,CELL_EMPTY);
		break;
  	}
	}
}

struct game* game_new(void) {
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));
	game->levels=7;
	game->maps = malloc(sizeof(struct game));

	for(int j=0;j<MAX_MONSTERS;j++) { //initialize all monsters even uactive ones to test on nullity in game_update and monster_display
		game->monsters[j]=NULL;
	}

	for(int i=0;i<game->levels;i++){

		game->maps[i]=map_get_static(i);
		map_set_bonus_monster(game->maps[i],i+14);
    game->monsters[2*i]=monster_init(i,game->maps[i]);
		monster_init_map(game->monsters[2*i],game->maps[i]);
		game->monsters[(2*i)+1]=monster_init(i,game->maps[i]);
		monster_init_map(game->monsters[(2*i)+1],game->maps[i]);
	}
	game->paused=0;
	game->level = 0;
	game->first_bomb=NULL;
  game->active_bombs=0;
	game->player = player_init(3);
	// Set default location of the player
	player_set_position(game->player, 1, 0);
	return game;
}

//return the structure with the monsters
struct monster** game_get_monsters(struct game * game){
	assert(game);
	return game->monsters;
}

void game_set_monster(struct game* game,struct monster* monster,int index) {
	assert(game);
	assert(monster);
	game->monsters[index]=monster;
}

int game_get_level(struct game * game){
	assert(game);
	return game->level;
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	for (int i = 0; i < game->levels; i++)
		map_free(game->maps[i]);
}

struct Bombs* game_get_first_bomb(struct game* game){
	assert(game);
	return game->first_bomb;
}

void game_dec_activebombs(struct game* game){
	assert(game);
	game->active_bombs--;
}

void game_set_first_bomb(struct game* game, struct Bombs* bomb){
	assert(game);
  game->first_bomb=bomb;
	bomb_set_next(game->first_bomb,NULL);
}

void game_insert_bomb(struct game* game, struct Bombs* bomb) {
	assert(game);
	bomb_set_next(bomb,game->first_bomb);
	game->first_bomb=bomb;
}

struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}



void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = white_bloc/2;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);
	x = white_bloc/2 + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_lives(game_get_player(game))), x, y);

	x = 2 * (white_bloc/2) + 2 * SIZE_BLOC - white_bloc/8;
	window_display_image(sprite_get_banner_bomb(), x, y);
	x = 3 * (white_bloc/2) + 2 * SIZE_BLOC - white_bloc/8;
	window_display_image(sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * (white_bloc/2) + 3* SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);
	x = 4 * (white_bloc/2) + 3 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_range(game_get_player(game))), x, y);

  x = 4* (white_bloc/2) + 4 * SIZE_BLOC;
  window_display_image(sprite_get_banner_key(), x, y);
  x=  5* (white_bloc/2) + 4 * SIZE_BLOC + white_bloc/8;
  window_display_image(sprite_get_number(player_get_keys(game_get_player(game))), x, y);

	x = 4* (white_bloc/2) + 6 * SIZE_BLOC;
	window_display_image(sprite_get_banner_flag(), x, y);
	x=  5* (white_bloc/2) + 6 * SIZE_BLOC;
	window_display_image(sprite_get_number(game->level), x, y);
}

void game_display(struct game* game) {
	assert(game);

	window_clear();
	game_banner_display(game);
	map_display(game_get_current_map(game));
	player_display(game_get_player(game));
	monster_display(game);
	window_refresh();
}


static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);
	int x = player_get_x(player);
	int y = player_get_y(player);

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_p:
 			 if(game->paused==0){//if game paused no action possible
 				 game->paused=1;
 			 }
 			 else {
 				 game->paused=0;
 			 }
			 break;
			case SDLK_UP:
			if (game->paused){}
			else{
				player_set_current_way(player, NORTH);
				if(y>0){
					if(map_get_cell_type(map,x,y-1)==CELL_PRINCESS){
    				printf("YOU HAVE SAVED THE PRINCESS\n");
						exit(EXIT_SUCCESS);
					}
					if (map_get_cell_type(map,x,y-1)==CELL_DOOR) {
					if ((map_get_compose_cell_type(map, x, y-1)& 1) ==1) {

							int level = (map_get_compose_cell_type(map,x,y-1) & 14)/2;
							game->level = level;
						}
						else if(player_get_keys(player)!=0){

						 map_set_cell_type(map,x,y-1,CELL_DOOR+map_get_compose_cell_type(map, x,y-1)+1);
						 player_dec_keys(player);
						 int level = (map_get_compose_cell_type(map,x,y-1) & 14)/2;
						 game->level = level;
					 }
					}
					//Bonus implementation;
					if (map_get_cell_type(map,x,y-1)==CELL_BONUS) {
						if(map_get_compose_cell_type(map,x,y-1)==BONUS_BOMB_RANGE_DEC){
						  if(player_get_range(player)!=0)	{
								player_dec_range(player);
							  map_set_cell_type(map,x,y-1,CELL_EMPTY);
					   	}
            }
						else if(map_get_compose_cell_type(map,x,y-1)==BONUS_BOMB_RANGE_INC) {
							player_inc_range(player);
							map_set_cell_type(map,x,y-1,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x,y-1)==BONUS_BOMB_NB_DEC){
							player_dec_nb_bomb(player);
							map_set_cell_type(map,x,y-1,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x,y-1)==BONUS_BOMB_NB_INC){
							player_inc_nb_bomb(player);
							map_set_cell_type(map,x,y-1,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x,y-1)==BONUS_LIFE){
							player_inc_lives(player);
							map_set_cell_type(map,x,y-1,CELL_EMPTY);
						}
				  }

				}
				player_move(player, map);
				break;
			}

			case SDLK_DOWN:
			if (game->paused){}//if game paused no action possible
			else {
				player_set_current_way(player, SOUTH);
				if(y<map_get_height(map)-1){
					if(map_get_cell_type(map,x,y+1)==CELL_PRINCESS){//win the game if you reach the princess
						printf("YOU HAVE SAVED THE PRINCESS\n");
						exit(EXIT_SUCCESS);
				 }
					if (map_get_cell_type(map,x,y+1)==CELL_DOOR) {//condition for the level changement
					if ((map_get_compose_cell_type(map, x, y+1)& 1) ==1) {

							int level = (map_get_compose_cell_type(map,x,y+1) & 14)/2;
							game->level = level;
						}
						else if(player_get_keys(player)!=0){
						 map_set_cell_type(map,x,y+1,CELL_DOOR+map_get_compose_cell_type(map, x,y+1)+1);
						 player_dec_keys(player);
						 int level = (map_get_compose_cell_type(map,x,y+1) & 14)/2;
						 game->level = level;
					 }
					}
					//Bonus implementation;
					if (map_get_cell_type(map,x,y+1)==CELL_BONUS) {
						if(map_get_compose_cell_type(map,x,y+1)==BONUS_BOMB_RANGE_DEC){
							if(player_get_range(player)!=0)	{
								player_dec_range(player);
								map_set_cell_type(map,x,y+1,CELL_EMPTY);
							}
						}
						else if(map_get_compose_cell_type(map,x,y+1)==BONUS_BOMB_RANGE_INC) {
							player_inc_range(player);
							map_set_cell_type(map,x,y+1,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x,y+1)==BONUS_BOMB_NB_DEC){
							player_dec_nb_bomb(player);
							map_set_cell_type(map,x,y+1,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x,y+1)==BONUS_BOMB_NB_INC){
							player_inc_nb_bomb(player);
							map_set_cell_type(map,x,y+1,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x,y+1)==BONUS_LIFE){
							player_inc_lives(player);
							map_set_cell_type(map,x,y+1,CELL_EMPTY);
						}
					}
				}
				player_move(player, map);
				break;
			}
			case SDLK_RIGHT:
			if (game->paused){ }//if game paused no action possible
			else{
				player_set_current_way(player, EAST);
				if(x<map_get_width(map)-1){
					if(map_get_cell_type(map,x+1,y)==CELL_PRINCESS){//win the game if you reach the princess
						printf("YOU HAVE SAVED THE PRINCESS\n");
						exit(EXIT_SUCCESS);
				 }
					if (map_get_cell_type(map,x+1,y) == CELL_DOOR) {//condition for the level changement
					if ((map_get_compose_cell_type(map, x+1, y)& 1) ==1) {

							int level = (map_get_compose_cell_type(map,x+1,y) & 14)/2;
							game->level = level;
						}
						else if(player_get_keys(player)!=0){
						 map_set_cell_type(map,x+1,y,CELL_DOOR+map_get_compose_cell_type(map, x+1,y)+1);
						 player_dec_keys(player);
						 int level = (map_get_compose_cell_type(map,x+1,y) & 14)/2;
						 game->level = level;
					 }
					}
					//Bonus implementation;
					if (map_get_cell_type(map,x+1,y)==CELL_BONUS) {
						if(map_get_compose_cell_type(map,x+1,y)==BONUS_BOMB_RANGE_DEC){
							if(player_get_range(player)!=0)	{

								player_dec_range(player);
								map_set_cell_type(map,x+1,y,CELL_EMPTY);
							}
						}
						else if(map_get_compose_cell_type(map,x+1,y)==BONUS_BOMB_RANGE_INC) {
							player_inc_range(player);
							map_set_cell_type(map,x+1,y,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x+1,y)==BONUS_BOMB_NB_DEC){
							if(player_get_nb_bomb(player)!=0)	{
							player_dec_nb_bomb(player);
							map_set_cell_type(map,x+1,y,CELL_EMPTY);
						}
					}
						else if(map_get_compose_cell_type(map,x+1,y)==BONUS_BOMB_NB_INC){
							player_inc_nb_bomb(player);
							map_set_cell_type(map,x+1,y,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x+1,y)==BONUS_LIFE){

							player_inc_lives(player);
							map_set_cell_type(map,x+1,y,CELL_EMPTY);
						}
					}
				}
				player_move(player, map);
				break;
			}
			case SDLK_LEFT:
			if (game->paused){ }//if game paused no action possible
			else{
				player_set_current_way(player, WEST);
				if(x>0){
					if(map_get_cell_type(map,x-1,y)==CELL_PRINCESS){//win the game if you reach the princess
						printf("YOU HAVE SAVED THE PRINCESS\n");
						exit(EXIT_SUCCESS);
				 }
					if (map_get_cell_type(map,x-1,y) == CELL_DOOR) {//condition for the level changement
					 if ((map_get_compose_cell_type(map, x-1, y)& 1) ==1) {
							int level = (map_get_compose_cell_type(map,x-1,y) & 14)/2;
							game->level = level;
						 }
					 else if(player_get_keys(player)!=0){
						map_set_cell_type(map,x-1,y,CELL_DOOR+map_get_compose_cell_type(map, x-1,y)+1);
						player_dec_keys(player);
						int level = (map_get_compose_cell_type(map,x-1,y) & 14)/2;

						game->level = level;
					 }
					}
					//Bonus implementation;
					if (map_get_cell_type(map,x-1,y)==CELL_BONUS) {
						if(map_get_compose_cell_type(map,x-1,y)==BONUS_BOMB_RANGE_DEC){
							if(player_get_range(player)!=0)	{
								player_dec_range(player);
								map_set_cell_type(map,x-1,y,CELL_EMPTY);
							}
						}
						else if(map_get_compose_cell_type(map,x-1,y)==BONUS_BOMB_RANGE_INC) {
							player_inc_range(player);
						  map_set_cell_type(map,x-1,y,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x-1,y)==BONUS_BOMB_NB_DEC){
							player_dec_nb_bomb(player);
							map_set_cell_type(map,x-1,y,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x-1,y)==BONUS_BOMB_NB_INC){
							player_inc_nb_bomb(player);
							map_set_cell_type(map,x-1,y,CELL_EMPTY);
						}
						else if(map_get_compose_cell_type(map,x-1,y)==BONUS_LIFE){
							player_inc_lives(player);
							map_set_cell_type(map,x-1,y,CELL_EMPTY);
						}
					}
				}
				player_move(player, map);
				break;
      }
			case SDLK_SPACE:
			if (game->paused){ }//if game paused no action possible
			else {
			  if(player_get_nb_bomb(player)>0){
			        struct Bombs* bomb=bomb_init(game);
			        bomb_launch(bomb,player,game);
							game->active_bombs++;
						 	player_dec_nb_bomb(player);
						}
					}
				break;

			default:
				break;
			}

			break;
		}
	}
	return 0;
}


int game_update(struct game* game) {

	assert(game);
	short input = input_keyboard(game);
	if (input==1)
		return 1; // exit game
if(game->paused) {}//if game paused no action possible
else {
   int level=game->level;
   struct Bombs* bomb=game->first_bomb;
   int timer=SDL_GetTicks();
   int i=game->active_bombs;
	 while(i!=0) {
		 bomb_chain_update(game,bomb,timer);
		 bomb=bomb_get_next(bomb);
		 i--;
	 } //Bomb chain update

   struct player* player=game_get_player(game);
	 if(!player_is_vulnerable(player)){
		 int player_timer=player_get_timer(player);
		 if(timer-player_timer>=1000){
			 player_set_vulnerability(player,1);
		 }
	 }//player invulnerability update after damage
	 if(player_get_lives(player)==0){
		 printf("%s\n","YOU HAVE FAILED YOUR PRINCESS");
		 exit(EXIT_FAILURE);
	 }
	 if(map_get_cell_type(game_get_current_map(game),player_get_x(player),player_get_y(player))==CELL_BOMB_TTL0 && player_is_vulnerable(player)){
		 player_dec_lives(player);
		 player_set_vulnerability(player,0);
		 player_set_timer(player,timer);
	 }

    struct map* map=game_get_current_map(game);
    for (int j=0;j<MAX_MONSTERS;j++){
     	struct monster* monster=game->monsters[j];
      if(monster!=NULL) {
	      if(monster_get_level(monster)==level && !monster_is_dead(monster)){
		       if(monster_get_x(monster)==player_get_x(player) && monster_get_y(monster)==player_get_y(player) && player_is_vulnerable(player) ) {
		          	player_dec_lives(player);
		           	player_set_vulnerability(player,0);
		           	player_set_timer(player,timer);
		       }
		       if(map_get_cell_type(map,monster_get_x(monster),monster_get_y(monster))==CELL_BOMB_TTL0){
	              kill_monster(monster);
	         }
	        int prevtime=monster_get_timer(monster);
	        if(timer-prevtime>=1000-(level*200/3)){
                monster_update(monster,game);
	              monster_set_timer(monster,timer);
          }
       }
     }
   } //Update of monsters' Movements and setting player damage from monsters
}
	return 0;
}
