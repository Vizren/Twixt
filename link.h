#ifndef LINK_H
#define LINK_H

#define MAX_LINKS 5000

typedef struct {
    int r1, c1;
    int r2, c2;
    int owner;
} Link;

typedef struct {
    Link links[MAX_LINKS];
    int count;
} LinkSet;

void linkset(LinkSet *set);
int linkadd(LinkSet *set, int r1, int c1, int r2, int c2, int owner);
void linkprint(const LinkSet *set,int owner);
void canonical_order(int *r1, int *c1, int *r2, int *c2);

// Geometry Helpers
int link_on_segment(int pr, int pc, int qr, int qc, int rr, int rc);
int link_orientation(int pr, int pc, int qr, int qc, int rr, int rc);
int link_segments_intersect(int p1r, int p1c, int q1r, int q1c,int p2r, int p2c, int q2r, int q2c);

int link_links_intersect(const Link *A, const Link *B);

int link_player_wins(const LinkSet *set, int player, int rows, int cols);

#endif
