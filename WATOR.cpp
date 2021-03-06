/*
Authors: Adam Eaton, Yongmin Park

TODO;
-Implement Timing
-Use 2D vector instead of Array to alow for user input for rows, columns
*/

#include "Ocean.h"

#include <omp.h>
#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>


using namespace std;

//Setting world variables, creating the map
int const rows = 15;
int const columns = 30;

Ocean ocean_map[rows][columns];

int fish_breed = 0;
int starting_fish = 0;

int shark_life = 0;
int shark_starve = 0;
int shark_breed = 0;
int starting_sharks = 0;

int turns = 0;
int max_turns = 500;

void generate_maps() {
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < columns; y++) {
			ocean_map[x][y].create_animal(0, x, y);
		}
	}
}

void populate_ocean() {
	int total_fish = 0;
	int total_sharks = 0;
	int total_animals = total_fish + total_sharks;
	int max_animals = rows * columns;
	int a = 0;

	while (total_sharks != starting_sharks && total_fish != starting_fish) {
		for (int x = 0; x < rows; x++) {
			for (int y = 0; y < columns; y++) {
				int animal_code = rand() % 3;

				if (ocean_map[x][y].type == 0) {
					if (animal_code == 1) {
						if (total_fish < starting_fish) {
							total_fish++;
							ocean_map[x][y].create_animal(animal_code, x, y);
							ocean_map[x][y].age = rand() % fish_breed;
						}
					}
					if (animal_code == 2) {
						if (total_sharks < starting_sharks) {
							total_sharks++;
							ocean_map[x][y].create_animal(animal_code, x, y);
							ocean_map[x][y].age = rand() % shark_breed;
						}
					}
				}
			}
		}
	}

}

bool display_map() {
	int fish_sum = 0;
	int shark_sum = 0;
	
	for (int x = 0; x < rows; ++x) {
		for (int y = 0; y < columns; ++y) {
			cout << ocean_map[x][y].show_animal() << " ";
			
			if (ocean_map[x][y].type == 1) {
				fish_sum++;
				
			}
			else if (ocean_map[x][y].type == 2) {
				shark_sum++;
			}
		}
		cout << endl;
	}
	if (fish_sum == 0 || shark_sum == 0) {
		return false;
	}
	return true;
}

void display_results() {
	int final_fish = 0;
	int final_sharks = 0;

	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < columns; y++) {
			if (ocean_map[x][y].type == 1) {
				final_fish++;
			}
			if (ocean_map[x][y].type == 2) {
				final_sharks++;
			}
		}
	}
	cout << "Total Fish: " << final_fish << "\nTotal Sharks: " << final_sharks << "\nTurns: " << turns << endl;
	return;
}

bool get_input() {
	int max_count = rows * columns;

	cout << "Welcome to the WATOR Simulation!" << endl;
	cout << "Please keep the sum of animals below: " << max_count << endl;
	cout << "\n**********************************************" << endl;
	
	cout << "\nStarting Fish: ";
	cin >> starting_fish;

	cout << "\nHow often Fish can breed: ";
	cin >> fish_breed;

	cout << "\nStarting Sharks: ";
	cin >> starting_sharks;
	
	cout << "\nHow often Sharks can breed: ";
	cin >> shark_breed;

	cout << "\nHow long a Shark can survive without food: ";
	cin >> shark_starve;

	/* Commented out as they are required to be constants. Need to change to a 2D Vector.
	cout << "\nX value for the size of the ocean: ";
	cin >> rows;

	cout << "\nY value for the size of the ocean: ";
	cin >> columns;
	*/

	int entered_sum = starting_sharks + starting_fish;

	if (entered_sum > max_count) {
		return false;
	}
	return true;
}


void fish_move(int x, int y) {
	int pos_a = 0;
	int pos_b = 0;
	int pos_c = 0;
	int pos_d = 0;

	ocean_map[x][y].wrap_ocean(x, y, pos_a, pos_b, pos_c, pos_d, rows, columns);

	Ocean temp[8] = { ocean_map[x][pos_d], ocean_map[pos_c][pos_d],
		ocean_map[pos_c][y], ocean_map[pos_c][pos_b],
		ocean_map[x][pos_b], ocean_map[pos_a][pos_b],
		ocean_map[pos_a][y], ocean_map[pos_a][pos_d] };

	bool can_move = false;
	vector<int> moves;

	//checking that there is at least 1 free available space
	for (int a = 0; a < 8; a++) {
		int temp_x = temp[a].pos_x;
		int temp_y = temp[a].pos_y;
		
		if (ocean_map[temp_x][temp_y].type == 0 && ocean_map[temp_x][temp_y].moved == 0) {
			moves.push_back(a);
			can_move = true;
		}
	}

	//if there's no open space, fish does nothing, return..
	if (can_move == false) {
		return;
	}
	
	//Choosing a random available position from the vector of positions.
	int rand_pos = rand() % moves.size();

	//check fish' age, set it's new coordinates
	int curr_fish_age = ocean_map[x][y].age;
	int new_x = temp[rand_pos].pos_x;
	int new_y = temp[rand_pos].pos_y;

	//if fish is at the age to breed
	//move to new spot, reset age,
	//create new fish in previous spot, set age to 0
	ocean_map[new_x][new_y].create_animal(1, new_x, new_y);

	if (curr_fish_age >= fish_breed) {	
		ocean_map[new_x][new_y].age = 0;
		ocean_map[new_x][new_y].moved = 1;
		ocean_map[x][y].create_animal(1, x, y);
		ocean_map[x][y].age = 0;
	}
	else {
		ocean_map[new_x][new_y].age = curr_fish_age;
		ocean_map[new_x][new_y].moved = 1;
		ocean_map[x][y].create_animal(0, x, y);
	}
}

