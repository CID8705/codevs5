#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
const int INF = 876765346;
const char CELL_EMPTY = '_';
const char CELL_WALL = 'W';
const char CELL_OBJECT = 'O';
const std::array<int, 5> dx = { 0, 1, 0, -1, 0 };
const std::array<int, 5> dy = { -1, 0, 1, 0, 0 };
const std::array<std::string, 5> ds = { "U", "R", "D", "L", "" };
class Point {
public:
	int x, y;
	Point() {
		x = y = -1;
	}
	Point(int x, int y) : x(x), y(y) {}
	bool operator== (const Point &p) const {
		return x == p.x && y == p.y;
	}
};
class Skill {
public:
	int id, cost;
	Skill() {
		id = cost = -1;
	}
	Skill(int id, int cost) : id(id), cost(cost) {}
	static Skill input(int id) {
		int cost;
		std::cin >> cost;
		return Skill(id, cost);
	}
};
class Cell : public Point {
public:
	char kind;
	bool containsNinja, containsSoul, containsDog;
	Cell() {
		kind = '?';
		containsNinja = containsSoul = containsDog = false;
	}
	Cell(int x, int y, char kind) : Point(x, y), kind(kind) {
		containsNinja = containsSoul = containsDog = false;
	}
	bool isWall() const {
		return kind == CELL_WALL;
	}
	bool isObject() const {
		return kind == CELL_OBJECT;
	}
	bool isEmpty() const {
		return kind == CELL_EMPTY;
	}
};
class Character : public Point {
public:
	int id;
	Character() {
		id = -1;
	}
	Character(int id, int x, int y) : id(id), Point(x, y) {}
	static Character input() {
		int id, x, y;
		std::cin >> id >> y >> x;
		return Character(id, x, y);
	}
};
class Search : public Point {
public:
	std::vector<std::vector<Cell>> field;
	int dist;
	Search() {}
	Search(std::vector<std::vector<Cell>> field, int x, int y, int dist) : field(field), Point(x, y), dist(dist) {}
	bool isMoveObject(const int &x, const int &y, const int &dir) {
		if (this->field[y][x].isObject()) {
			if (this->field[y + dy[dir]][x + dx[dir]].isEmpty() & !this->field[y + dy[dir]][x + dx[dir]].containsNinja & !this->field[y + dy[dir]][x + dx[dir]].containsDog) {
				this->field[y][x].kind = CELL_EMPTY;
				this->field[y + dy[dir]][x + dx[dir]].kind = CELL_OBJECT;
				return true;
			}
		}
		return false;
	}
};
class State {
public:
	int skillPoint;
	int H, W;
	std::vector<std::vector<Cell>> field;
	std::vector<Character> ninjas;
	std::vector<Character> dogs;
	//std::vector<Point> souls;
	std::vector<int> skillCount;
	State() {
		skillPoint = H = W = -1;
		field.clear();
		ninjas.clear();
		dogs.clear();
		//souls.clear();
		skillCount.clear();
	}
	static State input(int numOfSkills) {
		State st;
		std::cin >> st.skillPoint;
		std::cin >> st.H >> st.W;
		st.field.clear();
		for (int i = 0; i < st.H; ++i) {
			std::vector<Cell> line;
			std::string s;
			std::cin >> s;
			for (int j = 0; j < s.size(); ++j) {
				line.push_back(Cell(j, i, s[j]));
			}
			st.field.push_back(line);
		}
		int numOfNinjas;
		std::cin >> numOfNinjas;
		st.ninjas.clear();
		for (int i = 0; i < numOfNinjas; ++i) {
			Character ninja = Character::input();
			st.ninjas.push_back(ninja);
			st.field[ninja.y][ninja.x].containsNinja = true;
		}
		int numOfDogs;
		std::cin >> numOfDogs;
		st.dogs.clear();
		for (int i = 0; i < numOfDogs; ++i) {
			Character dog = Character::input();
			st.dogs.push_back(dog);
			st.field[dog.y][dog.x].containsDog = true;
		}
		int numOfSouls;
		std::cin >> numOfSouls;
		//st.souls = std::vector<Point>();
		for (int i = 0; i < numOfSouls; ++i) {
			int x, y;
			std::cin >> y >> x;
			//st.souls.push_back(Point(x, y));
			st.field[y][x].containsSoul = true;
		}
		st.skillCount = std::vector<int>();
		for (int i = 0; i < numOfSkills; ++i) {
			int count;
			std::cin >> count;
			st.skillCount.push_back(count);
		}
		return st;
	}
	void simulateDog(const int &x, const int &y) {
		int dist = 0, index = 0;
		std::vector<Character> temp;
		std::queue<Search> open;
		std::vector<std::vector<bool>> closed(this->H, std::vector<bool>(this->W, false));
		closed[y][x] = true;
		open.push(Search(this->field, x, y, 0));
		while (!open.empty()) {
			Search sc = open.front();
			open.pop();
			if (dist < sc.dist) {
				std::sort(temp.begin() + index, temp.end(), [](const Character &c1, const Character &c2)->bool { return c1.id < c2.id; });
				index = temp.size();
				++dist;
			}
			if (this->field[sc.y][sc.x].containsDog) {
				temp.push_back(*std::find_if(this->dogs.begin(), this->dogs.end(), [&](const Character &c)->bool { return c.x == sc.x && c.y == sc.y; }));
			}
			for (int dir = 0; dir < 4; ++dir) {
				int nx = sc.x + dx[dir];
				int ny = sc.y + dy[dir];
				if (!sc.field[ny][nx].isEmpty() | closed[ny][nx]) {
					continue;
				}
				closed[ny][nx] = true;
				open.push(Search(sc.field, nx, ny, sc.dist + 1));
			}
		}
		if (index < temp.size()) {
			std::sort(temp.begin() + index, temp.end(), [](const Character &c1, const Character &c2)->bool { return c1.id < c2.id; });
			index = temp.size();
			++dist;
		}
		for (int i = 0; i < dogs.size(); ++i) {
			if (temp[i].y > y) {
				if (this->field[temp[i].y - 1][temp[i].x].isEmpty() & !this->field[temp[i].y - 1][temp[i].x].containsDog) {
					this->field[temp[i].y][temp[i].x].containsDog = false;
					this->field[temp[i].y - 1][temp[i].x].containsDog = true;
					std::find(this->dogs.begin(), this->dogs.end(), temp[i])->y = temp[i].y - 1;
				}
			}
			else if (temp[i].x > x) {
				if (this->field[temp[i].y][temp[i].x - 1].isEmpty() & !this->field[temp[i].y][temp[i].x - 1].containsDog) {
					this->field[temp[i].y][temp[i].x].containsDog = false;
					this->field[temp[i].y][temp[i].x - 1].containsDog = true;
					std::find(this->dogs.begin(), this->dogs.end(), temp[i])->x = temp[i].x - 1;
				}
			}
			else if (temp[i].x < x) {
				if (this->field[temp[i].y][temp[i].x + 1].isEmpty() & !this->field[temp[i].y][temp[i].x + 1].containsDog) {
					this->field[temp[i].y][temp[i].x].containsDog = false;
					this->field[temp[i].y][temp[i].x + 1].containsDog = true;
					std::find(this->dogs.begin(), this->dogs.end(), temp[i])->x = temp[i].x + 1;
				}
			}
			else if (temp[i].y < y) {
				if (this->field[temp[i].y + 1][temp[i].x].isEmpty() & !this->field[temp[i].y + 1][temp[i].x].containsDog) {
					this->field[temp[i].y][temp[i].x].containsDog = false;
					this->field[temp[i].y + 1][temp[i].x].containsDog = true;
					std::find(this->dogs.begin(), this->dogs.end(), temp[i])->y = temp[i].y + 1;
				}
			}
		}
		return;
	}
	bool isCheckmate(const int &id, std::string &skillBuff);
	bool isMoveObject(const int &x, const int &y, const int &dir) {
		if (this->field[y][x].isObject()) {
			if (this->field[y + dy[dir]][x + dx[dir]].isEmpty() & !this->field[y + dy[dir]][x + dx[dir]].containsNinja & !this->field[y + dy[dir]][x + dx[dir]].containsDog) {
				this->field[y][x].kind = CELL_EMPTY;
				this->field[y + dy[dir]][x + dx[dir]].kind = CELL_OBJECT;
				return true;
			}
		}
		return false;
	}
	/*int countDog(const int &id, const int &dir) const {
		int count = 0;
		for (int j = this->ninjas[id].y + dy[dir] - 1 > 0 ? this->ninjas[id].y + dy[dir] - 1 : 0; j < (this->ninjas[id].y + dy[dir] + 2 < this->H ? this->ninjas[id].y + dy[dir] + 2 : this->H); ++j) {
			for (int i = this->ninjas[id].x + dx[dir] - 1 > 0 ? this->ninjas[id].x + dx[dir] - 1 : 0; i < (this->ninjas[id].x + dx[dir] + 2 < this->W ? this->ninjas[id].x + dx[dir] + 2 : this->W); ++i) {
				if (this->field[j][i].containsDog) {
					++count;
				}
			}
		}
		return count;
	}*/
	int killDog(const int &id, const int &dir) {
		int count = 0;
		for (int j = this->ninjas[id].y + dy[dir] - 1 > 0 ? this->ninjas[id].y + dy[dir] - 1 : 0; j < (this->ninjas[id].y + dy[dir] + 2 < this->H ? this->ninjas[id].y + dy[dir] + 2 : this->H); ++j) {
			for (int i = this->ninjas[id].x + dx[dir] - 1 > 0 ? this->ninjas[id].x + dx[dir] - 1 : 0; i < (this->ninjas[id].x + dx[dir] + 2 < this->W ? this->ninjas[id].x + dx[dir] + 2 : this->W); ++i) {
				if (this->field[j][i].containsDog) {
					this->field[j][i].containsDog = false;
					++count;
				}
			}
		}
		return count;
	}
};
class MinDist {
public:
	int dogDist, soulDist;
	bool getSoul;
	std::string skillBuff;
	MinDist(int dogDist, int soulDist, bool getSoul, std::string skillBuff) : dogDist(dogDist), soulDist(soulDist), getSoul(getSoul), skillBuff(skillBuff){}
};
int remTime;
std::vector<Skill> skills;
State myState;
State rivalState;
bool State::isCheckmate(const int &id, std::string &skillBuff) {
	if (!this->field[this->ninjas[id].y - 1][this->ninjas[id].x].containsDog & !this->field[this->ninjas[id].y][this->ninjas[id].x + 1].containsDog & !this->field[this->ninjas[id].y + 1][this->ninjas[id].x].containsDog & !this->field[this->ninjas[id].y][this->ninjas[id].x - 1].containsDog) {
		return false;
	}
	for (int dir = 0; dir < 4; ++dir) {
		if (this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].containsDog | this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].isWall()) {
			continue;
		}
		else if (this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].isObject() & !this->field[this->ninjas[id].y + dy[dir] + dy[dir]][this->ninjas[id].x + dx[dir] + dx[dir]].isEmpty()) {
			continue;
		}
		else {
			if (this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].isObject()) {
				if (skillBuff.empty() & !this->field[this->ninjas[id].y + dy[dir] + dy[dir]][this->ninjas[id].x + dx[dir] + dx[dir]].containsDog && !this->field[this->ninjas[id].y + dy[dir] + dy[dir]][this->ninjas[id].x + dx[dir] + dx[dir]].containsNinja && !this->field[this->ninjas[id].y + dy[dir] + dy[dir]][this->ninjas[id].x + dx[dir] + dx[dir]].containsSoul) {
					skillBuff = std::to_string(skills[2].id) + " " + std::to_string(this->ninjas[id].y + dy[dir] + dy[dir]) + " " + std::to_string(this->ninjas[id].x + dx[dir] + dx[dir]);
					this->field[this->ninjas[id].y + dy[dir] + dy[dir]][this->ninjas[id].x + dx[dir] + dx[dir]].kind = 'O';
				}
				else {
					skillBuff.clear();
					return false;
				}
			}
			else if (!this->field[this->ninjas[id].y + dy[dir] + dy[dir]][this->ninjas[id].x + dx[dir] + dx[dir]].isEmpty()) {
				if (skillBuff.empty() & this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].isEmpty() && !this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].containsDog && !this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].containsNinja && !this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].containsSoul) {
					skillBuff = std::to_string(skills[2].id) + " " + std::to_string(this->ninjas[id].y + dy[dir]) + " " + std::to_string(this->ninjas[id].x + dx[dir]);
					this->field[this->ninjas[id].y + dy[dir]][this->ninjas[id].x + dx[dir]].kind = 'O';
				}
				else {
					skillBuff.clear();
					return false;
				}
			}
			else {
				skillBuff.clear();
				return false;
				break;
			}
		}
	}
	return true;
}
bool simulateWalk(State &st, const int &id, const int &dir) {
	st.field[st.ninjas[id].y][st.ninjas[id].x].containsNinja = false;
	st.ninjas[id].x += dx[dir];
	st.ninjas[id].y += dy[dir];
	st.field[st.ninjas[id].y][st.ninjas[id].x].containsNinja = true;
	if (st.field[st.ninjas[id].y][st.ninjas[id].x].containsSoul) {
		st.skillPoint += 2;
		st.field[st.ninjas[id].y][st.ninjas[id].x].containsSoul = false;
		//st.souls.erase(std::find(st.souls.begin(), st.souls.end(), Point(nx, ny)));
		return true;
	}
	return false;
}
MinDist getMinDist(State st, const int &id, const int &dir, const bool &skill) {
	std::string skillBuff;
	int sx = st.ninjas[id].x + dx[dir / 5];
	int sy = st.ninjas[id].y + dy[dir / 5];
	if (st.field[sy][sx].containsSoul & st.field[sy][sx].isObject() & skill && myState.skillPoint >= skills[3].cost && rivalState.skillPoint >= skills[2].cost) {
		skillBuff = std::to_string(skills[3].id) + " " + std::to_string(sy) + " " + std::to_string(sx);
		st.field[sy][sx].kind = CELL_EMPTY;
	}
	if (!st.field[sy][sx].isEmpty() & !st.isMoveObject(sx, sy, dir / 5)) {
		if (st.field[sy][sx].isObject() & skillBuff.empty() & skill && myState.skillPoint >= skills[3].cost) {
			skillBuff = std::to_string(skills[3].id) + " " + std::to_string(sy) + " " + std::to_string(sx);
			st.field[sy][sx].kind = CELL_EMPTY;
		}
		else {
			return MinDist(-INF, -INF, false, skillBuff);
		}
	}
	bool getSoul = simulateWalk(st, id, dir / 5);
	sx += dx[dir % 5];
	sy += dy[dir % 5];
	if (!st.field[sy][sx].isEmpty() & !st.isMoveObject(sx, sy, dir % 5)) {
		return MinDist(-INF, -INF, false, skillBuff);
	}
	MinDist minDist(INF, INF, getSoul, skillBuff);
	std::queue<Search> open;
	std::vector<std::vector<bool>> closed(st.H, std::vector<bool>(st.W, false));
	closed[sy][sx] = true;
	open.push(Search(st.field, sx, sy, 0));
	while (!open.empty()) {
		Search sc = open.front();
		open.pop();
		if (sc.field[sc.y][sc.x].containsDog && minDist.dogDist == INF) {
			minDist.dogDist = sc.dist;
		}
		if (sc.field[sc.y][sc.x].containsSoul && minDist.soulDist == INF && !sc.field[sc.y - 1][sc.x].containsDog && !sc.field[sc.y][sc.x + 1].containsDog && !sc.field[sc.y + 1][sc.x].containsDog && !sc.field[sc.y][sc.x - 1].containsDog && !sc.field[sc.y][sc.x].containsDog) {
			minDist.soulDist = sc.dist;
		}
		if (minDist.dogDist != INF && minDist.soulDist != INF) {
			return minDist;
		}
		for (int dir = 0; dir < 4; ++dir) {
			Search temp = sc;
			int nx = temp.x + dx[dir];
			int ny = temp.y + dy[dir];
			if (!temp.field[ny][nx].isEmpty() & !temp.isMoveObject(nx, ny, dir)) {
				continue;
			}
			if (closed[ny][nx]) {
				continue;
			}
			closed[ny][nx] = true;
			open.push(Search(temp.field, nx, ny, temp.dist + 1));
		}
	}
	return minDist;
}
std::string thinkByNinjaId(const int &id, std::string &skillBuff) {
	std::vector<MinDist> dists;
	//std::vector<Point> points;
	for (int dir = 0; dir < 25; ++dir) {
		dists.push_back(getMinDist(myState, id, dir, skillBuff.empty()));
		//points.push_back(Point(myState.ninjas[id].x + dx[dir / 5] + dx[dir % 5], myState.ninjas[id].y + dy[dir / 5] + dy[dir % 5]));
	}
	int minDistSoul = INF;
	int minDistSoulDir = 24;
	int maxDistDog = -INF;
	int maxDistDogDir = 24;
	std::vector<int> nearDistDogDirs;
	std::vector<int> farDistDogDirs;
	bool soulGetSoul = false;
	bool dogGetSoul = false;
	bool soulWithoutSkill = false;
	bool dogWithoutSkill = false;
	for (int dir = 0; dir < 25; ++dir) {
		if (dists[dir].soulDist == -INF) {
			continue;
		}
		if (dists[dir].dogDist == 1 && dists[dir].skillBuff.empty()) {
			nearDistDogDirs.push_back(dir);
		}
		if (dists[dir].dogDist <= 1) {
			continue;
		}
		if (dists[dir].skillBuff.empty()) {
			farDistDogDirs.push_back(dir);
		}
		if (!soulGetSoul | dists[dir].getSoul) {
			if (minDistSoul == dists[dir].soulDist && !soulWithoutSkill & dists[dir].skillBuff.empty()) {
				if (!soulWithoutSkill | dists[dir].skillBuff.empty()) {
					minDistSoul = dists[dir].soulDist;
					minDistSoulDir = dir;
					if (dists[dir].getSoul) {
						soulGetSoul = true;
					}
					soulWithoutSkill = true;
				}
			}
			if (minDistSoul > dists[dir].soulDist || !soulGetSoul & dists[dir].getSoul) {
				if (!soulWithoutSkill | dists[dir].skillBuff.empty() || !soulGetSoul & dists[dir].getSoul) {
					minDistSoul = dists[dir].soulDist;
					minDistSoulDir = dir;
					if (dists[dir].getSoul) {
						soulGetSoul = true;
					}
					if (dists[dir].skillBuff.empty()) {
						soulWithoutSkill = true;
					}
				}
			}
		}
		if (!dogGetSoul | dists[dir].getSoul) {
			if (maxDistDog == dists[dir].dogDist && !dogWithoutSkill & dists[dir].skillBuff.empty()) {
				if (!dogWithoutSkill | dists[dir].skillBuff.empty()) {
					maxDistDog = dists[dir].dogDist;
					maxDistDogDir = dir;
					if (dists[dir].getSoul) {
						dogGetSoul = true;
					}
					if (dists[dir].skillBuff.empty()) {
						dogWithoutSkill = true;
					}
				}
			}
			if (maxDistDog < dists[dir].dogDist || !dogGetSoul & dists[dir].getSoul) {
				if (!dogWithoutSkill | dists[dir].skillBuff.empty() || !dogGetSoul & dists[dir].getSoul) {
					maxDistDog = dists[dir].dogDist;
					maxDistDogDir = dir;
					if (dists[dir].getSoul) {
						dogGetSoul = true;
					}
					if (dists[dir].skillBuff.empty()) {
						dogWithoutSkill = true;
					}
				}
			}
		}
	}
	bool dangerous = minDistSoul == INF && minDistSoulDir == 24 && maxDistDog == -INF && maxDistDogDir == 24 ? true : false;
	int dir = minDistSoulDir != INF && maxDistDog > 1 ? minDistSoulDir : maxDistDogDir;
	if (skillBuff.empty() & !dists[dir].skillBuff.empty()) {
		skillBuff = dists[dir].skillBuff;
	}
	if (!farDistDogDirs.empty()) {
		if (myState.skillPoint >= skills[3].cost && skillBuff.empty()) {
			if (farDistDogDirs.size() > 1) {
				if (std::find_if(farDistDogDirs.begin() + 1, farDistDogDirs.end(), [&](const int &i)->bool { return i / 5 != farDistDogDirs.front() / 5 && i / 5 != 4; }) == farDistDogDirs.end()) {
					if (myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5]].isObject()) {
						skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[farDistDogDirs.front() / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[farDistDogDirs.front() / 5]);
						myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5]].kind = CELL_EMPTY;
					}
					else if (myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5] >= 0 && myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5] >= 0 && myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5] < myState.W && myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5] < myState.H) {
						if (myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5]].isObject()) {
							skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5]);
							myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5]].kind = CELL_EMPTY;
						}
					}
				}
			}
			else {
				if (myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5]].isObject()) {
					skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[farDistDogDirs.front() / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[farDistDogDirs.front() / 5]);
					myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5]].kind = CELL_EMPTY;
				}
				else if (myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5] >= 0 && myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5] >= 0 && myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5] < myState.W && myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5] < myState.H) {
					if (myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5]].isObject()) {
						skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[farDistDogDirs.front() / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[farDistDogDirs.front() / 5]);
						myState.field[myState.ninjas[id].y + dy[farDistDogDirs.front() / 5] + dy[farDistDogDirs.front() / 5]][myState.ninjas[id].x + dx[farDistDogDirs.front() / 5] + dx[farDistDogDirs.front() / 5]].kind = CELL_EMPTY;
					}
				}
			}
		}
	}
	if ((myState.skillPoint >= skills[0].cost && skillBuff.empty() & dangerous || skillBuff == std::to_string(skills[0].id)) && !nearDistDogDirs.empty()) {
		dists.clear();
		soulGetSoul = false;
		dogGetSoul = false;
		soulWithoutSkill = false;
		dogWithoutSkill = false;
		for (int i = 0; i < nearDistDogDirs.size(); ++i) {
			State st = myState;
			if (!st.field[st.ninjas[id].y + dy[nearDistDogDirs[i] / 5]][st.ninjas[id].x + dx[nearDistDogDirs[i] / 5]].isEmpty() & !st.isMoveObject(st.ninjas[id].x + dx[nearDistDogDirs[i] / 5], st.ninjas[id].y + dy[nearDistDogDirs[i] / 5], nearDistDogDirs[i] / 5)) {
				if (st.field[st.ninjas[id].y + dy[nearDistDogDirs[i] / 5]][st.ninjas[id].x + dx[nearDistDogDirs[i] / 5]].isObject() && st.skillPoint >= skills[3].cost) {
					st.field[st.ninjas[id].y + dy[nearDistDogDirs[i] / 5]][st.ninjas[id].x + dx[nearDistDogDirs[i] / 5]].kind = CELL_EMPTY;
				}
			}
			simulateWalk(st, id, nearDistDogDirs[i] / 5);
			if (!st.field[st.ninjas[id].y + dy[nearDistDogDirs[i] % 5]][st.ninjas[id].x + dx[nearDistDogDirs[i] % 5]].isEmpty() & !st.isMoveObject(st.ninjas[id].x + dx[nearDistDogDirs[i] % 5], st.ninjas[id].y + dy[nearDistDogDirs[i] % 5], nearDistDogDirs[i] % 5)) {
				if (st.field[st.ninjas[id].y + dy[nearDistDogDirs[i] % 5]][st.ninjas[id].x + dx[nearDistDogDirs[i] % 5]].isObject() && st.skillPoint >= skills[3].cost) {
					st.field[st.ninjas[id].y + dy[nearDistDogDirs[i] % 5]][st.ninjas[id].x + dx[nearDistDogDirs[i] % 5]].kind = CELL_EMPTY;
				}
			}
			simulateWalk(st, id, nearDistDogDirs[i] % 5);
			for (int j = 0; j < 4; ++j) {
				dists.push_back(getMinDist(st, id, 20 + j, false));
			}
		}
		minDistSoul = INF;
		minDistSoulDir = 4;
		maxDistDog = -INF;
		maxDistDogDir = 4;
		for (int i = 0; i < dists.size(); ++i) {
			if (dists[i].soulDist == -INF) {
				continue;
			}
			if (dists[i].dogDist <= 1) {
				continue;
			}
			if (!soulGetSoul | dists[i].getSoul) {
				if (minDistSoul > dists[i].soulDist || !soulGetSoul & dists[i].getSoul) {
					if (!soulWithoutSkill | dists[i].skillBuff.empty()) {
						minDistSoul = dists[i].soulDist;
						minDistSoulDir = i;
						if (dists[i].getSoul) {
							soulGetSoul = true;
						}
						if (dists[i].skillBuff.empty()) {
							soulWithoutSkill = true;
						}
					}
				}
			}
			if (!dogGetSoul | dists[i].getSoul) {
				if (maxDistDog < dists[i].dogDist || !dogGetSoul & dists[i].getSoul) {
					if (!dogWithoutSkill | dists[i].skillBuff.empty()) {
						maxDistDog = dists[i].dogDist;
						maxDistDogDir = i;
						if (dists[i].getSoul) {
							dogGetSoul = true;
						}
						if (dists[i].skillBuff.empty()) {
							dogWithoutSkill = true;
						}
					}
				}
			}
		}
		int nextDir = minDistSoulDir != INF && maxDistDog > 1 ? minDistSoulDir : maxDistDogDir;
		if (minDistSoul != INF || minDistSoulDir != 4 || maxDistDog != -INF || maxDistDogDir != 4) {
			skillBuff = std::to_string(skills[0].id);
			if (!myState.field[myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] / 5]][myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] / 5]].isEmpty() & !myState.isMoveObject(myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] / 5], myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] / 5], nearDistDogDirs[nextDir / 4] / 5)) {
				if (myState.field[myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] / 5]][myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] / 5]].isObject() && myState.skillPoint >= skills[3].cost) {
					myState.field[myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] / 5]][myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] / 5]].kind = CELL_EMPTY;
				}
			}
			simulateWalk(myState, id, nearDistDogDirs[nextDir / 4] / 5);
			if (!myState.field[myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] % 5]][myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] % 5]].isEmpty() & !myState.isMoveObject(myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] % 5], myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] % 5], nearDistDogDirs[nextDir / 4] % 5)) {
				if (myState.field[myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] % 5]][myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] % 5]].isObject() && myState.skillPoint >= skills[3].cost) {
					myState.field[myState.ninjas[id].y + dy[nearDistDogDirs[nextDir / 4] % 5]][myState.ninjas[id].x + dx[nearDistDogDirs[nextDir / 4] % 5]].kind = CELL_EMPTY;
				}
			}
			simulateWalk(myState, id, nearDistDogDirs[nextDir / 4] % 5);
			if (!myState.field[myState.ninjas[id].y + dy[nextDir % 4]][myState.ninjas[id].x + dx[nextDir % 4]].isEmpty() & !myState.isMoveObject(myState.ninjas[id].x + dx[nextDir % 4], myState.ninjas[id].y + dy[nextDir % 4], nextDir % 4)) {
				if (myState.field[myState.ninjas[id].y + dy[nextDir % 4]][myState.ninjas[id].x + dx[nextDir % 4]].isObject() && myState.skillPoint >= skills[3].cost) {
					myState.field[myState.ninjas[id].y + dy[nextDir % 4]][myState.ninjas[id].x + dx[nextDir % 4]].kind = CELL_EMPTY;
				}
			}
			simulateWalk(myState, id, nextDir % 4);
			return ds[nearDistDogDirs[nextDir / 4] / 5] + ds[nearDistDogDirs[nextDir / 4] % 5] + ds[nextDir % 4];
		}
	}
	if (myState.skillPoint >= skills[7].cost && skillBuff.empty() & dangerous) {
		skillBuff = std::to_string(skills[7].id) + " " + std::to_string(id);
		myState.killDog(id, 4);
		return thinkByNinjaId(id, skillBuff);
	}
	if (myState.skillPoint >= skills[5].cost && skillBuff.empty() & dangerous) {
		int x, y;
		if (myState.ninjas[id].y < myState.H / 2) {
			if (myState.ninjas[id].x < myState.W / 2) {
				for (y = myState.H - 1; y >= myState.H / 2; --y) {
					for (x = myState.W - 1; x >= myState.W / 2; --x) {
						if (myState.field[y][x].isEmpty()) {
							skillBuff = std::to_string(skills[5].id) + " " + std::to_string(y) + " " + std::to_string(x);
							break;
						}
					}
					if (x >= myState.W / 2) {
						break;
					}
				}
			}
			else {
				for (y = myState.H - 1; y >= myState.H / 2; --y) {
					for (x = 0; x < myState.W / 2; ++x) {
						if (myState.field[y][x].isEmpty()) {
							skillBuff = std::to_string(skills[5].id) + " " + std::to_string(y) + " " + std::to_string(x);
							break;
						}
					}
					if (x < myState.W / 2) {
						break;
					}
				}
			}
		}
		else {
			if (myState.ninjas[id].x < myState.W / 2) {
				for (y = 0; y < myState.H / 2; ++y) {
					for (x = myState.W - 1; x >= myState.W / 2; --x) {
						if (myState.field[y][x].isEmpty()) {
							skillBuff = std::to_string(skills[5].id) + " " + std::to_string(y) + " " + std::to_string(x);
							break;
						}
					}
					if (x >= myState.W / 2) {
						break;
					}
				}
			}
			else {
				for (y = 0; y < myState.H / 2; ++y) {
					for (x = 0; x < myState.W / 2; ++x) {
						if (myState.field[y][x].isEmpty()) {
							skillBuff = std::to_string(skills[5].id) + " " + std::to_string(y) + " " + std::to_string(x);
							break;
						}
					}
					if (x < myState.W / 2) {
						break;
					}
				}
			}
		}
		if (myState.field[y][x].isEmpty()) {
			myState.simulateDog(x, y);
			return thinkByNinjaId(id, skillBuff);
		}
	}
	if (!myState.field[myState.ninjas[id].y + dy[dir / 5]][myState.ninjas[id].x + dx[dir / 5]].isEmpty() & !myState.isMoveObject(myState.ninjas[id].x + dx[dir / 5], myState.ninjas[id].y + dy[dir / 5], dir / 5)) {
		if (myState.field[myState.ninjas[id].y + dy[dir / 5]][myState.ninjas[id].x + dx[dir / 5]].isObject() && myState.skillPoint >= skills[3].cost) {
			myState.field[myState.ninjas[id].y + dy[dir / 5]][myState.ninjas[id].x + dx[dir / 5]].kind = CELL_EMPTY;
		}
	}
	simulateWalk(myState, id, dir / 5);
	if (!myState.field[myState.ninjas[id].y + dy[dir % 5]][myState.ninjas[id].x + dx[dir % 5]].isEmpty() & !myState.isMoveObject(myState.ninjas[id].x + dx[dir % 5], myState.ninjas[id].y + dy[dir % 5], dir % 5)) {
		if (myState.field[myState.ninjas[id].y + dy[dir % 5]][myState.ninjas[id].x + dx[dir % 5]].isObject() && myState.skillPoint >= skills[3].cost) {
			myState.field[myState.ninjas[id].y + dy[dir % 5]][myState.ninjas[id].x + dx[dir % 5]].kind = CELL_EMPTY;
		}
	}
	simulateWalk(myState, id, dir % 5);
	return ds[dir / 5] + ds[dir % 5];
}
void think() {
	std::string mySkillBuff;
	std::vector<std::string> myRoutes;
	for (int id = 0; id < rivalState.ninjas.size(); ++id) {
		if (!mySkillBuff.empty() || myState.skillPoint < skills[2].cost) {
			break;
		}
		if (!rivalState.isCheckmate(id, mySkillBuff)) {
			break;
		}
	}
	for (int i = 0; i < myState.ninjas.size(); ++i) {
		myRoutes.push_back(thinkByNinjaId(myState.ninjas[i].id, mySkillBuff));
	}
	for (int id = 0; id < rivalState.ninjas.size(); ++id) {
		if (!mySkillBuff.empty() || myState.skillPoint < skills[2].cost/* || getMinDist(rivalState, id, 24, false).dogDist > 2*/) {
			break;
		}
		/*for (int dir = 0; dir < 25; ++dir) {
			if (rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] >= 0 && rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] >= 0 && rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] < rivalState.W && rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] < rivalState.H) {
				if (rivalState.field[rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5]][rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5]].containsSoul && rivalState.field[rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5]][rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5]].isObject()) {
					if (rivalState.field[rivalState.ninjas[id].y + dy[dir / 5]][rivalState.ninjas[id].x + dx[dir / 5]].isEmpty() && !rivalState.field[rivalState.ninjas[id].y + dy[dir / 5]][rivalState.ninjas[id].x + dx[dir / 5]].containsDog && !rivalState.field[rivalState.ninjas[id].y + dy[dir / 5]][rivalState.ninjas[id].x + dx[dir / 5]].containsNinja && !rivalState.field[rivalState.ninjas[id].y + dy[dir / 5]][rivalState.ninjas[id].x + dx[dir / 5]].containsSoul) {
						mySkillBuff = std::to_string(skills[2].id) + " " + std::to_string(rivalState.ninjas[id].y + dy[dir / 5]) + " " + std::to_string(rivalState.ninjas[id].x + dx[dir / 5]);
						rivalState.field[rivalState.ninjas[id].y + dy[dir / 5]][rivalState.ninjas[id].x + dx[dir / 5]].kind = 'O';
						break;
					}
					if (rivalState.field[rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] + dy[dir % 5]][rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] + dx[dir % 5]].isEmpty() && !rivalState.field[rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] + dy[dir % 5]][rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] + dx[dir % 5]].containsDog && !rivalState.field[rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] + dy[dir % 5]][rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] + dx[dir % 5]].containsNinja && !rivalState.field[rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] + dy[dir % 5]][rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] + dx[dir % 5]].containsSoul) {
						mySkillBuff = std::to_string(skills[2].id) + " " + std::to_string(rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] + dy[dir % 5]) + " " + std::to_string(rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] + dx[dir % 5]);
						rivalState.field[rivalState.ninjas[id].y + dy[dir / 5] + dy[dir % 5] + dy[dir % 5]][rivalState.ninjas[id].x + dx[dir / 5] + dx[dir % 5] + dx[dir % 5]].kind = 'O';
						break;
					}
				}
			}
		}*/
		for (int dir = 0; dir < 4; ++dir) {
			if (rivalState.ninjas[id].x + dx[dir] + dx[dir] >= 0 && rivalState.ninjas[id].y + dy[dir] + dy[dir] >= 0 && rivalState.ninjas[id].x + dx[dir] + dx[dir] < rivalState.W && rivalState.ninjas[id].y + dy[dir] + dy[dir] < rivalState.H) {
				if (rivalState.field[rivalState.ninjas[id].y + dy[dir]][rivalState.ninjas[id].x + dx[dir]].containsSoul && rivalState.field[rivalState.ninjas[id].y + dy[dir]][rivalState.ninjas[id].x + dx[dir]].isObject()) {
					if (rivalState.field[rivalState.ninjas[id].y + dy[dir] + dy[dir]][rivalState.ninjas[id].x + dx[dir] + dx[dir]].isEmpty() && !rivalState.field[rivalState.ninjas[id].y + dy[dir] + dy[dir]][rivalState.ninjas[id].x + dx[dir] + dx[dir]].containsDog && !rivalState.field[rivalState.ninjas[id].y + dy[dir] + dy[dir]][rivalState.ninjas[id].x + dx[dir] + dx[dir]].containsNinja && !rivalState.field[rivalState.ninjas[id].y + dy[dir] + dy[dir]][rivalState.ninjas[id].x + dx[dir] + dx[dir]].containsSoul) {
						mySkillBuff = std::to_string(skills[2].id) + " " + std::to_string(rivalState.ninjas[id].y + dy[dir] + dy[dir]) + " " + std::to_string(rivalState.ninjas[id].x + dx[dir] + dx[dir]);
						rivalState.field[rivalState.ninjas[id].y + dy[dir] + dy[dir]][rivalState.ninjas[id].x + dx[dir] + dx[dir]].kind = 'O';
						break;
					}
				}
				if (rivalState.field[rivalState.ninjas[id].y + dy[dir] + dy[dir]][rivalState.ninjas[id].x + dx[dir] + dx[dir]].containsSoul && rivalState.field[rivalState.ninjas[id].y + dy[dir] + dy[dir]][rivalState.ninjas[id].x + dx[dir] + dx[dir]].isObject()) {
					if (rivalState.field[rivalState.ninjas[id].y + dy[dir]][rivalState.ninjas[id].x + dx[dir]].isEmpty() && !rivalState.field[rivalState.ninjas[id].y + dy[dir]][rivalState.ninjas[id].x + dx[dir]].containsDog && !rivalState.field[rivalState.ninjas[id].y + dy[dir]][rivalState.ninjas[id].x + dx[dir]].containsNinja && !rivalState.field[rivalState.ninjas[id].y + dy[dir]][rivalState.ninjas[id].x + dx[dir]].containsSoul) {
						mySkillBuff = std::to_string(skills[2].id) + " " + std::to_string(rivalState.ninjas[id].y + dy[dir]) + " " + std::to_string(rivalState.ninjas[id].x + dx[dir]);
						rivalState.field[rivalState.ninjas[id].y + dy[dir]][rivalState.ninjas[id].x + dx[dir]].kind = 'O';
						break;
					}
				}
			}
		}
	}
	if (mySkillBuff.empty()) {
		std::cout << 2 << std::endl;
	}
	else {
		std::cout << 3 << std::endl << mySkillBuff << std::endl;
	}
	for (int i = 0; i < myRoutes.size(); ++i) {
		std::cout << myRoutes[i] << std::endl;
	}
	return;
}
bool input() {
	if (!(std::cin >> remTime)) {
		return false;
	}
	int numOfSkills;
	std::cin >> numOfSkills;
	skills.clear();
	for (int i = 0; i < numOfSkills; ++i) {
		skills.push_back(Skill::input(i));
	}
	myState = State::input(skills.size());
	rivalState = State::input(skills.size());
	return true;
}
int main() {
	std::cout << "s8079" << std::endl;
	std::cout.flush();
	while (input()) {
#ifdef _DEBUG
		for (int j = 0; j < myState.H; ++j) {
			for (int i = 0; i < myState.W; ++i) {
				if (i == myState.ninjas[0].x && j == myState.ninjas[0].y) {
					std::cout << "A";
				}
				else if (i == myState.ninjas[1].x && j == myState.ninjas[1].y) {
					std::cout << "B";
				}
				else if (myState.field[j][i].containsDog) {
					std::cout << "D";
				}
				else if (myState.field[j][i].containsSoul) {
					std::cout << "S";
				}
				else {
					std::cout << myState.field[j][i].kind;
				}
			}
			std::cout << std::endl;
		}
#endif
		think();
		std::cout.flush();
	}
	return 0;
}