#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_CITIES 100000

typedef struct {
    int id;
    int x, y;
    bool visited;
} City;

City cities[MAX_CITIES];
int city_count = 0;
int penalty = 0;

int tour[MAX_CITIES]; // indices of visited cities
int visited_count = 0;

#include <math.h>

int distance(City a, City b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return (int)(round(sqrt(dx * dx + dy * dy)));
}

void read_input(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Input file");
        exit(1);
    }

    fscanf(f, "%d", &penalty);
    int id, x, y;
    while (fscanf(f, "%d %d %d", &id, &x, &y) == 3) {
        cities[city_count++] = (City){id, x, y, true};
    }

    fclose(f);
}

void build_initial_tour() {
    visited_count = 0;
    for (int i = 0; i < city_count; i++) {
        if (cities[i].visited) {
            tour[visited_count++] = i;
        }
    }
}

int compute_tour_length() {
    if (visited_count < 2) return 0;
    int total = 0;
    for (int i = 0; i < visited_count - 1; i++) {
        total += distance(cities[tour[i]], cities[tour[i + 1]]);
    }
    total += distance(cities[tour[visited_count - 1]], cities[tour[0]]);
    return total;
}

int compute_total_cost() {
    int tour_len = compute_tour_length();
    int penalty_sum = 0;
    for (int i = 0; i < city_count; i++) {
        if (!cities[i].visited)
            penalty_sum += penalty;
    }
    return tour_len + penalty_sum;
}

void try_remove_city() {
    bool improved = true;

    while (improved) {
        improved = false;

        for (int i = 0; i < city_count; i++) {
            if (!cities[i].visited || visited_count <= 3)
                continue;

            // Save old state
            cities[i].visited = false;
            build_initial_tour();
            int new_cost = compute_total_cost();

            cities[i].visited = true;
            build_initial_tour();
            int old_cost = compute_total_cost();

            if (new_cost < old_cost) {
                cities[i].visited = false;
                improved = true;
                build_initial_tour(); // update tour
            }
        }
    }
}

void apply_2opt() {
    bool improved = true;

    while (improved) {
        improved = false;
        for (int i = 0; i < visited_count - 1; i++) {
            for (int j = i + 2; j < visited_count - (i == 0 ? 1 : 0); j++) {
                int a = tour[i];
                int b = tour[i + 1];
                int c = tour[j];
                int d = tour[(j + 1) % visited_count];

                int d_old = distance(cities[a], cities[b]) + distance(cities[c], cities[d]);
                int d_new = distance(cities[a], cities[c]) + distance(cities[b], cities[d]);

                if (d_new < d_old) {
                    // reverse segment (i+1 to j)
                    for (int l = i + 1, r = j; l < r; l++, r--) {
                        int tmp = tour[l];
                        tour[l] = tour[r];
                        tour[r] = tmp;
                    }
                    improved = true;
                }
            }
        }
    }
}

void write_output(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Output file");
        exit(1);
    }

    int total_cost = compute_total_cost();
    fprintf(f, "%d %d\n", total_cost, visited_count);
    for (int i = 0; i < visited_count; i++) {
        fprintf(f, "%d\n", cities[tour[i]].id);
    }
    fprintf(f, "\n");
    fclose(f);
}

int main() {
    read_input("input.txt");

    for (int i = 0; i < city_count; i++) {
        cities[i].visited = true;
    }

    build_initial_tour();

    try_remove_city();
    apply_2opt();

    write_output("output.txt");
    return 0;
}