
#include <SDL/SDL_image.h>
#include <constant.h>
#include <game.h>
#include <map.h>
#include <monster.h>
#include <misc.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <assert.h>


struct Bombs { // Bomb structure definition using chained list
    int x,y;
    int TTL;
    int timer;
    struct map* map;
    struct Bombs* next;
    int stop_up;
    int stop_down;
    int stop_left;
    int stop_right;
};


struct Bombs* bomb_init(struct game* game){//initialises a bomb
    struct Bombs* bomb = malloc(sizeof(*bomb));
    if (!bomb){
	error("Memory error");
}
    bomb->TTL = 5; //TTL=5 so as to see the explosion and create damage and make bonuses appear
    bomb->timer=0;
    bomb->next=NULL;
    bomb->map=game_get_current_map(game);//so as to link the bomb to the map and not the game
    bomb->stop_up=0;
    bomb->stop_down=0;
    bomb->stop_right=0;
    bomb->stop_left=0;
    return bomb;
}

int bomb_get_TTL(struct Bombs* bomb){
  return bomb->TTL; //returns a bomb's TTL
}

void Dec_TTL_bomb(struct Bombs* bomb) {
    assert(bomb);
    bomb->TTL--; //Decreases a bomb's TTL
}

struct Bombs* bomb_get_next(struct Bombs* bomb) {
assert(bomb);
return bomb->next; //returns the next bomb in chained list
}

void bomb_set_next(struct Bombs *bomb,struct Bombs *bombnext) {
  assert(bomb);
  bomb->next=bombnext; //sets next bomb in listed chain
}


void bomb_settimer(struct Bombs* bomb,int timer) {
  assert(bomb);
  bomb->timer=timer;
}

int bomb_gettimer(struct Bombs* bomb){
  assert(bomb);
  return bomb->timer; //returns bomb timer
}


void bomb_launch(struct Bombs* bomb,struct player* player, struct game* game){//sets initialized bomb's x y coordinates
  if (player_get_nb_bomb(player)>0){
    bomb->x=player_get_x(player);
    bomb->y=player_get_y(player);
  if(game_get_first_bomb(game)==NULL) {//tests if the bomb is first in listed chain
    game_set_first_bomb(game,bomb);
  }
  else { //the bomb is inserted into the chained list
    game_insert_bomb(game,bomb);
  }
 }
}



void bomb_propagate(struct Bombs* bomb,struct game* game) {//changes bomb's state according to its TTL
  struct map* map=bomb->map;
  struct player* player=game_get_player(game);
  int x=bomb->x;
  int y=bomb->y;
  int range=player_get_range(game_get_player(game)); //get propagation range
  if(bomb_get_TTL(bomb)==5){
    map_set_cell_type(map, x, y, CELL_BOMB_TTL4);
    game_display(game);
  }
  if(bomb_get_TTL(bomb)==4){
    map_set_cell_type(map, x, y, CELL_BOMB_TTL3);
    game_display(game);
  }

  if(bomb_get_TTL(bomb)==3){
    map_set_cell_type(map,x, y, CELL_BOMB_TTL2);
    game_display(game);
  }

  if(bomb_get_TTL(bomb)==2){
    map_set_cell_type(map, x, y, CELL_BOMB_TTL1);
    game_display(game);
  }

  if(bomb_get_TTL(bomb)==1){
    map_set_cell_type(map, x, y, CELL_BOMB_TTL0);
    game_display(game);
    int i=1;
    while(i!=range+1){//set destructible cells to explosion cell
      if(map_is_inside(map,x+i,y)){
       if((map_get_cell_type(map,x+i,y)!=CELL_EMPTY && map_get_cell_type(map,x+i,y)!=CELL_BOX && map_get_cell_type(map,x+i,y)!=CELL_BONUS && map_get_cell_type(map,x+i,y)!=CELL_MONSTER) || bomb->stop_right){
        bomb->stop_right=1;//stops bomb from propagating further on the right
        }
        else if(map_get_cell_type(map,x+i,y)==CELL_BOX && !bomb->stop_right){
          bomb_explode(x+i,y,map);
          bomb->stop_right=1;
        }
       else {
        bomb_explode(x+i,y,map);
        }
      }
    if(map_is_inside(map,x,y+i)){
     if((map_get_cell_type(map,x,y+i)!=CELL_EMPTY && map_get_cell_type(map,x,y+i)!=CELL_BOX && map_get_cell_type(map,x,y+i)!=CELL_BONUS && map_get_cell_type(map,x,y+i)!=CELL_MONSTER) || bomb->stop_down){
      bomb->stop_down=1;//stops bomb from propagating further downward
      }
      else if(map_get_cell_type(map,x,y+i)==CELL_BOX && !bomb->stop_down){
        bomb_explode(x,y+i,map);
        bomb->stop_down=1;
      }
     else {
      bomb_explode(x,y+i,map);
      }
    }
    if(map_is_inside(map,x-i,y)){
     if((map_get_cell_type(map,x-i,y)!=CELL_EMPTY && map_get_cell_type(map,x-i,y)!=CELL_BOX && map_get_cell_type(map,x-i,y)!=CELL_BONUS && map_get_cell_type(map,x-i,y)!=CELL_MONSTER) || bomb->stop_left){
      bomb->stop_left=1;//stops bomb from propagating further on the right if it's a cell_scenery
     }
     else if(map_get_cell_type(map,x-i,y)==CELL_BOX && !bomb->stop_left){
       bomb_explode(x-i,y,map);
       bomb->stop_left=1;//stops bomb from propagating further than this box
     }
     else {
      bomb_explode(x-i,y,map);//propagates bomb normally
     }
    }
    if(map_is_inside(map,x,y-i)){
     if((map_get_cell_type(map,x,y-i)!=CELL_EMPTY && map_get_cell_type(map,x,y-i)!=CELL_BOX && map_get_cell_type(map,x,y-i)!=CELL_BONUS && map_get_cell_type(map,x,y-i)!=CELL_MONSTER) || bomb->stop_up){
       bomb->stop_up=1;//stops bomb from propagating further upward if it's a cell_scenery
     }
     else if(map_get_cell_type(map,x,y-i)==CELL_BOX && !bomb->stop_up){
       bomb_explode(x,y-i,map);
       bomb->stop_up=1;//stops bomb from propagating further on the right if it's a cell_scenery
     }
     else {
      bomb_explode(x,y-i,map);
     }
   }
      i++;
 }
}
  if (bomb_get_TTL(bomb)==0){
    int i=range;
    map_set_cell_type(map, x, y, CELL_EMPTY);
    while(i!=0){ //disipate the bomb and show potential bonus
      bomb_disipate(x+i,y,map,game);
      bomb_disipate(x,y+i,map,game);
      bomb_disipate(x-i,y,map,game);
      bomb_disipate(x,y-i,map,game);
      i--;
    }
    bomb=NULL;
    game_dec_activebombs(game);
    player_inc_nb_bomb(player);
  }

}

