#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
const int INF = 876765346;
const char CELL_EMPTY = '_';
const char CELL_WALL = 'W';
const char CELL_OBJECT = 'O';
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
class State {
public:
	int skillPoint;
	int H, W;
	std::vector<std::vector<Cell>> field;
	std::vector<Character> ninjas;
	std::vector<Character> dogs;
	std::vector<Point> souls;
	std::vector<int> skillCount;
	State() {
		skillPoint = H = W = -1;
		field.clear();
		ninjas.clear();
		dogs.clear();
		souls.clear();
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
		st.souls = std::vector<Point>();
		for (int i = 0; i < numOfSouls; ++i) {
			int x, y;
			std::cin >> y >> x;
			st.souls.push_back(Point(x, y));
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
};
class Search : public Point {
public:
	int dist;
	Search() {}
	Search(int x, int y, int dist) : Point(x, y), dist(dist) {}
};
class MinDist {
public:
	int dogDist, soulDist;
	MinDist(int dogDist, int soulDist) : dogDist(dogDist), soulDist(soulDist) {}
};
int remTime;
std::vector<Skill> skills;
State myState;
State rivalState;
int dx[] = { 0, 1, 0, -1, 0 };
int dy[] = { -1, 0, 1, 0, 0 };
std::string ds[] = { "U", "R", "D", "L",  "" };
/**
* (sx, sy) を起点として、幅優先探索で (最も近い忍犬までの距離, 最も近いアイテムまでの距離) を計算して MinDist で返します。
* 忍犬やアイテムがフィールド上に存在しない場合は、対応する値を INF として返します。
* (sx, sy) が歩けない場所である場合、 MinDist(-INF, -INF) として返します。
*/
MinDist getMinDist(const State &st, int sx, int sy) {
	if (!st.field[sy][sx].isEmpty()) {
		return MinDist(-INF, -INF);
	}
	MinDist minDist(INF, INF);
	std::queue<Search> open;
	std::vector<std::vector<bool>> closed(st.H, std::vector<bool>(st.W, false));
	closed[sy][sx] = true;
	open.push(Search(sx, sy, 0));
	while (!open.empty()) {
		Search sc = open.front(); open.pop();
		if (st.field[sc.y][sc.x].containsDog && minDist.dogDist == INF) {
			minDist.dogDist = sc.dist;
		}
		if (st.field[sc.y][sc.x].containsSoul && minDist.soulDist == INF) {
			minDist.soulDist = sc.dist;
		}
		if (minDist.dogDist != INF && minDist.soulDist != INF) {
			return minDist;
		}
		for (int dir = 0; dir < 4; ++dir) {
			int nx = sc.x + dx[dir];
			int ny = sc.y + dy[dir];
			if (!st.field[ny][nx].isEmpty()) {
				continue;
			}
			if (closed[ny][nx]) {
				continue;
			}
			closed[ny][nx] = true;
			open.push(Search(nx, ny, sc.dist + 1));
		}
	}
	return minDist;
}
/*
* 引数
* - id: 忍者ID
* - dir: 忍者を歩かせる方向
*
* ID が id である忍者を dir の方向へ移動するシミュレートを行います。
* この関数で行われるシミュレート内容
* - 忍者の位置修正 (移動先が岩の場合は、位置修正を行わずにシミュレートを終了します)
* - 移動先にニンジャソウルが存在する場合、取得処理(忍力回復する, フィールドのソウルフラグをfalseにする, 取得済みのソウルの座標削除)が行われます。
* (※簡単なシミュレートのため、壁を押すなどの処理は行われません)
*/
void simulateWalk(int id, int dir) {
	int nx = myState.ninjas[id].x + dx[dir];
	int ny = myState.ninjas[id].y + dy[dir];
	if (!myState.field[ny][nx].isEmpty()) {
		return;
	}
	myState.ninjas[id].x = nx;
	myState.ninjas[id].y = ny;
	if (!myState.field[ny][nx].containsSoul) {
		return;
	}
	// 忍力回復
	myState.skillPoint += 2;
	// フィールドのフラグをfalseに
	myState.field[ny][nx].containsSoul = false;
	// 取得済みのソウルの座標削除
	myState.souls.erase(find(myState.souls.begin(), myState.souls.end(), Point(nx, ny)));
}
/*
* 移動方向の決め方
* - 忍者は、自分自身から最も近いニンジャソウルへ向かって移動します。
* - 壁を押さずに移動します。
* - 忍犬までの最短距離が1以下になるようなマスへは移動しません。
* - 自分自身のマスから連結であるマスの中にニンジャソウルが存在しない場合は、忍犬までの最短距離が最大になるように移動します。
* -- 忍犬も存在しない場合は、その場にとどまります。
*/
std::string thinkByNinjaId(int id) {
	std::vector<MinDist> dists;
	std::vector<Point> points;
	for (int dir = 0; dir < 5; ++dir) {
		int nx = myState.ninjas[id].x + dx[dir];
		int ny = myState.ninjas[id].y + dy[dir];
		dists.push_back(getMinDist(myState, nx, ny));
		points.push_back(Point(nx, ny));
	}
	// ニンジャソウルへ近づく方向
	int minDistSoul = INF;
	int minDistSoulDir = 4;
	// 忍犬から遠ざかる方向
	int maxDistDog = -INF;
	int maxDistDogDir = 4;
	for (int dir = 0; dir < 5; ++dir) {
		// この方向には歩くことができないので無視
		if (dists[dir].soulDist == -INF) {
			continue;
		}
		// 忍犬までの距離が1以下になってしまうので無視
		if (dists[dir].dogDist <= 1) {
			continue;
		}
		if (minDistSoul > dists[dir].soulDist) {
			minDistSoul = dists[dir].soulDist;
			minDistSoulDir = dir;
		}
		if (maxDistDog < dists[dir].dogDist) {
			maxDistDog = dists[dir].dogDist;
			maxDistDogDir = dir;
		}
	}
	int dir = 4;
	if (minDistSoul != INF) {
		dir = minDistSoulDir;
	}
	else {
		dir = maxDistDogDir;
	}
	if (dir == 4) {
		return ds[dir];
	}
	simulateWalk(id, dir);
	return ds[dir];
}
/*
* このAIについて
* - 各忍者について、 thinkByNinja(id) を2回行います。
* - thinkByNinja(id) は、一人の忍者の一歩の行動を決める関数です(詳しくは関数のコメントを参照)。
*
* - 忍術
* -- 「超高速」のみを使用します。
* -- 「超高速」を使えるだけの忍力を所持している場合に自動的に使用して、thinkByNinja(id) を1回多く呼び出します。
*/
void think() {
	int moveLoop = 2;
	if (myState.skillPoint >= skills[0].cost) {
		std::cout << 3 << std::endl;
		std::cout << skills[0].id << std::endl;
		moveLoop = 3;
	}
	else {
		std::cout << 2 << std::endl;
	}
	for (int i = 0; i < myState.ninjas.size(); ++i) {
		for (int j = 0; j < moveLoop; ++j) {
			std::cout << thinkByNinjaId(myState.ninjas[i].id);
		}
		std::cout << std::endl;
	}
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
		think();
		std::cout.flush();
	}
	return 0;
}
