#include <iostream>
#include <list>
#include <typeinfo>
#include <string>
#include <ctime>

using namespace std;

class Person;
class Character;

class Person {
public:
	int hit_points;
	Character* has_character;
	bool is_dead;
	void hit (Character* p);
	int get_hp() {return hit_points;};
};

class Character {
public:
	Person* is_attached_to;
	Character() {
		is_attached_to = new Person;
		is_attached_to->has_character = this;
		is_attached_to->hit_points = 100;
		is_attached_to->is_dead = false;
	};
	virtual ~Character() {}; //character destroyed

	virtual void convert(int){}
	virtual void attack (Character*) {}; //attacks Character*
	int get_character_type(); //returns 1 for Good, 2 for Bad, 3 for Zombie
	void died(); //destroys a dead character, as well as the person it is linked to
	void lower_hp();
	virtual void status() = 0;
};

class Good : public Character {
public:
	Good(){}; //default constructor
	~Good(){}; //default destructor

	void convert (int);
	void attack(Character*);
	void status() { cout << "ok" << endl;}
};

class Bad : public Character {
public:
	Bad(){}; //default constructor
	~Bad(){}; //default destructor

	void convert (int);
	void attack (Character*);
	void status() {cout << "ok" << endl;};
};

class Zombie: public Character {
public:
	Zombie(){}; //default constructor
	~Zombie(){}; //default destructor

	void convert (int);
	void attack (Character* p);
	void status() {cout << "ok" << endl;};
};

class World {
private:
	list<Person*> zombie_list; //list containing points to person-type objects

public:
	void create_list (int);
	void round();
	void current(); //displays the currently alive characters and their hit points
	void winner(int); //displays the winning team of the game
	bool game_over(int);
};

void Person :: hit (Character *c) {
	this->has_character->attack(c);
};

void Character :: lower_hp() {
	is_attached_to->hit_points = is_attached_to->hit_points - 10;
	if (is_attached_to->hit_points <= 0) { //checks if the character died after the last attack
		is_attached_to->is_dead = true;
	};
};

int Character :: get_character_type () { //returns 1 for Good, 2 for Bad and 3 for Zombie
	string type = typeid(*this).name();
	if (type == "4Good") {return 1;}
	else if (type == "3Bad") {return 2;}
	else if (type == "6Zombie") {return 3;}
	else {return 0; cout << 0 << endl;}
};

void Character :: died () {
	if (this->is_attached_to->is_dead) {
		delete is_attached_to; //deletes the person which the character is attached to
		delete this; //then it deletes the dead character
	};
};

void Good :: convert (int t) { //converts Good to Zombie
	Zombie *z;
	z = new Zombie;
	delete z->is_attached_to;
	z->is_attached_to = this->is_attached_to;
	this->is_attached_to->has_character = z;
	delete this;
};

void Bad :: convert (int t) { //converts Bad to Zombie
    Zombie *z;
    z = new Zombie;
    delete z->is_attached_to;
    z->is_attached_to = this->is_attached_to;
    this->is_attached_to->has_character = z;
    delete this;
};

void Zombie :: convert (int t) {//t == 1: converts Zombie to Good; t == 2: converts Zombie to Bad
	Character *c;
	switch (t) {
	case 1: {
		c = new Good;
		delete c->is_attached_to;
		c->is_attached_to = this->is_attached_to;
		this->is_attached_to->has_character = c;
		delete this;
		break; };
	case 2: {
		c = new Bad;
		delete c->is_attached_to;
		c->is_attached_to = this->is_attached_to;
		this->is_attached_to->has_character = c;
		delete this;
		break; };
	default:
		cout << "zombie conversion failed" << endl;
	};
}

void Good :: attack (Character* c){ //a Good attacks a Bad(2) or a Zombie(3)
	int x;
	switch (c->get_character_type()) {
	case 2: {
		c->lower_hp();
		break; };
	case 3: {
		x = rand() % 100;
		if (x < 8) c->convert(1);
		else if (x < 20) c->convert(2);
		else c->lower_hp();
		break; };
	};
};

void Bad :: attack (Character* c) { //a Bad objects attacks a Good(1) or a Zombie(3)
	switch (c->get_character_type()) {
	case 1: {
		c->lower_hp();
		break; };
	case 3: {
		c->lower_hp();
		break; };
	};
};

void Zombie :: attack (Character *c) { //a Zombie attacks a Good or a Bad
	int x;
    if (c->get_character_type() != 3) {
    	x = rand() % 100;
    	if (x < 10) c->convert(3);
        else c->lower_hp();
    };
};

void World :: create_list (int n) { //fills the list with pointers to person objects, which in turn are connected to characters
	int x, y, i;
	Character *c;
	for (i = 0; i < n; i++) {
		x = rand() % 100;
		y = x % 3;
		switch (y) { //randomly creates a Good, a Bad or a Zombie (33% possibility each)
		case 0:
			c = new Good;
			break;
		case 1:
			c = new Bad;
			break;
		case 2:
			c = new Zombie;
			break;
		default:
			cout << "error" << endl;
		};
		zombie_list.push_back(c->is_attached_to); //adds the newly created objects and the back of the list
	};
	cout << endl;
};

void World :: round () {
	list <Person*>::iterator i, k; //i = character that attacks k = character being attacked
    for(i = zombie_list.begin(); i != zombie_list.end(); ++i) {
    	if (i == prev(zombie_list.end())) {k = zombie_list.begin();} //list.end() shows the place after the last list objects, so list.prev is used
    	else {k = i; k++;};
    	(*i)->hit((*k)->has_character);

    	if ((*k)->is_dead) { //if a character died, it is removed from the list and the destroyed
        	zombie_list.remove(*k);
        	(*k)->has_character->died();
        };
    };
};

bool World :: game_over (int rounds) {
        if (rounds == 100) return true; //game ends if 100 rounds have elapsed
        for(list<Person*>::iterator it = zombie_list.begin(); it != zombie_list.end(); ++it) {
        	if (((*it)->has_character->get_character_type()) != ((*zombie_list.begin())->has_character->get_character_type())) return false;
        }; //returns false if there are more than one type of characters
        return true;
};

void World :: winner (int rounds) {
	if (rounds < 100) {
		switch ((*zombie_list.begin())->has_character->get_character_type()) {
		case 1: {
			cout << "The Goods win!" << endl;
			break; }
		case 2: {
			cout << "The Bads win!" << endl;
			break; }
		case 3: {
			cout << "The Zombies win!" << endl;
			break; }
		};
	}
	else cout << "100 rounds were completed without winner" << endl;
};

void World :: current () {
	for(list<Person*>::iterator it = zombie_list.begin(); it != zombie_list.end(); ++it) {
		switch((*it)->has_character->get_character_type()) {
		case 1: {
			cout << "Good-" << (*it)->get_hp() << " ";
			break; }
		case 2: {
			cout << "Bad-" << (*it)->get_hp() << " ";
			break; }
		case 3: {
			cout << "Zombie-" << (*it)->get_hp() << " ";
			break; }
		default:
			cout << "Unknown character ";
		};
	};
	cout << endl;
};

int main() {
	int players;
    int rounds = 0;
    World zombieland;
    cin >> players;
    srand (time(NULL));
    zombieland.create_list(players);
    zombieland.current();
    while (!zombieland.game_over(rounds)) {
    	zombieland.round();
    	zombieland.current();
    	rounds++;
    };
    zombieland.winner(rounds);
    return 0;
}
