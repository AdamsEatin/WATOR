/*
	Authors: Adam Eaton, Yongmin Park
*/
class Ocean {
public:
	int type;
	int pos_x;
	int pos_y;
	int age;
	int starve;
	int moved;
	int turn;

	void create_animal(int t, int x, int y);
	void wrap_ocean(int pos_x, int pos_y, int pos_a, int pos_b, int pos_c, int pos_d, int rows, int columns);
	char show_animal();
	// potentially have to remove
	//bool is_alive();
};
