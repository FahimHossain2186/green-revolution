#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESOURCES 100
#define MAX_TURNS 100
#define MAX_PURCHASES 50

typedef struct {
    int resource_id, activation_cost, maintenance_cost, active_duration;
    int downtime, lifespan, power_capacity, effect_value;
    char special_effect;
} Resource;

typedef struct {
    int min_buildings, max_buildings, profit_per_building;
} Turn;

void read_input(const char *file_path, int *initial_budget, int *total_resources, int *total_turns, Resource resources_list[MAX_RESOURCES], Turn turns_list[MAX_TURNS]) {
    
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fscanf(file, "%d %d %d", initial_budget, total_resources, total_turns);

    for (int i = 0; i < *total_resources; i++) {
        fscanf(file, "%d %d %d %d %d %d %d %c %d", &resources_list[i].resource_id, &resources_list[i].activation_cost,
               &resources_list[i].maintenance_cost, &resources_list[i].active_duration, &resources_list[i].downtime,
               &resources_list[i].lifespan, &resources_list[i].power_capacity, &resources_list[i].special_effect,
               &resources_list[i].effect_value);
    }

    for (int i = 0; i < *total_turns; i++) {
        fscanf(file, "%d %d %d", &turns_list[i].min_buildings, &turns_list[i].max_buildings,
               &turns_list[i].profit_per_building);
    }
    fclose(file);
}

int compare_resources(const void *a, const void *b) {
    Resource *resA = (Resource *)a;
    Resource *resB = (Resource *)b;
    return (resB->power_capacity - resA->power_capacity) ?: (resA->activation_cost - resB->activation_cost);
}

void simulate_game(int initial_budget, int total_resources, int total_turns,
                   Resource resources_list[MAX_RESOURCES], Turn turns_list[MAX_TURNS]) {
    int available_budget = initial_budget;
    int active_resources[MAX_RESOURCES] = {0};
    int resource_lifetime[MAX_RESOURCES] = {0};

    FILE *output_file = fopen("output.txt", "w");
    if (!output_file) {
        perror("Error opening output file");
        exit(1);
    }

    for (int t = 0; t < total_turns; t++) {
        int purchased_resources[MAX_PURCHASES], purchased_count = 0;

        qsort(resources_list, total_resources, sizeof(Resource), compare_resources);
        
        for (int i = 0; i < total_resources && purchased_count < MAX_PURCHASES; i++) {
            if (available_budget >= resources_list[i].activation_cost) {
                available_budget -= resources_list[i].activation_cost;
                active_resources[i] = 1;
                resource_lifetime[i] = resources_list[i].lifespan;
                purchased_resources[purchased_count++] = resources_list[i].resource_id;
            }
        }

        int powered_buildings = 0, maintenance_cost = 0;
        for (int i = 0; i < total_resources; i++) {
            if (active_resources[i] && resource_lifetime[i] > 0) {
                powered_buildings += resources_list[i].power_capacity;
                maintenance_cost += resources_list[i].maintenance_cost;
                resource_lifetime[i]--;
                if (resource_lifetime[i] == 0) active_resources[i] = 0;
            }
        }

        int profit = (powered_buildings >= turns_list[t].min_buildings) ?
                    (powered_buildings < turns_list[t].max_buildings ? powered_buildings : turns_list[t].max_buildings) * turns_list[t].profit_per_building : 0;
        
        available_budget += profit - maintenance_cost;

        if (purchased_count > 0) {
            fprintf(output_file, "%d %d", t, purchased_count);
            for (int i = 0; i < purchased_count; i++) {
                fprintf(output_file, " %d", purchased_resources[i]);
            }
            fprintf(output_file, "\n");
        }
    }
    fclose(output_file);
    printf("Simulation complete. Results saved in output.txt\n");
}

int main() {
    int initial_budget, total_resources, total_turns;
    Resource resources_list[MAX_RESOURCES];
    Turn turns_list[MAX_TURNS];

    read_input("Green_Revolution_Game.txt", &initial_budget, &total_resources, &total_turns, resources_list, turns_list);
    simulate_game(initial_budget, total_resources, total_turns, resources_list, turns_list);
    
    return 0;
}