#include "link.h"
#include <stdio.h>
#include<stdlib.h>

//initialise set
void linkset(LinkSet *set){
    set->count = 0;
}

//Store links in sorted form
void canonical_order(int *r1, int *c1, int *r2, int *c2){
    if (*r2 < *r1 || (*r2 == *r1 && *c2 < *c1)) {
        int tr = *r1, tc = *c1;
        *r1 = *r2; *c1 = *c2;
        *r2 = tr;  *c2 = tc;
    }
}

// GEOMETRY HELPERS

int link_on_segment(int pr, int pc, int qr, int qc, int rr, int rc){
    return (qr <= (pr > rr ? pr : rr) && qr >= (pr < rr ? pr : rr) && qc <= (pc > rc ? pc : rc) && qc >= (pc < rc ? pc : rc));
}

//check orientation
int link_orientation(int pr, int pc, int qr, int qc, int rr, int rc){
    long long val = (long long)(qc - pc) * (rr - qr) - (long long)(qr - pr) * (rc - qc);
    if (val == 0) return 0;    //collinear
    return (val > 0) ? 1 : 2;  //1->clockwise 2->anti-clockwise
}

int link_segments_intersect(int p1r, int p1c, int q1r, int q1c,int p2r, int p2c, int q2r, int q2c){
    
    //Shared endpoint
    if ((p1r == p2r && p1c == p2c) || (p1r == q2r && p1c == q2c) || (q1r == p2r && q1c == p2c) || (q1r == q2r && q1c == q2c)){
        return 0;
    }

    int o1 = link_orientation(p1r, p1c, q1r, q1c, p2r, p2c);
    int o2 = link_orientation(p1r, p1c, q1r, q1c, q2r, q2c);
    int o3 = link_orientation(p2r, p2c, q2r, q2c, p1r, p1c);
    int o4 = link_orientation(p2r, p2c, q2r, q2c, q1r, q1c);

    if (o1 != o2 && o3 != o4)
        return 1;

    //Collinear special cases
    if (o1 == 0 && link_on_segment(p1r, p1c, p2r, p2c, q1r, q1c)) return 1;
    if (o2 == 0 && link_on_segment(p1r, p1c, q2r, q2c, q1r, q1c)) return 1;
    if (o3 == 0 && link_on_segment(p2r, p2c, p1r, p1c, q2r, q2c)) return 1;
    if (o4 == 0 && link_on_segment(p2r, p2c, q1r, q1c, q2r, q2c)) return 1;

    return 0;
}

//convinience wrapper
int link_links_intersect(const Link *A, const Link *B){
    return link_segments_intersect(A->r1, A->c1, A->r2, A->c2,B->r1, B->c1, B->r2, B->c2);
}

int linkadd(LinkSet *set, int r1, int c1, int r2, int c2, int owner){
    canonical_order(&r1, &c1, &r2, &c2);

    //Reject duplicates
    for (int i = 0; i < set->count; i++) {
        Link *L = &set->links[i];
        if (L->owner == owner && L->r1 == r1 && L->c1 == c1 && L->r2 == r2 && L->c2 == c2){
            return 0;
        }
    }

    Link newL = { r1, c1, r2, c2, owner };

    //Check for intersections
    for (int i = 0; i < set->count; i++) {
        if (link_links_intersect(&newL, &set->links[i])) return 0;
    }

    if (set->count >= MAX_LINKS) return 0;

    set->links[set->count++] = newL;
    return 1;
}

void linkprint(const LinkSet *set,int owner){
    int found=0;
    for (int i = 0; i < set->count; ++i) {
        const Link *L = &set->links[i];
        if(L->owner==owner){
            printf("    (%d,%d) <-> (%d,%d)\n",L->r1, L->c1, L->r2, L->c2);
            found=1;
        }
    }
    if(!found){
        printf("    No Links for Player %d\n",owner);
    }
}

//WINNING CHECK

//Array of pegs that are part of links
static int link_find_or_add(int *nodes_r, int *nodes_c, int *node_count,int max_nodes, int r, int c){
    for (int i = 0; i < *node_count; ++i) {
        if (nodes_r[i] == r && nodes_c[i] == c) return i;
    }
    if (*node_count >= max_nodes) return -1;
    nodes_r[*node_count] = r;
    nodes_c[*node_count] = c;
    return (*node_count)++;
}

