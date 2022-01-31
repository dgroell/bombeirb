#ifndef BOMB_H_
#define BOMB_H_

#include <player.h>





struct Bombs;

struct Bombs* bomb_init(struct game* game);

int bomb_get_TTL(struct Bombs * bomb);
void bomb_launch(struct Bombs* bomb,struct player* player, struct game* game);
void bomb_propagate(struct Bombs* bomb, struct game* game);
void bomb_display(struct Bombs* bomb);
void Dec_TTL_bomb(struct Bombs* bomb);
void free_bomb(struct Bombs *bomb);
int bomb_gettimer(struct Bombs* bomb);
void bomb_settimer(struct Bombs* bomb, int timer);
void bomb_set_next(struct Bombs *bomb, struct Bombs *bombnext);
struct Bombs* bomb_get_next(struct Bombs* bomb);
void bomb_chain_update(struct game* game, struct Bombs* bomb, int timer);
void bomb_disipate(int x, int y, struct map* map, struct game* game);
void bomb_explode(int x, int y,struct map* map);
#endif /* BOMB_H_ */
