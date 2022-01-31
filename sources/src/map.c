/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bomb.h>
#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>
#include <string.h>

struct map {
	int width;
	int height;
	unsigned char* grid;
	int bonus_monster;
};

#define CELL(i,j) ( (i) + (j) * map->width)

struct map* map_new(int width, int height)
{
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof *map);
	if (map == NULL )
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;

	map->grid = malloc(height * width);
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
	  for (j = 0; j < height; j++)
	    map->grid[CELL(i,j)] = CELL_EMPTY;

	return map;
}
//tests validity of cell
int map_is_inside(struct map* map, int x, int y)
{
	assert(map);
	if (x<0){
		return 0;
	}
	if (y<0){
		return 0;
	}
	if (map->width <=x){
		return 0;
	}
	if (map->height <=y){
		return 0;
	}
	return 1;
}
//frees map
void map_free(struct map *map)
{
	if (map == NULL )
		return;
	free(map->grid);
	free(map);
}
//returns map width
int map_get_width(struct map* map)
{
	assert(map != NULL);
	return map->width;
}
//returns mapheight
int map_get_height(struct map* map)
{
	assert(map);
	return map->height;
}
//sets monster malus' index within game->monsters
void map_set_bonus_monster(struct map* map, int i){
	assert(map);
	map->bonus_monster=i;
}
//sets monster malus' index within game->monsters
int map_get_bonus_monster(struct map* map){
	assert(map);
	return map->bonus_monster;
}
//returns cell type
enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));

	return map->grid[CELL(x,y)] & 0xf0;
}
//returns compose cell type
enum compose_type map_get_compose_cell_type(struct map* map, int x,int y){
	assert(map && map_is_inside(map,x,y));
	return map->grid[CELL(x,y)] & 15;
}
//returns 1 if cell is accessible, 0 otherwise
int  move_is_blocked(struct map* map, int x,int y){
	if (map_get_cell_type(map,x,y)==CELL_SCENERY){
		return 1;
	}
	return 0;
}
//sets map cell type
void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
}
//displays bonuses within the map
void display_bonus(struct map* map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type & 0x0f) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;
	case BONUS_LIFE:
	 window_display_image(sprite_get_bonus(BONUS_LIFE),x,y);
	}
}
// displays scenery type cells in map
void display_scenery(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;

	}
}
//displays all map cells in the map
void map_display(struct map* map){
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);

	int x, y;
	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {
	    x = i * SIZE_BLOC;
	    y = j * SIZE_BLOC;

	    unsigned char type = map->grid[CELL(i,j)];

	    switch (type & 0xf0) {
			case CELL_PRINCESS:
			  window_display_image(sprite_get_princess(),x,y);
				break;
			case CELL_SCENERY:
		  	display_scenery(map, x, y, type);
		  	break;
	    case CELL_BOX:
	      window_display_image(sprite_get_box(), x, y);
	      break;
	    case CELL_BONUS:
	      display_bonus(map, x, y, type);
	      break;
	    case CELL_KEY:
	      window_display_image(sprite_get_key(), x, y);
	      break;
      case CELL_BOMB_TTL0:
        window_display_image(sprite_get_bomb(0),x,y);
	      break;
			case CELL_BOMB_TTL1:
	      window_display_image(sprite_get_bomb(1),x,y);
		    break;
			case CELL_BOMB_TTL2:
		    window_display_image(sprite_get_bomb(2),x,y);
			  break;
			case CELL_BOMB_TTL3:
			   window_display_image(sprite_get_bomb(3),x,y);
				break;
			case CELL_BOMB_TTL4:
				 window_display_image(sprite_get_bomb(4),x,y);
			  break;
	    case CELL_DOOR:
				if ( type%2 == 0 ){
					window_display_image(sprite_get_door_closed(), x, y);
				}
	      else window_display_image(sprite_get_door_opened(), x, y);
	      break;
		case CELL_MONSTER:
			window_display_image(sprite_get_monster(1),x,y);
		break;
	    }
	  }
	}
}
// reads map text files and returns map type struct to be loaded in the game
struct map* map_get_static(int level){
	char fichier_map[20];
	strcpy(fichier_map, "src/map/map_");
	char* number_map = malloc(10);
	sprintf(number_map,"%d",level);
	strcat(fichier_map,number_map);
	strcat(fichier_map,".txt");
  FILE* fmap=fopen(fichier_map,"r");
	int map_width;
	int map_height;
	fscanf(fmap,"%i ",&map_width);
	fgetc(fmap);
	fscanf(fmap,"%i ",&map_height);
	int cells;
	struct map* map = map_new(map_width, map_height);
  if (fmap!=NULL){
		for (int i = 0; i < map_width * map_height; i++){
			fscanf(fmap,"%i ",&cells);
			map->grid[i] = cells;
	  }
			fclose(fmap);
	}

	return map;
}
