#ifndef MONSTER_H_
#define MONSTER_H_

struct monster;

struct monster* monster_init(int level, struct map* map);
void monster_set_position(struct monster* monster, int x,int y);
int monster_get_dir(struct monster* monster);
void monster_set_dir(struct monster*monster,int dir);
int monster_get_x(struct monster* monster);
int monster_get_y(struct monster* monster);
int monster_get_timer(struct monster* monster);
void monster_display(struct monster* monster);
void monster_set_timer(struct monster* monster, int monster_timer);
int monster_blocked(struct monster *monster,int x,int y);
void monster_set_heur(struct monster* monster,struct game* game);
int monster_min_heur(struct monster* monster);
int monster_get_level(struct monster* monster);
void monster_update(struct monster* monster,struct game* game);
void kill_monster(struct monster* monster);
int monster_is_dead(struct monster* monster);
#endif /* MONSTER_H_ */