void bomb_explode(int x, int y,struct map* map) {//replace affected cells with explosion cells having bonus types
  if(map_is_inside(map,x,y) && map_get_cell_type(map,x,y)==CELL_EMPTY){
    map_set_cell_type(map,x,y,CELL_BOMB_TTL0);
  }
  if(map_is_inside(map,x,y) && map_get_cell_type(map,x,y)==CELL_BONUS){
    map_set_cell_type(map,x,y,CELL_BOMB_TTL0);//destroys bonus
  }
  if(map_is_inside(map,x,y) && map_get_cell_type(map,x,y)==CELL_MONSTER){
    map_set_cell_type(map,x,y,CELL_BOMB_TTL0);//destroys bonus
  }
  else if(map_is_inside(map,x,y) && map_get_cell_type(map,x,y)==CELL_BOX) {

    if(map_get_compose_cell_type(map,x,y)==NO_BONUS) {
      map_set_cell_type(map,x,y,CELL_BOMB_TTL0);
    }
    if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_RANGE_DEC){
      map_set_cell_type(map,x,y,CELL_EXPLOSION_BOMB_RANGE_DEC);
    }
    if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_RANGE_INC){
        map_set_cell_type(map,x,y,CELL_EXPLOSION_BOMB_RANGE_INC);
    }

    if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_NB_DEC){
       map_set_cell_type(map,x,y,CELL_EXPLOSION_BOMB_NB_DEC);
    }

    if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_NB_INC){
       map_set_cell_type(map,x,y,CELL_EXPLOSION_BOMB_NB_INC);
    }

    if(map_get_compose_cell_type(map,x,y)==BONUS_MONSTER){
       map_set_cell_type(map,x,y,CELL_EXPLOSION_MONSTER);
    }

    if(map_get_compose_cell_type(map,x,y)==BONUS_LIFE){
      map_set_cell_type(map,x,y,CELL_EXPLOSION_LIFE);
    }
  }


}

void bomb_disipate(int x, int y, struct map* map, struct game* game) {
 if(map_is_inside(map,x,y) && map_get_cell_type(map,x,y)==CELL_BOMB_TTL0){ //Replace explosion cells with empty cells or bonus cells potentially
  if(map_get_compose_cell_type(map,x,y)==NO_BONUS) {
    map_set_cell_type(map,x,y,CELL_EMPTY);
  }

  if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_RANGE_DEC){
    map_set_cell_type(map,x,y,CELL_BONUS_BOMB_RANGE_DEC);
  }

  if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_RANGE_INC){
      map_set_cell_type(map,x,y,CELL_BONUS_BOMB_RANGE_INC);
  }

  if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_NB_DEC){
     map_set_cell_type(map,x,y,CELL_BONUS_BOMB_NB_DEC);
  }

  if(map_get_compose_cell_type(map,x,y)==BONUS_BOMB_NB_INC){
     map_set_cell_type(map,x,y,CELL_BONUS_BOMB_NB_INC);
   }

  if(map_get_compose_cell_type(map,x,y)==BONUS_MONSTER){
    map_set_cell_type(map,x,y,CELL_EMPTY);
    struct monster* monster=monster_init(game_get_level(game),game_get_current_map(game));//initialize monster from malus
    monster_set_position(monster,x,y);//set monster's position as cell explosion's position
    game_set_monster(game,monster,map_get_bonus_monster(game_get_current_map(game)));//put monster into game and map
  }

  if(map_get_compose_cell_type(map,x,y)==BONUS_LIFE){
    map_set_cell_type(map,x,y,CELL_BONUS_LIFE);
  }
 }
}

void bomb_chain_update(struct game* game, struct Bombs* bomb,int timer) {//updates all active bombs
  if(bomb!=NULL){//if the bomb is active
  int prevtime=bomb_gettimer(bomb);
  if(timer-prevtime>=1000){//tests if one second has elpased
  Dec_TTL_bomb(bomb);
  bomb_settimer(bomb,timer);
  bomb_propagate(bomb,game);
  }
 }
}
