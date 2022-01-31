#include <SDL/SDL_image.h>
#include <assert.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <constant.h>
#include <game.h>


struct monster{
  int x,y;
  int monster_direction;
	int monster_timer;
  struct map* map;
  int level;
  int heur[4];//heuristic function that calculates the best move for the monster
  int dead;
  int blocked;
};

struct monster* monster_init(int level,struct map* map){
  struct monster* monster = malloc(sizeof(*monster));
  if (!monster)
    error("Memory error");
  monster->x=NULL;
  monster->y=NULL;
  monster->monster_direction = 2;
	monster->monster_timer=0;
  monster->level=level;
  monster->map=map;
  monster->dead=0;
  monster->blocked=0; //if monster is blocked
	return monster;
}



int monster_get_dir(struct monster* monster){
  assert(monster);
  return monster->monster_direction;
}

void monster_set_dir(struct monster* monster,int dir){
  monster->monster_direction = dir;
}

void monster_set_position(struct monster* monster, int x, int y){
  assert(monster);
  monster->x = x;
  monster->y = y;
}


int monster_get_x(struct monster* monster){
  assert(monster);
  return monster->x;
}

int monster_get_y(struct monster* monster){
  assert(monster);
  return monster->y;
}

int monster_blocked(struct monster *monster,int x,int y) {//monster is not blocked by bonuses and explosions
  struct map* map=monster->map;
  int x_m = monster_get_x(monster);
  int y_m = monster_get_y(monster);
  if (map_is_inside(map,2*x-x_m,2*y-y_m) && map_get_cell_type(map,2*x-x_m,2*y-y_m)==CELL_DOOR){ // 2*x-x_m =x+(x-x_m) or x-x_m est le déplacement selon l'axe x
      return 1;
  }
  switch(map_get_cell_type(map,x,y)) {
    case CELL_EMPTY:
    return 0;
    break;
    case CELL_BOMB_TTL0:
    return 0;
    break;
    case CELL_BONUS:
    return 0;
    break;
    default:
    return 1;
    break;
  }
}
void monster_set_heur(struct monster* monster,struct game* game) {//calculates heuristic function in all directions
  struct map* map=monster->map;
  int x=player_get_x(game_get_player(game));
  int y=player_get_y(game_get_player(game));
  int x_monster=monster_get_x(monster);
  int y_monster=monster_get_y(monster);
  monster->blocked=0;
  if(map_is_inside(map,monster->x,monster->y-1) && !monster_blocked(monster,monster->x,monster->y-1)) {
    monster->heur[0]=abs(x_monster-x)+abs(y_monster-1-y);
  }
  else {
    monster->heur[0]=100;
  }


  if(map_is_inside(map,monster->x+1,monster->y) && !monster_blocked(monster,monster->x+1,monster->y)) {
    monster->heur[1]=abs(x_monster+1-x)+abs(y_monster-y);
  }
  else {
    monster->heur[1]=100;
  }


 if(map_is_inside(map,monster->x,monster->y+1) && !monster_blocked(monster,monster->x,monster->y+1)) {
    monster->heur[2]=abs(x_monster-x)+abs(y_monster+1-y);
  }
  else {
    monster->heur[2]=100;
  }


  if(map_is_inside(map,monster->x-1,monster->y) && !monster_blocked(monster,monster->x-1,monster->y)) {
    monster->heur[3]=abs(x_monster-1-x)+abs(y_monster-y);
  }
  else {
    monster->heur[3]=100;
  }
  if(monster->heur[0]==100 && monster->heur[1]==100 && monster->heur[2]==100 && monster->heur[3]==100) {
    monster->blocked=1;
  }

}

int monster_min_heur(struct monster* monster,struct player* player) {//Returns the optimal moving direction for the monster to stalk the player
  int min=0;
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      if(monster->heur[i]>monster->heur[j]){
        min=j;
      }
    }
  }
  return min;

}

void monster_update(struct monster* monster,struct game* game) {//updates the monster's position and state(blocked or moving)
  monster->map=game_get_current_map(game);
  struct map* map = game_get_current_map(game);
  int x_monster=monster_get_x(monster);
  int y_monster=monster_get_y(monster);
  monster_set_heur(monster,game);
  int i=monster_min_heur(monster,game_get_player(game));
  if(monster->dead){
    monster=NULL;
   }
  else if(monster->blocked==1) {
     monster->blocked=0;
   }
  else {
  switch(i){
    case(NORTH):
    monster_set_dir(monster,0);
    monster_set_position(monster,x_monster,y_monster-1);
    if (map_get_cell_type(map,x_monster,y_monster-1)!= CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster,y_monster-1,CELL_MONSTER);
    }
    if ( map_get_cell_type(map,x_monster,y_monster)!=CELL_BONUS || map_get_cell_type(map,x_monster,y_monster)!=CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster,y_monster,CELL_EMPTY);
    }

    break;

    case(EAST):
    monster_set_dir(monster,1);
    monster_set_position(monster,x_monster+1,y_monster);
    if (map_get_cell_type(map,x_monster+1,y_monster)!= CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster+1,y_monster,CELL_MONSTER);
    }
    if ( map_get_cell_type(map,x_monster,y_monster)!=CELL_BONUS || map_get_cell_type(map,x_monster,y_monster)!=CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster,y_monster,CELL_EMPTY);
    }

    break;

    case(SOUTH):
    monster_set_dir(monster,2);
    monster_set_position(monster,x_monster,y_monster+1);
    if (map_get_cell_type(map,x_monster,y_monster+1)!= CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster,y_monster+1,CELL_MONSTER);
    }
    if (map_get_cell_type(map,x_monster,y_monster)!=CELL_BONUS || map_get_cell_type(map,x_monster,y_monster)!=CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster,y_monster,CELL_EMPTY);
    }

    break;

    case(WEST):
    monster_set_dir(monster,3);
    monster_set_position(monster,x_monster-1,y_monster);
    if (map_get_cell_type(map,x_monster-1,y_monster)!= CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster-1,y_monster,CELL_MONSTER);
    }
    if (map_get_cell_type(map,x_monster,y_monster)!=CELL_BONUS || map_get_cell_type(map,x_monster,y_monster)!=CELL_BOMB_TTL0 ){
      map_set_cell_type(map,x_monster,y_monster,CELL_EMPTY);
    }

    break;
}
}
}

void kill_monster(struct monster* monster){
  assert(monster);
  monster->dead=1;
}
int monster_is_dead(struct monster* monster) {
  assert(monster);
  return monster->dead;
}
int monster_get_level(struct monster* monster){
  assert(monster);
  return monster->level;
}

int monster_get_timer(struct monster* monster){
	assert(monster);
	return monster->monster_timer;
}

void monster_set_timer(struct monster* monster,int monster_timer) {
  assert(monster);
  monster->monster_timer=monster_timer;
}

void monster_display(struct game* game){
  struct monster** monsters=game_get_monsters(game);
  for(int i=0;i<MAX_MONSTERS;i++){
    if(monsters[i]!=NULL){//to test if a monster is still in a box
     if(monsters[i]->level==game_get_level(game) && monsters[i]->dead==0){ //tests if monster is in current map and not dead
       window_display_image(sprite_get_monster(monsters[i]->monster_direction),monsters[i]->x*SIZE_BLOC,monsters[i]->y*SIZE_BLOC);//displays monster on current map
     }
    }
  }
}
