/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef MAP_H_
#define MAP_H_

enum cell_type {
	CELL_EMPTY=0x00,   		//  0000 0000
	CELL_SCENERY=0x10, 		//  0001 0000
	CELL_BOX=0x20,   		//  0010 0000
	CELL_DOOR=0x30,      	//  0011 0000
	CELL_KEY=0x40,       	//  0100 0000
	CELL_BONUS=0x50, 		// 	0101 0000
	CELL_MONSTER=0x60, 		// 	0110 0000
	CELL_BOMB_TTL0=0x70,
	CELL_BOMB_TTL1=0x80,
	CELL_BOMB_TTL2=0x90,
	CELL_BOMB_TTL3=0xa0,
	CELL_BOMB_TTL4=0xb0,
	CELL_PRINCESS=0xc0 //princess coded as cell type for simplicity

};

enum bonus_type {
	NO_BONUS=0,
	BONUS_BOMB_RANGE_DEC=1,
	BONUS_BOMB_RANGE_INC=2,
	BONUS_BOMB_NB_DEC=3,
	BONUS_BOMB_NB_INC=4,
	BONUS_MONSTER=5,
	BONUS_LIFE=6,

};

enum scenery_type {
	SCENERY_STONE = 1,    // 0001
	SCENERY_TREE  = 2,    // 0010

};

enum compose_type {
	CELL_MONSTER_NORTH = CELL_MONSTER | 1,
	CELL_MONSTER_WEST = CELL_MONSTER | 2,
	CELL_MONSTER_SOUTH = CELL_MONSTER | 3,
	CELL_MONSTER_EAST = CELL_MONSTER | 4,
	CELL_TREE     = CELL_SCENERY | SCENERY_TREE,
	CELL_STONE    = CELL_SCENERY | SCENERY_STONE,
  CELL_EXPLOSION_BOMB_RANGE_INC= CELL_BOMB_TTL0 | BONUS_BOMB_RANGE_DEC,
	CELL_EXPLOSION_BOMB_RANGE_DEC=CELL_BOMB_TTL0 | BONUS_BOMB_RANGE_INC,
	CELL_EXPLOSION_BOMB_NB_DEC=CELL_BOMB_TTL0 | BONUS_BOMB_NB_DEC,
	CELL_EXPLOSION_BOMB_NB_INC=CELL_BOMB_TTL0 | BONUS_BOMB_NB_INC,
	CELL_EXPLOSION_MONSTER=CELL_BOMB_TTL0 | BONUS_MONSTER,
	CELL_EXPLOSION_LIFE=CELL_BOMB_TTL0 | BONUS_LIFE,


    CELL_BOX_RANGEINC = CELL_BOX | BONUS_BOMB_RANGE_INC,
    CELL_BOX_RANGEDEC = CELL_BOX | BONUS_BOMB_RANGE_DEC,
	CELL_BOX_BOMBINC  = CELL_BOX | BONUS_BOMB_NB_INC,
    CELL_BOX_BOMBDEC  = CELL_BOX | BONUS_BOMB_NB_DEC,
    CELL_BOX_LIFE     = CELL_BOX | BONUS_LIFE,
    CELL_BOX_MONSTER  = CELL_BOX | BONUS_MONSTER,

		CELL_BONUS_BOMB_RANGE_DEC=CELL_BONUS | BONUS_BOMB_RANGE_DEC,
	  CELL_BONUS_BOMB_RANGE_INC=CELL_BONUS | BONUS_BOMB_RANGE_INC,
		CELL_BONUS_BOMB_NB_DEC=CELL_BONUS | BONUS_BOMB_NB_DEC,
		CELL_BONUS_BOMB_NB_INC=CELL_BONUS | BONUS_BOMB_NB_INC,
		CELL_BONUS_MONSTER=CELL_BONUS | BONUS_MONSTER,
		CELL_BONUS_LIFE=CELL_BONUS | BONUS_LIFE,
 };

struct map;

// Create a new empty map
struct map* map_new(int width, int height);
void map_free(struct map* map);
//Sets the bonus monster iindex within the game monsters
void map_set_bonus_monster(struct map* map, int i);
//returns the index in wich the bonus monster should be set
int map_get_bonus_monster(struct map* map);
// Return the height and width of a map
int map_get_width(struct map* map);
int map_get_height(struct map* map);

// Return the type of a cell
enum cell_type map_get_cell_type(struct map* map, int x, int y);

//Return the type of a compose cell
enum compose_type map_get_compose_cell_type(struct map* map, int x,int y);

// Set the type of a cell
void  map_set_cell_type(struct map* map, int x, int y, enum cell_type type);

// Test if (x,y) is within the map
int map_is_inside(struct map* map, int x, int y);
int move_is_blocked(struct map* map, int x, int y);

// Return a default static map
struct map* map_get_static();

// Display the map on the screen
void map_display(struct map* map);

#endif /* MAP_H_ */
