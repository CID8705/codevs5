#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
const int INF = 876765346;
const char CELL_EMPTY = '_';
const char CELL_WALL = 'W';
const char CELL_OBJECT = 'O';
const std::array<int, 5> dx = { 0, -1, 1, 0, 0 };
const std::array<int, 5> dy = { -1, 0, 0, 1, 0 };
const std::array<std::string, 5> ds = { "U", "L", "R", "D", "" };
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
		return kind == CELL_OBJECT || kind == 'H';
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
				this->field[y + dy[dir]][x + dx[dir]].kind = 'H';
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
		std::vector<std::vector<int>> closed(this->H, std::vector<int>(this->W, -1));
		closed[y][x] = 0;
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
				if (!sc.field[ny][nx].isEmpty() || closed[ny][nx] > -1) {
					continue;
				}
				closed[ny][nx] = sc.dist + 1;
				open.push(Search(sc.field, nx, ny, sc.dist + 1));
			}
		}
		if (index < temp.size()) {
			std::sort(temp.begin() + index, temp.end(), [](const Character &c1, const Character &c2)->bool { return c1.id < c2.id; });
			index = temp.size();
			++dist;
		}
		for (int i = 0; i < temp.size(); ++i) {
			for (int dir = 0; dir < 4; ++dir) {
				if (closed[temp[i].y + dy[dir]][temp[i].x + dx[dir]] < closed[temp[i].y][temp[i].x]) {
					if (this->field[temp[i].y + dy[dir]][temp[i].x + dx[dir]].isEmpty() & !this->field[temp[i].y + dy[dir]][temp[i].x + dx[dir]].containsDog) {
						std::vector<Character>::iterator itr = std::find(this->dogs.begin(), this->dogs.end(), temp[i]);
						this->field[temp[i].y][temp[i].x].containsDog = false;
						this->field[temp[i].y + dy[dir]][temp[i].x + dx[dir]].containsDog = true;
						itr->x = temp[i].x + dx[dir];
						itr->y = temp[i].y + dy[dir];
						break;
					}
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
				this->field[y + dy[dir]][x + dx[dir]].kind = 'H';
				return true;
			}
		}
		return false;
	}
	int countDog(const int &id, const int &dist) const {
		int count = 0;
		std::queue<Search> open;
		std::vector<std::vector<bool>> closed(this->H, std::vector<bool>(this->W, false));
		closed[this->ninjas[id].y][this->ninjas[id].x] = true;
		open.push(Search(this->field, this->ninjas[id].x, this->ninjas[id].y, 0));
		while (!open.empty()) {
			Search sc = open.front();
			open.pop();
			if (this->field[sc.y][sc.x].containsDog) {
				++count;
			}
			for (int dir = 0; dir < 4; ++dir) {
				int nx = sc.x + dx[dir];
				int ny = sc.y + dy[dir];
				if (!sc.field[ny][nx].isEmpty() | closed[ny][nx] || sc.dist >= dist) {
					continue;
				}
				closed[ny][nx] = true;
				open.push(Search(sc.field, nx, ny, sc.dist + 1));
			}
		}
		return count;
	}
	int killDog(const int &id) {
		int count = 0;
		for (int j = this->ninjas[id].y - 1 > 0 ? this->ninjas[id].y - 1 : 0; j < (this->ninjas[id].y + 2 < this->H ? this->ninjas[id].y + 2 : this->H); ++j) {
			for (int i = this->ninjas[id].x - 1 > 0 ? this->ninjas[id].x - 1 : 0; i < (this->ninjas[id].x + 2 < this->W ? this->ninjas[id].x + 2 : this->W); ++i) {
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
	int dogDist, soulDist, getSoul;
	std::string skillBuff;
	MinDist(int dogDist, int soulDist, int getSoul, std::string skillBuff) : dogDist(dogDist), soulDist(soulDist), getSoul(getSoul), skillBuff(skillBuff){}
};
int remTime;
std::vector<Skill> skills;
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
int simulateWalk(State &st, const int &id, const int &dir) {
	st.field[st.ninjas[id].y][st.ninjas[id].x].containsNinja = false;
	st.ninjas[id].x += dx[dir];
	st.ninjas[id].y += dy[dir];
	st.field[st.ninjas[id].y][st.ninjas[id].x].containsNinja = true;
	if (st.field[st.ninjas[id].y][st.ninjas[id].x].containsSoul) {
		st.skillPoint += 2;
		st.field[st.ninjas[id].y][st.ninjas[id].x].containsSoul = false;
		//st.souls.erase(std::find(st.souls.begin(), st.souls.end(), Point(nx, ny)));
		return 1;
	}
	return 0;
}
MinDist getMinDist(State st, const int &id, const int &dir, std::string skillBuff) {
	int getSoul = 0;
	getSoul += simulateWalk(st, id, dir / 5);
	if (!st.field[st.ninjas[id].y][st.ninjas[id].x].isEmpty() & !st.isMoveObject(st.ninjas[id].x, st.ninjas[id].y, dir / 5)) {
		if (st.field[st.ninjas[id].y][st.ninjas[id].x].kind == CELL_OBJECT & skillBuff.empty() && st.skillPoint >= skills[3].cost) {
			skillBuff = std::to_string(skills[3].id) + " " + std::to_string(st.ninjas[id].y) + " " + std::to_string(st.ninjas[id].x);
			st.field[st.ninjas[id].y][st.ninjas[id].x].kind = CELL_EMPTY;
		}
		else {
			return MinDist(-INF, -INF, 0, skillBuff);
		}
	}
	getSoul += simulateWalk(st, id, dir % 5);
	if (!st.field[st.ninjas[id].y][st.ninjas[id].x].isEmpty() & !st.isMoveObject(st.ninjas[id].x, st.ninjas[id].y, dir % 5)) {
		return MinDist(-INF, -INF, 0, skillBuff);
	}
	if (!skillBuff.empty()) {
		std::istringstream iss(skillBuff);
		std::string str;
		iss >> str;
		if (str == std::to_string(skills[5].id)) {
			int i;
			iss >> str;
			i = std::stoi(str);
			iss >> str;
			st.simulateDog(std::stoi(str), i);
		}
	}
	MinDist minDist(INF, INF, getSoul, skillBuff);
	std::queue<Search> open;
	std::vector<std::vector<bool>> closed(st.H, std::vector<bool>(st.W, false));
	closed[st.ninjas[id].y][st.ninjas[id].x] = true;
	open.push(Search(st.field, st.ninjas[id].x, st.ninjas[id].y, 0));
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
		for (int i = 0; i < 4; ++i) {
			Search temp = sc;
			int nx = temp.x + dx[i];
			int ny = temp.y + dy[i];
			if (!temp.field[ny][nx].isEmpty() & !temp.isMoveObject(nx, ny, i) | closed[ny][nx]) {
				continue;
			}
			closed[ny][nx] = true;
			open.push(Search(temp.field, nx, ny, temp.dist + 1));
		}
	}
	return minDist;
}
std::string thinkByNinjaId(State &myState, const State &rivalState, const int &id, std::string &skillBuff) {
	std::vector<MinDist> dists;
	//std::vector<Point> points;
	for (int dir = 0; dir < 25; ++dir) {
		State st = myState;
		std::string str = skillBuff;
		if (st.field[st.ninjas[id].y + dy[dir / 5]][st.ninjas[id].x + dx[dir / 5]].containsSoul & st.field[st.ninjas[id].y + dy[dir / 5]][st.ninjas[id].x + dx[dir / 5]].kind == CELL_OBJECT & skillBuff.empty() && st.skillPoint >= skills[3].cost && rivalState.skillPoint >= skills[2].cost/* && myState.countDog(id, 2) > 0*/) {
			str = std::to_string(skills[3].id) + " " + std::to_string(st.ninjas[id].y + dy[dir / 5]) + " " + std::to_string(st.ninjas[id].x + dx[dir / 5]);
			st.field[st.ninjas[id].y + dy[dir / 5]][st.ninjas[id].x + dx[dir / 5]].kind = CELL_EMPTY;
		}
		dists.push_back(getMinDist(st, id, dir, str));
		//points.push_back(Point(myState.ninjas[id].x + dx[dir / 5] + dx[dir % 5], myState.ninjas[id].y + dy[dir / 5] + dy[dir % 5]));
	}
	int minDistSoul = INF;
	int maxDistDog = -INF;
	int bestDir = 24;
	std::vector<int> allDirs;
	std::vector<int> safeDirs;
	int getSoul = 0;
	bool withoutSkill = false;
	for (int dir = 0; dir < 25; ++dir) {
		if (dists[dir].soulDist == -INF) {
			continue;
		}
		if (dists[dir].dogDist < 1) {
			continue;
		}
		if (dists[dir].skillBuff.empty() || skillBuff == std::to_string(skills[0].id)) {
			allDirs.push_back(dir);
		}
		if (dists[dir].dogDist == 1) {
			if (!dists[dir].skillBuff.empty() && (dx[dir / 5] + dx[dir % 5] != 0 || dy[dir / 5] + dy[dir % 5] != 0)) {
				std::istringstream iss(dists[dir].skillBuff);
				std::string str;
				iss >> str;
				if (str != std::to_string(skills[5].id)) {
					continue;
				}
			}
			else {
				continue;
			}
		}
		if (dists[dir].skillBuff.empty()) {
			safeDirs.push_back(dir);
		}
		if (dists[dir].getSoul > getSoul) {
			minDistSoul = dists[dir].soulDist;
			maxDistDog = dists[dir].dogDist;
			bestDir = dir;
			getSoul = dists[dir].getSoul;
			withoutSkill = dists[dir].skillBuff.empty();
		}
		else if (dists[dir].getSoul == getSoul) {
			if (dists[dir].soulDist < minDistSoul) {
				minDistSoul = dists[dir].soulDist;
				maxDistDog = dists[dir].dogDist;
				bestDir = dir;
				getSoul = dists[dir].getSoul;
				withoutSkill = dists[dir].skillBuff.empty();
			}
			else if (dists[dir].soulDist == minDistSoul) {
				if (!withoutSkill & dists[dir].skillBuff.empty()) {
					minDistSoul = dists[dir].soulDist;
					maxDistDog = dists[dir].dogDist;
					bestDir = dir;
					getSoul = dists[dir].getSoul;
					withoutSkill = dists[dir].skillBuff.empty();
				}
				else if (withoutSkill & dists[dir].skillBuff.empty() | !withoutSkill & !dists[dir].skillBuff.empty()) {
					if (dists[dir].dogDist > maxDistDog) {
						minDistSoul = dists[dir].soulDist;
						maxDistDog = dists[dir].dogDist;
						bestDir = dir;
						getSoul = dists[dir].getSoul;
						withoutSkill = dists[dir].skillBuff.empty();
					}
				}
			}
		}
	}
	bool dangerous = minDistSoul == INF && maxDistDog == -INF && bestDir == 24 && myState.countDog(id, 1) > 0 ? true : false;
	if (skillBuff.empty() & !dists[bestDir].skillBuff.empty()) {
		skillBuff = dists[bestDir].skillBuff;
	}
	if (!safeDirs.empty()) {
		if (myState.skillPoint >= skills[3].cost && skillBuff.empty() && rivalState.skillPoint >= skills[2].cost) {
			if (myState.countDog(id, 2) > 0) {
				if (safeDirs.size() > 1) {
					if (std::find_if(safeDirs.begin() + 1, safeDirs.end(), [&](const int &i)->bool { return i / 5 != safeDirs.front() / 5 && i / 5 != 4; }) == safeDirs.end()) {
						if (myState.field[myState.ninjas[id].y + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5]].kind == CELL_OBJECT) {
							skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[bestDir / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[bestDir / 5]);
							myState.field[myState.ninjas[id].y + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5]].kind = CELL_EMPTY;
						}
						else if (myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5] >= 0 && myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5] >= 0 && myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5] < myState.W && myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5] < myState.H) {
							if (myState.field[myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5]].kind == CELL_OBJECT) {
								skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5]);
								myState.field[myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5]].kind = CELL_EMPTY;
							}
							else if (myState.field[myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5]].isWall() && myState.countDog(id, 1) > 0) {
								skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[bestDir / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[bestDir / 5]);
								myState.field[myState.ninjas[id].y + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5]].kind = CELL_EMPTY;
							}
						}
					}
				}
				else {
					if (myState.field[myState.ninjas[id].y + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5]].kind == CELL_OBJECT) {
						skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[bestDir / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[bestDir / 5]);
						myState.field[myState.ninjas[id].y + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5]].kind = CELL_EMPTY;
					}
					else if (myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5] >= 0 && myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5] >= 0 && myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5] < myState.W && myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5] < myState.H) {
						if (myState.field[myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5]].kind == CELL_OBJECT) {
							skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[bestDir / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[bestDir / 5]);
							myState.field[myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5]].kind = CELL_EMPTY;
						}
						else if (myState.field[myState.ninjas[id].y + dy[bestDir / 5] + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5] + dx[bestDir / 5]].isWall() && myState.countDog(id, 1) > 0) {
							skillBuff = std::to_string(skills[3].id) + " " + std::to_string(myState.ninjas[id].y + dy[bestDir / 5]) + " " + std::to_string(myState.ninjas[id].x + dx[bestDir / 5]);
							myState.field[myState.ninjas[id].y + dy[bestDir / 5]][myState.ninjas[id].x + dx[bestDir / 5]].kind = CELL_EMPTY;
						}
					}
				}
			}
		}
	}
	if (skillBuff == std::to_string(skills[0].id)) {
		dists.clear();
		for (int i = 0; i < allDirs.size(); ++i) {
			State st = myState;
			getSoul = 0;
			st.isMoveObject(st.ninjas[id].x + dx[allDirs[i] / 5], st.ninjas[id].y + dy[allDirs[i] / 5], allDirs[i] / 5);
			getSoul += simulateWalk(st, id, allDirs[i] / 5);
			st.isMoveObject(st.ninjas[id].x + dx[allDirs[i] % 5], st.ninjas[id].y + dy[allDirs[i] % 5], allDirs[i] % 5);
			getSoul += simulateWalk(st, id, allDirs[i] % 5);
			for (int dir = 0; dir < 4; ++dir) {
				dists.push_back(getMinDist(st, id, 20 + dir, skillBuff));
				dists.back().getSoul += getSoul;
			}
		}
		minDistSoul = INF;
		maxDistDog = -INF;
		bestDir = 4;
		getSoul = 0;
		withoutSkill = false;
		for (int i = 0; i < dists.size(); ++i) {
			if (dists[i].soulDist == -INF) {
				continue;
			}
			if (dists[i].dogDist <= 1) {
				continue;
			}
			if (dists[i].getSoul > getSoul) {
				minDistSoul = dists[i].soulDist;
				maxDistDog = dists[i].dogDist;
				bestDir = i;
				getSoul = dists[i].getSoul;
				withoutSkill = dists[i].skillBuff.empty();
			}
			else if (dists[i].getSoul == getSoul) {
				if (dists[i].soulDist < minDistSoul) {
					minDistSoul = dists[i].soulDist;
					maxDistDog = dists[i].dogDist;
					bestDir = i;
					getSoul = dists[i].getSoul;
					withoutSkill = dists[i].skillBuff.empty();
				}
				else if (minDistSoul == dists[i].soulDist) {
					if (dists[i].dogDist > maxDistDog) {
						minDistSoul = dists[i].soulDist;
						maxDistDog = dists[i].dogDist;
						bestDir = i;
						getSoul = dists[i].getSoul;
						withoutSkill = dists[i].skillBuff.empty();
					}
				}
			}
		}
		if (minDistSoul != INF || maxDistDog != -INF || bestDir != 4) {
			//myState.isMoveObject(myState.ninjas[id].x + dx[allDistDogDirs[nextDir / 4] / 5], myState.ninjas[id].y + dy[allDistDogDirs[nextDir / 4] / 5], allDistDogDirs[nextDir / 4] / 5);
			//simulateWalk(myState, id, allDistDogDirs[nextDir / 4] / 5);
			//myState.isMoveObject(myState.ninjas[id].x + dx[allDistDogDirs[nextDir / 4] % 5], myState.ninjas[id].y + dy[allDistDogDirs[nextDir / 4] % 5], allDistDogDirs[nextDir / 4] % 5);
			//simulateWalk(myState, id, allDistDogDirs[nextDir / 4] % 5);
			//myState.isMoveObject(myState.ninjas[id].x + dx[nextDir % 4], myState.ninjas[id].y + dy[nextDir % 4], nextDir % 4);
			//simulateWalk(myState, id, nextDir % 4);
			return ds[allDirs[bestDir / 4] / 5] + ds[allDirs[bestDir / 4] % 5] + ds[bestDir % 4];
		}
	}
	if (dangerous & skillBuff.empty()) {
		std::vector<Skill> vec = { skills[0], skills[5], skills[7] };
		std::sort(vec.begin(), vec.end(), [](const Skill &s1, const Skill &s2)->bool { return s1.cost < s2.cost; });
		for (int i = 0; i < vec.size(); ++i) {
			switch (vec[i].id) {
			case 0:
				if ((myState.skillPoint >= skills[0].cost || skillBuff == std::to_string(skills[0].id)) && !allDirs.empty()) {
					dists.clear();
					getSoul = 0;
					for (int j = 0; j < allDirs.size(); ++j) {
						State st(myState);
						st.isMoveObject(st.ninjas[id].x + dx[allDirs[j] / 5], st.ninjas[id].y + dy[allDirs[j] / 5], allDirs[j] / 5);
						getSoul += simulateWalk(st, id, allDirs[j] / 5);
						st.isMoveObject(st.ninjas[id].x + dx[allDirs[j] % 5], st.ninjas[id].y + dy[allDirs[j] % 5], allDirs[j] % 5);
						getSoul += simulateWalk(st, id, allDirs[j] % 5);
						for (int dir = 0; dir < 4; ++dir) {
							dists.push_back(getMinDist(st, id, 20 + dir, skillBuff));
							dists.back().getSoul += getSoul;
						}
					}
					minDistSoul = INF;
					maxDistDog = -INF;
					bestDir = 4;
					getSoul = 0;
					withoutSkill = false;
					for (int j = 0; j < dists.size(); ++j) {
						if (dists[j].soulDist == -INF) {
							continue;
						}
						if (dists[j].dogDist <= 1) {
							continue;
						}
						if (dists[j].getSoul > getSoul) {
							minDistSoul = dists[j].soulDist;
							maxDistDog = dists[j].dogDist;
							bestDir = j;
							getSoul = dists[j].getSoul;
							withoutSkill = dists[j].skillBuff.empty();
						}
						else if (dists[j].getSoul == getSoul) {
							if (dists[j].soulDist < minDistSoul) {
								minDistSoul = dists[j].soulDist;
								maxDistDog = dists[j].dogDist;
								bestDir = j;
								getSoul = dists[j].getSoul;
								withoutSkill = dists[j].skillBuff.empty();
							}
							else if (minDistSoul == dists[j].soulDist) {
								if (dists[j].dogDist > maxDistDog) {
									minDistSoul = dists[j].soulDist;
									maxDistDog = dists[j].dogDist;
									bestDir = j;
									getSoul = dists[j].getSoul;
									withoutSkill = dists[j].skillBuff.empty();
								}
							}
						}
					}
					if (minDistSoul != INF || maxDistDog != -INF || bestDir != 4) {
						skillBuff = std::to_string(skills[0].id);
						//myState.isMoveObject(myState.ninjas[id].x + dx[allDirs[nextDir / 4] / 5], myState.ninjas[id].y + dy[allDirs[nextDir / 4] / 5], allDirs[nextDir / 4] / 5);
						//simulateWalk(myState, id, allDirs[nextDir / 4] / 5);
						//myState.isMoveObject(myState.ninjas[id].x + dx[allDirs[nextDir / 4] % 5], myState.ninjas[id].y + dy[allDirs[nextDir / 4] % 5], allDirs[nextDir / 4] % 5);
						//simulateWalk(myState, id, allDirs[nextDir / 4] % 5);
						//myState.isMoveObject(myState.ninjas[id].x + dx[nextDir % 4], myState.ninjas[id].y + dy[nextDir % 4], nextDir % 4);
						//simulateWalk(myState, id, nextDir % 4);
						return ds[allDirs[bestDir / 4] / 5] + ds[allDirs[bestDir / 4] % 5] + ds[bestDir % 4];
					}
				}
				break;
			case 5:
				if (myState.skillPoint >= skills[5].cost) {
					State st(myState);
					std::string str;
					skillBuff = std::to_string(skills[5].id) + " " + std::to_string(st.ninjas[id].y) + " " + std::to_string(st.ninjas[id].x);
					if (!(str = thinkByNinjaId(st, rivalState, id, skillBuff)).empty()) {
						myState = st;
						return str;
					}
					else {
						st = myState;
						std::vector<std::vector<int>> dist(st.H, std::vector<int>(st.W, 0));
						for (int j = 0; j < st.ninjas.size(); ++j) {
							std::queue<Search> open;
							std::vector<std::vector<int>> closed(st.H, std::vector<int>(st.W, -1));
							closed[st.ninjas[j].y][st.ninjas[j].x] = 0;
							open.push(Search(st.field, st.ninjas[j].x, st.ninjas[j].y, 0));
							while (!open.empty()) {
								Search sc = open.front();
								open.pop();
								for (int dir = 0; dir < 4; ++dir) {
									int nx = sc.x + dx[dir];
									int ny = sc.y + dy[dir];
									if (!sc.field[ny][nx].isEmpty() || closed[ny][nx] > -1) {
										continue;
									}
									closed[ny][nx] = sc.dist + 1;
									open.push(Search(sc.field, nx, ny, sc.dist + 1));
								}
							}
							for (int l = 0; l < st.H; ++l) {
								for (int k = 0; k < st.W; ++k) {
									dist[l][k] += closed[l][k];
								}
							}
						}
						std::vector<int>::iterator maxItr = std::max_element(dist[0].begin(), dist[0].end());
						int x = std::distance(dist[0].begin(), maxItr), y = 0;
						for (int j = 1; j < dist.size(); ++j) {
							std::vector<int>::iterator tempItr;
							if (*(tempItr = std::max_element(dist[j].begin(), dist[j].end())) > *maxItr) {
								maxItr = tempItr;
								x = std::distance(dist[j].begin(), maxItr);
								y = j;
							}
						}
						/*int x, y, maxDist = -1;
						for (int k = 0; k < closed.size(); ++k) {
						for (int j = 0; j < closed[0].size(); ++j) {
						if (closed[k][j] > maxDist) {
						maxDist = closed[k][j];
						x = j;
						y = k;
						}
						}
						}*/
						skillBuff = std::to_string(skills[5].id) + " " + std::to_string(y) + " " + std::to_string(x);
						if (!(str = thinkByNinjaId(st, rivalState, id, skillBuff)).empty() || i == vec.size() - 1) {
							myState = st;
							return str;
						}
						else {
							if (i < vec.size() - 1) {
								vec.push_back(skills[5]);
							}
							skillBuff.clear();
						}
					}
				}
				break;
			case 7:
				if (myState.skillPoint >= skills[7].cost) {
					State st(myState);
					std::string str;
					skillBuff = std::to_string(skills[7].id) + " " + std::to_string(st.ninjas[id].id);
					st.killDog(id);
					if (!(str = thinkByNinjaId(st, rivalState, id, skillBuff)).empty()) {
						myState = st;
						return str;
					}
					else {
						skillBuff.clear();
					}
				}
				break;
			}
		}
	}
	if (!skillBuff.empty()) {
		std::istringstream iss(skillBuff);
		std::string str;
		iss >> str;
		if (str == std::to_string(skills[3].id)) {
			int i;
			iss >> str;
			i = std::stoi(str);
			iss >> str;
			myState.field[i][std::stoi(str)].kind = CELL_EMPTY;
		}
	}
	//myState.isMoveObject(myState.ninjas[id].x + dx[bestDir / 5], myState.ninjas[id].y + dy[bestDir / 5], bestDir / 5);
	//simulateWalk(myState, id, bestDir / 5);
	//myState.isMoveObject(myState.ninjas[id].x + dx[bestDir % 5], myState.ninjas[id].y + dy[bestDir % 5], bestDir % 5);
	//simulateWalk(myState, id, bestDir % 5);
	return ds[bestDir / 5] + ds[bestDir % 5];
}
void think(State &myState, State &rivalState) {
	State st(myState);
	std::string mySkillBuff;
	std::deque<std::string> myRoutes;
	bool flag = false;
	for (int id = 0; id < rivalState.ninjas.size(); ++id) {
		if (!mySkillBuff.empty() || myState.skillPoint < skills[2].cost) {
			break;
		}
		if (!rivalState.isCheckmate(id, mySkillBuff)) {
			break;
		}
	}
	if (myState.countDog(myState.ninjas.back().id, 1) > 0 && myState.countDog(myState.ninjas.front().id, 1) == 0) {
		bool usedSkill = false;
		for (int id = myState.ninjas.size() - 1; id >= 0; --id) {
			myRoutes.push_front(thinkByNinjaId(st, rivalState, id, mySkillBuff));
			if (mySkillBuff == std::to_string(skills[0].id)) {
				if (!usedSkill && id > 0) {
					st = myState;
					myRoutes.clear();
					for (int i = myState.ninjas.size() - 1; i >= id; --i) {
						myRoutes.push_front(thinkByNinjaId(st, rivalState, id, mySkillBuff));
					}
				}
				usedSkill = true;
			}
		}
		for (int id = 0; id < myState.ninjas.size() && !flag; ++id) {
			for (int i = 0; i < myRoutes[id].size(); ++i) {
				std::array<std::string, 5>::const_iterator itr = std::find_if(ds.begin(), ds.end(), [&](const std::string &s)->bool { return s.front() == myRoutes[id][i]; });
				int dir = std::distance(ds.begin(), itr);
				if (!st.field[st.ninjas[id].y + dy[dir]][st.ninjas[id].x + dx[dir]].isEmpty() & !st.isMoveObject(st.ninjas[id].x + dx[dir], st.ninjas[id].y + dy[dir], dir)) {
					myRoutes.clear();
					mySkillBuff.clear();
					flag = true;
					break;
				}
				simulateWalk(st, id, dir);
			}
		}
	}
	else {
		flag = true;
	}
	if (flag) {
		st = myState;
		bool usedSkill = false;
		for (int id = 0; id < myState.ninjas.size(); ++id) {
			myRoutes.push_back(thinkByNinjaId(st, rivalState, id, mySkillBuff));
			if (mySkillBuff == std::to_string(skills[0].id)) {
				if (!usedSkill && id > 0) {
					st = myState;
					myRoutes.clear();
					for (int i = 0; i <= id; ++i) {
						myRoutes.push_back(thinkByNinjaId(st, rivalState, i, mySkillBuff));
						for (int j = 0; j < myRoutes[i].size() && i < id; ++j) {
							std::array<std::string, 5>::const_iterator itr = std::find_if(ds.begin(), ds.end(), [&](const std::string &s)->bool { return s.front() == myRoutes[i][j]; });
							int dir = std::distance(ds.begin(), itr);
							st.isMoveObject(st.ninjas[i].x + dx[dir], st.ninjas[i].y + dy[dir], dir);
							simulateWalk(st, i, dir);
						}
					}
				}
				usedSkill = true;
			}
			for (int i = 0; i < myRoutes[id].size(); ++i) {
				std::array<std::string, 5>::const_iterator itr = std::find_if(ds.begin(), ds.end(), [&](const std::string &s)->bool { return s.front() == myRoutes[id][i]; });
				int dir = std::distance(ds.begin(), itr);
				st.isMoveObject(st.ninjas[id].x + dx[dir], st.ninjas[id].y + dy[dir], dir);
				simulateWalk(st, id, dir);
			}
		}
	}
	myState = st;
	for (int id = 0; id < rivalState.ninjas.size(); ++id) {
		if (!mySkillBuff.empty() || myState.skillPoint < skills[2].cost || rivalState.countDog(id, 2) < 1) {
			break;
		}
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
bool input(State &myState, State &rivalState) {
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
	State myState;
	State rivalState;
	std::cout << "s8079" << std::endl;
	std::cout.flush();
	while (input(myState, rivalState)) {
#ifdef _DEBUG
		for (int j = 0; j < myState.H; ++j) {
			for (int i = 0; i < myState.W; ++i) {
				if (i == myState.ninjas[0].x && j == myState.ninjas[0].y) {
					std::cout << "A";
				}
				else if (i == myState.ninjas[1].x && j == myState.ninjas[1].y) {
					std::cout << "B";
				}
				else if (myState.field[j][i].containsSoul) {
					std::cout << "S";
				}
				else if (myState.field[j][i].containsDog) {
					std::cout << "D";
				}
				else {
					std::cout << myState.field[j][i].kind;
				}
			}
			std::cout << std::endl;
		}
#endif
		think(myState, rivalState);
		std::cout.flush();
	}
	return 0;
}