void shark_move(int x, int y) {
	if (ocean_map[x][y].starve == shark_starve) {
		ocean_map[x][y].create_animal(0, x, y);
		ocean_map[x][y].age = 0;
		ocean_map[x][y].starve = 0;
		return;
	}

	int pos_a = 0;
	int pos_b = 0;
	int pos_c = 0;
	int pos_d = 0;

	ocean_map[x][y].wrap_ocean(x, y, pos_a, pos_b, pos_c, pos_d, rows, columns);

	Ocean temp[8] = { ocean_map[x][pos_d], ocean_map[pos_c][pos_d],
		ocean_map[pos_c][y], ocean_map[pos_c][pos_b],
		ocean_map[x][pos_b], ocean_map[pos_a][pos_b],
		ocean_map[pos_a][y], ocean_map[pos_a][pos_d] };

	bool can_move = false;
	bool fish_present = false;
	vector<int> moves;
	vector<int> fish_positions;

	//checking that there is at least 1 free available space
	for (int a = 0; a < 8; a++) {
		int temp_x = temp[a].pos_x;
		int temp_y = temp[a].pos_y;

		if (ocean_map[temp_x][temp_y].type == 0 && ocean_map[temp_x][temp_y].moved == 0) {
			moves.push_back(a);
			can_move = true;
		}
		if (ocean_map[temp_x][temp_y].type == 1) {
			fish_positions.push_back(a);
			fish_present = true;
		}
	}

	//if there's no open space, shark does nothing, return..
	if (can_move == false) {
		return;
	}

	if (fish_present == true) {
		//Choosing a random available position from the vector of positions.
		int rand_pos = rand() % fish_positions.size();

		//check shark' age, set it's new coordinates
		int curr_shark_age = ocean_map[x][y].age;
		int curr_shark_starve = ocean_map[x][y].starve;
		int new_x = temp[rand_pos].pos_x;
		int new_y = temp[rand_pos].pos_y;

		//create new shark at given location
		ocean_map[new_x][new_y].create_animal(2, new_x, new_y);

		if (curr_shark_age >= shark_breed) {
			//Move shark, set new position variables
			ocean_map[new_x][new_y].age = 0;
			ocean_map[new_x][new_y].starve = 0;
			ocean_map[new_x][new_y].moved = 1;
			//Create new shark, set it's variables
			ocean_map[x][y].create_animal(2, x, y);
			ocean_map[x][y].age = 0;
			ocean_map[x][y].starve = 0;
		}
		else {
			//Move shark, set new position variables
			ocean_map[new_x][new_y].age = curr_shark_age;
			ocean_map[new_x][new_y].starve = 0;
			ocean_map[new_x][new_y].moved = 1;
			//Move shark, reset position to empty
			ocean_map[x][y].create_animal(0, x, y);
			ocean_map[x][y].age = 0;
			ocean_map[x][y].starve = 0;
		}
	}// if fish not present but there are free spaces
	else {

		//Choosing a random available position from the vector of positions.
		int rand_pos = rand() % moves.size();

		//check shark' age, set it's new coordinates
		int curr_shark_age = ocean_map[x][y].age;
		int curr_shark_starve = ocean_map[x][y].starve;
		int new_x = temp[rand_pos].pos_x;
		int new_y = temp[rand_pos].pos_y;
		
		//create new shark at given location
		ocean_map[new_x][new_y].create_animal(2, new_x, new_y);
	
		if (curr_shark_age >= shark_breed) {
			//Move shark, set new position variables
			ocean_map[new_x][new_y].age = 0;
			ocean_map[new_x][new_y].starve = curr_shark_starve;
			ocean_map[new_x][new_y].moved = 1;
			//Create new shark, set it's variables
			ocean_map[x][y].create_animal(2, x, y);
			ocean_map[x][y].age = 0;
			ocean_map[x][y].starve = 0;
		}
		else {
			//Move shark, set new position variables
			ocean_map[new_x][new_y].age = curr_shark_age;
			ocean_map[new_x][new_y].starve = curr_shark_starve;
			ocean_map[new_x][new_y].moved = 1;
			//Move shark, reset position to empty
			ocean_map[x][y].create_animal(0, x, y);
			ocean_map[x][y].age = 0;
			ocean_map[x][y].starve = 0;
		}
	}
}


void view_map() {
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < columns; y++) {
			if (ocean_map[x][y].type == 1 && ocean_map[x][y].moved == 0) {
				fish_move(x,y);
			}
			else if (ocean_map[x][y].type == 2 && ocean_map[x][y].moved == 0) {
				shark_move(x, y);
			}
		}
	}
	turns++;
	for (int a = 0; a < rows; a++) {
		for (int b = 0; b < columns; b++) {
			if (ocean_map[a][b].type == 1) {
				ocean_map[a][b].moved = 0;
			}
			if (ocean_map[a][b].type == 2) {
				ocean_map[a][b].moved = 0;
			}
		}
	}
}

int main()
{
	bool correct_input = false;
	correct_input = get_input();

	if (correct_input == true) {
		generate_maps();
		populate_ocean();
		display_map();
		system("clear");

		bool running = true;

		while (running == true && turns < max_turns) {
			view_map();
			running = display_map();
			system("clear");
		}

		display_results();
		
	}
	else {
		int max_possible = rows * columns;
		cout << "*** INPUT ERROR ***" << endl;
		cout << "Maximum number of animals was exceeded. Please try again." << endl;
	}
	
	return 0;
}