//recursive search
static int link_search(int cur, int *adj_counts, int **adj, char *visited, int *target_flags){
    if (target_flags[cur]) return 1;
    visited[cur] = 1;
    for (int i = 0; i < adj_counts[cur]; ++i) {
        int nb = adj[cur][i];
        if (!visited[nb]) {
            if (link_search(nb, adj_counts, adj, visited, target_flags))
                return 1;
        }
    }
    return 0;
}

//Public winning check
int link_player_wins(const LinkSet *set, int player, int rows, int cols){
    if (!set || set->count == 0) return 0;

    //count player's links
    int player_links = 0;
    for (int i = 0; i < set->count; ++i) if (set->links[i].owner == player) player_links++;
    if (player_links == 0) return 0;

    int max_nodes = player_links * 2;
    int *nodes_r = malloc(sizeof(int) * max_nodes);
    int *nodes_c = malloc(sizeof(int) * max_nodes);
    if (!nodes_r || !nodes_c) { free(nodes_r); free(nodes_c); return 0; }
    int node_count = 0;

    //collect nodes
    for (int i = 0; i < set->count; ++i) {
        const Link *L = &set->links[i];
        if (L->owner != player) continue;
        if (link_find_or_add(nodes_r, nodes_c, &node_count, max_nodes, L->r1, L->c1) < 0) goto cleanup_fail;
        if (link_find_or_add(nodes_r, nodes_c, &node_count, max_nodes, L->r2, L->c2) < 0) goto cleanup_fail;
    }

    //build adjacency
    int *adj_counts = calloc(node_count, sizeof(int));
    int **adj = malloc(sizeof(int*) * node_count);
    if (!adj || !adj_counts) goto cleanup_fail;
    for (int i = 0; i < node_count; ++i) {
        adj[i] = malloc(sizeof(int) * (player_links + 1));
        if (!adj[i]) {
            for (int j = 0; j < i; ++j) free(adj[j]);
            free(adj); free(adj_counts);
            goto cleanup_fail;
        }
    }

    for (int i = 0; i < set->count; ++i) {
        const Link *L = &set->links[i];
        if (L->owner != player) continue;
        int u = link_find_or_add(nodes_r, nodes_c, &node_count, max_nodes, L->r1, L->c1);
        int v = link_find_or_add(nodes_r, nodes_c, &node_count, max_nodes, L->r2, L->c2);
        if (u < 0 || v < 0) continue;
        adj[u][adj_counts[u]++] = v;
        adj[v][adj_counts[v]++] = u;
    }

    //mark targets and starts
    int *target_flags = calloc(node_count, sizeof(int));
    char *visited = calloc(node_count, 1);
    if (!target_flags || !visited) goto cleanup_adjfail;

    int any_start = 0;
    for (int i = 0; i < node_count; ++i) {
        int r = nodes_r[i], c = nodes_c[i];
        if (player == 1) {
            if (r == 1) any_start = 1;
            if (r == rows) target_flags[i] = 1;
        } else {
            if (c == 1) any_start = 1;
            if (c == cols) target_flags[i] = 1;
        }
    }
    if (!any_start) {
        for (int i = 0; i < node_count; ++i) free(adj[i]);
        free(adj); free(adj_counts);
        free(nodes_r); free(nodes_c);
        free(target_flags); free(visited);
        return 0;
    }

    //try all start nodes
    int win = 0;
    for (int i = 0; i < node_count && !win; ++i) {
        int r = nodes_r[i], c = nodes_c[i];
        int is_start = (player == 1 ? (r == 1) : (c == 1));
        if (!is_start) continue;
        if (!visited[i]) {
            if (link_search(i, adj_counts, adj, visited, target_flags)) {
                win = 1;
                break;
            }
        }
    }

    for (int i = 0; i < node_count; ++i) free(adj[i]);
    free(adj); free(adj_counts);
    free(nodes_r); free(nodes_c);
    free(visited); free(target_flags);
    return win;

cleanup_adjfail:
    for (int i = 0; i < node_count; ++i) free(adj[i]);
    free(adj); free(adj_counts);
cleanup_fail:
    free(nodes_r); free(nodes_c);
    return 0;
}