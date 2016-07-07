using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

class chokudAI
{
	string name = "chokudAI_zentansaku3";

	static void Main(string[] args)
	{
		new chokudAI();
	}

	Scanner cin;

	static public int RemainTime;
	static public int SkillNum;
	static public int[] SkillCost;
	enum Skills { CHOKASOKU, JIRAKUSEKI, TEKIRAKUSEKI, JIRAIGEKI, TEKIRAIGEKI, JIBUNSHIN, TEKIBUNSHIN, KAITENGIRI };

	public static int H;
	public static int W;


	int[] Power;
	int[] Ninja;
	string[][] Map;
	int[][] NinjaID, NinjaY, NinjaX;
	int[] Dog;
	int[][] DogID, DogY, DogX;
	int[] Soul;
	int[][] SoulY, SoulX;
	int[][] usedSkill;

	int[] vy = { -1, 0, 0, 1 };
	int[] vx = { 0, -1, 1, 0 };

	Board[] board;

	int T;

	public chokudAI()
	{
		T = 0;
		cin = new Scanner();
		Console.WriteLine(name);
		while (true)
		{
			Console.Error.WriteLine("Turn: {0}", ++T);

			RemainTime = cin.nextInt();
			SkillNum = cin.nextInt();
			SkillCost = new int[SkillNum];
			for (int i = 0; i < SkillNum; i++)
			{
				SkillCost[i] = cin.nextInt();
			}
			Board.SkillCost = SkillCost;

			Power = new int[2];
			Ninja = new int[2];
			Map = new string[2][];
			NinjaID = new int[2][];
			NinjaY = new int[2][];
			NinjaX = new int[2][];
			Dog = new int[2];
			DogID = new int[2][];
			DogY = new int[2][];
			DogX = new int[2][];
			Soul = new int[2];
			SoulY = new int[2][];
			SoulX = new int[2][];
			usedSkill = new int[2][];
			board = new Board[2];

			for (int i = 0; i < 2; i++)
			{
				Power[i] = cin.nextInt();
				H = cin.nextInt();
				W = cin.nextInt();
				Board.H = H;
				Board.W = W;
				Board.SkillNum = SkillNum;
				Map[i] = new string[H];
				for (int j = 0; j < H; j++)
				{
					Map[i][j] = cin.next();
				}

				Ninja[i] = cin.nextInt();
				NinjaID[i] = new int[Ninja[i]];
				NinjaY[i] = new int[Ninja[i]];
				NinjaX[i] = new int[Ninja[i]];
				for (int j = 0; j < Ninja[i]; j++)
				{
					NinjaID[i][j] = cin.nextInt();
					NinjaY[i][j] = cin.nextInt();
					NinjaX[i][j] = cin.nextInt();
				}
				Dog[i] = cin.nextInt();

				DogID[i] = new int[Dog[i]];
				DogY[i] = new int[Dog[i]];
				DogX[i] = new int[Dog[i]];

				for (int j = 0; j < Dog[i]; j++)
				{
					DogID[i][j] = cin.nextInt();
					DogY[i][j] = cin.nextInt();
					DogX[i][j] = cin.nextInt();
				}
				Soul[i] = cin.nextInt();
				SoulY[i] = new int[Soul[i]];
				SoulX[i] = new int[Soul[i]];
				for (int j = 0; j < Soul[i]; j++)
				{
					SoulY[i][j] = cin.nextInt();
					SoulX[i][j] = cin.nextInt();
				}
				usedSkill[i] = new int[SkillNum];
				for (int j = 0; j < SkillNum; j++)
				{
					usedSkill[i][j] = cin.nextInt();
				}
				board[i] = new Board(Map[i], Power[i], Ninja[i], NinjaID[i], NinjaY[i], NinjaX[i], Dog[i], DogID[i], DogY[i], DogX[i], Soul[i], SoulY[i], SoulX[i], usedSkill[i]);

				Console.Error.WriteLine("P{0} Pow:{1}, Dog:{2}", i + 1, Power[i], Dog[i]);
			}
			Game game = new Game(board[0], board[1]);
			Move result = game.GetMove(RemainTime);
			if (result.SkillID >= 0)
			{
				Console.WriteLine(3);
				if (result.SkillID == (int)Skills.CHOKASOKU || result.SkillID == (int)Skills.KAITENGIRI)
				{
					Console.WriteLine(result.SkillID);
				}
				else
				{
					Console.WriteLine("{0} {1} {2}", result.SkillID, result.Y, result.X);
				}
			}
			else
			{
				Console.WriteLine(2);
			}
			Console.WriteLine(result.move[0]);
			Console.WriteLine(result.move[1]);


		}
	}


	bool inside(int y, int x)
	{
		return y >= 0 && x >= 0 && y < H && x < W;
	}


	class Board
	{
		public int Power;
		public int Ninja;
		public int[] NinjaID, NinjaY, NinjaX;
		public int Dog;
		public List<int> DogID, DogY, DogX;
		public int Soul;
		public int[] SoulY, SoulX;
		public static int[] SkillCost;

		public int[,] field;
		static public int H, W;
		static public int SkillNum;

		const int NINJA = 1 << 10;
		const int ISNINJA = 3 << 10;
		const int WALL = 1 << 8;
		const int ROCK = 1 << 6;
		const int WALLORROCK = WALL + ROCK;

		const int DOG = 1 << 4;

		const int NOMOVE = (1 << 20) - (DOG);

		const int NGPOINT2 = 1 << 2;
		const int NGPOINT = 1 << 1;
		const int SOUL = 1;

		int[] vy = { -1, 0, 0, 1 };
		int[] vx = { 0, -1, 1, 0 };

		int[] vy8 = { 1, 0, -1, 0, 1, 1, -1, -1 };
		int[] vx8 = { 0, 1, 0, -1, 1, -1, 1, -1 };

		public Board(string[] Map, int Power, int Ninja, int[] NinjaID, int[] NinjaY, int[] NinjaX, int Dog, int[] DogID, int[] DogY, int[] DogX, int Soul, int[] SoulY, int[] SoulX, int[] usedSkill)
		{

			this.Power = Power;
			this.Ninja = Ninja;
			this.NinjaID = (int[])NinjaID.Clone();
			this.NinjaY = (int[])NinjaY.Clone();
			this.NinjaX = (int[])NinjaX.Clone();
			this.Dog = Dog;
			this.DogID = new List<int>(DogID);
			this.DogY = new List<int>(DogY);
			this.DogX = new List<int>(DogX);
			this.Soul = Soul;
			this.SoulY = (int[])SoulY.Clone();
			this.SoulX = (int[])SoulX.Clone();
			MapToField(Map);
		}

		public Board(int[,] field, int Power, int Ninja, int[] NinjaID, int[] NinjaY, int[] NinjaX, int Dog, int[] DogID, int[] DogY, int[] DogX, int Soul, int[] SoulY, int[] SoulX)
		{
			this.field = (int[,])field.Clone();
			this.Power = Power;
			this.Ninja = Ninja;
			this.NinjaID = (int[])NinjaID.Clone();
			this.NinjaY = (int[])NinjaY.Clone();
			this.NinjaX = (int[])NinjaX.Clone();
			this.Dog = Dog;
			this.DogID = new List<int>(DogID);
			this.DogY = new List<int>(DogY);
			this.DogX = new List<int>(DogX);
			this.Soul = Soul;
			this.SoulY = (int[])SoulY.Clone();
			this.SoulX = (int[])SoulX.Clone();
		}

		public Board(Board b)
		{
			this.field = (int[,])b.field.Clone();
			this.Power = b.Power;
			this.Ninja = b.Ninja;
			this.NinjaID = (int[])b.NinjaID.Clone();
			this.NinjaY = (int[])b.NinjaY.Clone();
			this.NinjaX = (int[])b.NinjaX.Clone();
			this.Dog = b.Dog;
			this.DogID = new List<int>(b.DogID);
			this.DogY = new List<int>(b.DogY);
			this.DogX = new List<int>(b.DogX);
			this.Soul = b.Soul;
			this.SoulY = (int[])b.SoulY.Clone();
			this.SoulX = (int[])b.SoulX.Clone();
		}

		public void MapToField(string[] Map)
		{
			field = new int[H, W];
			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					if (Map[i][j] == 'W')
					{
						field[i, j] += WALL;
						field[i, j] |= NGPOINT2;
						field[i, j] |= NGPOINT;
					}
					else if (Map[i][j] == 'O') field[i, j] += ROCK;
					else field[i, j] = 0;
				}
			}
			for (int i = 0; i < Ninja; i++)
			{
				field[NinjaY[i], NinjaX[i]] += NINJA;
			}
			for (int i = 0; i < Dog; i++)
			{
				field[DogY[i], DogX[i]] += DOG;
				field[DogY[i], DogX[i]] |= NGPOINT;
				field[DogY[i], DogX[i]] |= NGPOINT2;
				for (int k = 0; k < vy.Length; k++)
				{
					int ny = DogY[i] + vy[k];
					int nx = DogX[i] + vx[k];
					if (!inside(ny, nx)) continue;
					field[ny, nx] |= NGPOINT;
				}
			}
			for (int i = 0; i < Soul; i++)
			{
				field[SoulY[i], SoulX[i]] |= SOUL;
			}
		}

		public int UseMySkill(int skillid, int y, int x)
		{
			if (skillid == (int)Skills.JIRAKUSEKI)
			{
				field[y, x] |= ROCK;
				return 0;
			}
			else if (skillid == (int)Skills.JIRAIGEKI)
			{
				field[y, x] &= ~ROCK;
				return 0;
			}
			else if (skillid == (int)Skills.KAITENGIRI)
			{
				int ans = 0;
				for (int k = 0; k < 8; k++)
				{
					int ny = y + vy8[k];
					int nx = x + vx8[k];
					if ((field[y, x] & DOG) != 0)
					{
						field[y, x] &= ~DOG;
						ans++;
					}
				}
				return ans;
			}
			return 0;
		}

		public int UseEnemySkill(int skillid, int y, int x)
		{
			if (skillid == (int)Skills.TEKIRAKUSEKI)
			{
				field[y, x] |= ROCK;
				return 0;
			}
			else if (skillid == (int)Skills.TEKIRAIGEKI)
			{
				field[y, x] &= ~ROCK;
				return 0;
			}
			return 0;
		}

		bool inside(int y, int x)
		{
			return y >= 0 && x >= 0 && y < H && x < W;
		}

		public int checkMoveNinja(int id, char C)
		{
			int k = 0;
			if (C == 'L') k = 1;
			else if (C == 'R') k = 2;
			else if (C == 'D') k = 3;
			return checkMoveNinja(id, k);
		}

		public int checkMoveNinja(int id, int k)
		{
			int y = NinjaY[id] + vy[k];
			int x = NinjaX[id] + vx[k];

			if ((field[y, x] & WALL) != 0) return -1;

			if ((field[y, x] & ROCK) != 0)
			{
				int ny = y + vy[k];
				int nx = x + vx[k];
				if ((field[ny, nx] & NOMOVE) != 0) return -1;
			}
			if ((field[y, x] & SOUL) != 0)
			{
				return 1;
			}
			return 0;
		}


		public int MoveNinja(int id, string S)
		{
			//mada
			int enemy = 0;
			for (int i = 0; i < S.Length; i++)
			{
				enemy += MoveNinja(id, S[i]);
			}
			return enemy;
		}

		public int MoveNinja(int id, char C)
		{
			int k = 0;
			if (C == 'L') k = 1;
			else if (C == 'R') k = 2;
			else if (C == 'D') k = 3;
			return MoveNinja(id, k);
		}

		public int MoveNinja(int id, int k)
		{
			int y = NinjaY[id] + vy[k];
			int x = NinjaX[id] + vx[k];
			if ((field[y, x] & WALL) != 0) return 0;
			if ((field[y, x] & ROCK) != 0)
			{
				int ny = y + vy[k];
				int nx = x + vx[k];
				if ((field[ny, nx] & (NOMOVE)) != 0) return 0;
				field[ny, nx] |= ROCK;
				field[y, x] &= ~ROCK;
			}
			field[NinjaY[id], NinjaX[id]] -= NINJA;
			field[y, x] += NINJA;
			NinjaY[id] = y;
			NinjaX[id] = x;

			if ((field[y, x] & SOUL) != 0)
			{
				field[y, x] &= ~SOUL;
				Power += 2;
				return 2;
			}
			//Console.Error.WriteLine(y + " " + x);
			return 0;
		}

		public int checkMoveNinjaNoRock(int y, int x, int id, char C)
		{
			int k = 0;
			if (C == 'L') k = 1;
			else if (C == 'R') k = 2;
			else if (C == 'D') k = 3;
			return checkMoveNinjaNoRock(y, x, id, k);
		}

		public int checkMoveNinjaNoRock(int y, int x, int id, int k)
		{
			if ((field[y, x] & WALL) != 0) return -1;
			if ((field[y, x] & ROCK) != 0) return -1;
			if ((field[y, x] & SOUL) != 0) return 1;
			return 0;
		}

		int[,] AllNinjaDist;
		int MAX = 9999;

		List<Tuple<int, int>> MoveDogMemo;

		public void MoveDog(int myY, int myX, int enemyY, int enemyX, bool movePOS = true)
		{
			AllNinjaDist = new int[H, W];
			MoveDogMemo = new List<Tuple<int, int>>();


			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					AllNinjaDist[i, j] = MAX;
				}
			}
			if (myY != -1 || enemyY != -1)
			{
				if (myY != -1) pushDist(myY, myX);
				if (enemyY != -1) pushDist(enemyY, enemyX);
			}
			else
			{
				pushDist(NinjaY[0], NinjaX[0]);
				pushDist(NinjaY[1], NinjaX[1]);
			}

			List<Tuple<int, int>> ll = new List<Tuple<int, int>>();
			for (int i = 0; i < DogY.Count; i++)
			{
				ll.Add(Tuple.Create(AllNinjaDist[DogY[i], DogX[i]], i));
			}
			ll.Sort();

			List<Tuple<int, int>> delete = new List<Tuple<int, int>>();
			List<Tuple<int, int>> add = new List<Tuple<int, int>>();
			foreach (var item in ll)
			{
				int i = item.Item2;
				int y = DogY[i];
				int x = DogX[i];
				//回転切り使うならここでDOG消す

				for (int k = 0; k < 4; k++)
				{
					int ny = y + vy[k];
					int nx = x + vx[k];
					if ((field[ny, nx] & (NOMOVE & ~ISNINJA)) != 0)
					{
						if (IsRock(field[ny, nx]))
						{
							MoveDogMemo.Add(Tuple.Create(ny, nx));
						}
						continue;
					}
					if (AllNinjaDist[y, x] - 1 != AllNinjaDist[ny, nx]) continue;
					DogY[i] = ny;
					DogX[i] = nx;

					field[y, x] &= ~DOG;
					field[ny, nx] |= DOG;

					if (!movePOS)
					{
						delete.Add(Tuple.Create(y, x));
						add.Add(Tuple.Create(ny, nx));
					}

					break;
				}
			}
			if (!movePOS)
			{
				foreach (var item in add)
				{
					field[item.Item1, item.Item2] &= ~DOG;
				}
				foreach (var item in delete)
				{
					field[item.Item1, item.Item2] |= DOG;
				}
			}
		}

		public void SetNGPoint()
		{
			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					field[i, j] &= ~(NGPOINT | NGPOINT2);
				}
			}

			for (int i = 0; i < DogY.Count; i++)
			{
				int y = DogY[i];
				int x = DogX[i];
				field[y, x] |= NGPOINT;
				field[y, x] |= NGPOINT2;
				//for (int k = 0; k < 4; k++)
				//{
				//	int ny = y + vy[k];
				//	int nx = x + vx[k];
				//	field[y, x] |= NGPOINT;
				//}
			}
			foreach (var item in MoveDogMemo)
			{
				field[item.Item1, item.Item2] |= (NGPOINT | NGPOINT2);
			}
		}

		public void pushDist(int fy, int fx)
		{
			AllNinjaDist[fy, fx] = 0;
			Queue<Tuple<int, int>> q = new Queue<Tuple<int, int>>();
			q.Enqueue(Tuple.Create(fy, fx));

			while (q.Count != 0)
			{
				var now = q.Dequeue();
				int y = now.Item1;
				int x = now.Item2;
				for (int k = 0; k < 4; k++)
				{
					int ny = y + vy[k];
					int nx = x + vx[k];
					if ((field[ny, nx] & WALLORROCK) != 0) continue;
					if (AllNinjaDist[ny, nx] > AllNinjaDist[y, x] + 1)
					{
						AllNinjaDist[ny, nx] = AllNinjaDist[y, x] + 1;
						q.Enqueue(Tuple.Create(ny, nx));
					}
				}
			}
		}

		public void PopEnemy(int num)
		{
			if (num == 0) return;
			SetNinjaDist(0);
			SetNinjaDist(1);
			for (int t = 0; t < num; t++)
			{
				int maxpos = -1;
				int y = 0;
				int x = 0;
				for (int i = 0; i < H; i++)
				{
					for (int j = 0; j < W; j++)
					{
						if ((field[i, j] & WALLORROCK) != 0) continue;
						if ((field[i, j] & DOG) != 0) continue;
						int d = Math.Min(NinjaDist[0][i, j], NinjaDist[1][i, j]);
						if (d != MAX) continue;
						if (d < maxpos)
						{
							maxpos = d;
							y = i;
							x = j;
						}
					}
				}
				if (maxpos != -1)
				{
					DogX.Add(x);
					DogY.Add(y);
				}
			}
		}

		public Board puchiMove(Move m1)
		{
			Board nb1 = new Board(this);

			if (m1.SkillID >= 0)
			{
				nb1.UseMySkill(m1.SkillID, m1.Y, m1.X);
				nb1.Power -= SkillCost[m1.SkillID];
			}

			nb1.MoveNinja(0, m1.move[0]);
			nb1.MoveNinja(1, m1.move[1]);

			int my, mx, ey, ex;
			my = mx = ey = ex = -1;
			if (m1.SkillID == (int)Skills.JIBUNSHIN)
			{
				my = m1.Y;
				mx = m1.X;
			}
			nb1.MoveDog(my, mx, ey, ex);
			return nb1;
		}

		int[][,] NinjaDist = new int[2][,];
		public void SetNinjaDist(int id)
		{
			NinjaDist[id] = new int[H, W];
			var nd = NinjaDist[id];
			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					nd[i, j] = MAX;
				}
			}

			int fy = NinjaY[id];
			int fx = NinjaX[id];

			Queue<Tuple<int, int>> q = new Queue<Tuple<int, int>>();
			nd[fy, fx] = 0;
			q.Enqueue(Tuple.Create(fy, fx));
			while (q.Count > 0)
			{
				var now = q.Dequeue();
				int y = now.Item1;
				int x = now.Item2;
				for (int k = 0; k < vy.Length; k++)
				{
					int ny = y + vy[k];
					int nx = x + vx[k];
					if (!inside(ny, nx)) continue;
					if (nd[ny, nx] != MAX) continue;
					if (IsWallOrRock(field[ny, nx])) continue;

					nd[ny, nx] = nd[y, x] + 1;
					q.Enqueue(Tuple.Create(ny, nx));
				}
			}
		}

		int[][,] SoulDist = new int[8][,];
		public void SetSoulDist(int id)
		{
			SoulDist[id] = new int[H, W];
			var nd = SoulDist[id];
			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					nd[i, j] = MAX;
				}
			}

			int fy = SoulY[id];
			int fx = SoulX[id];

			Queue<Tuple<int, int>> q = new Queue<Tuple<int, int>>();
			nd[fy, fx] = 0;
			q.Enqueue(Tuple.Create(fy, fx));
			while (q.Count > 0)
			{
				var now = q.Dequeue();
				int y = now.Item1;
				int x = now.Item2;
				for (int k = 0; k < vy.Length; k++)
				{
					int ny = y + vy[k];
					int nx = x + vx[k];
					if (!inside(ny, nx)) continue;
					if (nd[ny, nx] == MAX) continue;

					nd[ny, nx] = nd[y, x] + 1;
					q.Enqueue(Tuple.Create(ny, nx));
				}
			}
		}

		int[,] EnemyDist;
		public void SetEnemyDist(int id)
		{
			EnemyDist = new int[H, W];
			var nd = EnemyDist;
			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					nd[i, j] = MAX;
				}
			}


			Queue<Tuple<int, int>> q = new Queue<Tuple<int, int>>();

			for (int i = 0; i < DogX.Count; i++)
			{
				int fy = DogY[id];
				int fx = DogX[id];
				if ((field[fy, fx] & DOG) == 0) continue;
				nd[fy, fx] = 0;
				q.Enqueue(Tuple.Create(fy, fx));
			}
			while (q.Count > 0)
			{
				var now = q.Dequeue();
				int y = now.Item1;
				int x = now.Item2;
				for (int k = 0; k < vy.Length; k++)
				{
					int ny = y + vy[k];
					int nx = x + vx[k];
					if (!inside(ny, nx)) continue;
					if (nd[ny, nx] == MAX) continue;

					nd[ny, nx] = nd[y, x] + 1;
					q.Enqueue(Tuple.Create(ny, nx));
				}
			}
		}

		string S4 = "ULRD";
		public string EasyMoving(int id, int border)
		{
			string ans = "-1";
			int bestpoint = -99999;

			int NG = NGPOINT;
			if (border == 0) NG = NGPOINT2;


			for (int i = 0; i < 4; i++)
			{
				int add1 = checkMoveNinja(id, i);
				if (add1 == -1) continue;


				Board next = new Board(this);
				next.MoveNinja(id, i);
				next.SetNinjaDist(id);

				if ((field[next.NinjaY[id], next.NinjaX[id]] & NG) == 0)
				{
					int mindist1 = 99;
					for (int k = 0; k < Soul; k++)
					{
						if ((next.field[next.SoulY[k], next.SoulX[k]] & SOUL) == 0) continue;
						mindist1 = Math.Min(mindist1, next.NinjaDist[id][next.SoulY[k], next.SoulX[k]]);
					}
					//Console.Error.WriteLine("{0} {1}: {2}", next.NinjaY[id], next.NinjaX[id], add1 * 1000 - mindist1);
					if (bestpoint < add1 * 1000 - mindist1)
					{
						bestpoint = add1 * 1000 - mindist1;
						ans = S4[i] + "";
					}
				}


				for (int j = 0; j < 4; j++)
				{
					int add2 = next.checkMoveNinja(id, j);
					if (add2 == -1) continue;
					Board n2 = new Board(next);
					n2.MoveNinja(id, j);
					n2.SetNinjaDist(id);

					if ((field[n2.NinjaY[id], n2.NinjaX[id]] & NG) == 0)
					{
						int mindist2 = 99;
						for (int k = 0; k < Soul; k++)
						{
							if ((n2.field[n2.SoulY[k], n2.SoulX[k]] & SOUL) == 0) continue;
							mindist2 = Math.Min(mindist2, n2.NinjaDist[id][n2.SoulY[k], n2.SoulX[k]]);
						}
						//Console.Error.WriteLine("{0} {1}: {2}", n2.NinjaY[id], n2.NinjaX[id], (add1 + add2) * 1000 - mindist2);
						if (bestpoint < (add1 + add2) * 1000 - mindist2)
						{
							ans = S4[i] + "" + S4[j];
							bestpoint = (add1 + add2) * 1000 - mindist2;
						}
					}

				}
			}
			return ans;
		}

		void Debug(int x)
		{
			Console.Error.WriteLine("NGPOINT: " + checknonzero(x & NGPOINT));
			Console.Error.WriteLine("NGPOINT2: " + checknonzero(x & NGPOINT2));
			Console.Error.WriteLine("SOUL: " + checknonzero(x & SOUL));
			Console.Error.WriteLine("DOG: " + checknonzero(x & DOG));
			Console.Error.WriteLine("WALL: " + checknonzero(x & WALL));
			Console.Error.WriteLine("NINJA: " + checknonzero(x & NINJA));
			Console.Error.WriteLine("ISNGPOINT: " + checknonzero(x & NINJA));
		}

		bool checknonzero(int x)
		{
			return x != 0;
		}

		public Tuple<int, int> FindBunshinPos()
		{
			Tuple<int, int> ret = Tuple.Create(1, 1);
			int bestdist = -1;
			SetNinjaDist(0);
			SetNinjaDist(1);
			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					if (IsWallOrRock(field[i, j])) continue;
					//Debug(field[i, j]);
					int d = Math.Min(NinjaDist[0][i, j], NinjaDist[1][i, j]);
					if (bestdist < d && d != MAX)
					{
						bestdist = d;
						ret = Tuple.Create(i, j);
					}
				}
			}
			//Debug(field[ret.Item1, ret.Item2]);
			return ret;
		}


		public bool IsWall(int x) { return ((x) & WALL) != 0; }
		public bool IsSoul(int x) { return ((x) & SOUL) != 0; }
		public bool IsRock(int x) { return ((x) & ROCK) != 0; }
		public bool IsWallOrRock(int x) { return ((x) & WALLORROCK) != 0; }
		public bool IsNinja(int x) { return ((x) & ISNINJA) != 0; }
		public bool IsNGPOINT(int x) { return ((x) & NGPOINT) != 0; }
		public bool IsDog(int x) { return ((x) & DOG) != 0; }

		public bool IsWall(int y, int x) { return IsWall(field[y, x]); }
		public bool IsSoul(int y, int x) { return IsSoul(field[y, x]); }
		public bool IsRock(int y, int x) { return IsRock(field[y, x]); }
		public bool IsWallOrRock(int y, int x) { return IsWallOrRock(field[y, x]); }
		public bool IsNinja(int y, int x) { return IsNinja(field[y, x]); }
		public bool IsNGPOINT(int y, int x) { return IsNGPOINT(field[y, x]); }
		public bool IsDog(int y, int x) { return IsDog(field[y, x]); }

	}

	class Game
	{

		public Board[] board;

		public Game(Board b1, Board b2)
		{
			board = new Board[2];
			board[0] = b1;
			board[1] = b2;
		}

		public Game NextGame(Board b1, Board b2, Move m1, Move m2)
		{
			Board nb1 = new Board(b1);
			Board nb2 = new Board(b2);

			int pop1 = 0;
			int pop2 = 0;
			if (m1.SkillID >= 0)
			{
				pop2 += nb1.UseMySkill(m1.SkillID, m1.Y, m1.X);
				pop1 += nb2.UseEnemySkill(m1.SkillID, m1.Y, m1.X);
			}
			if (m2.SkillID >= 0)
			{
				pop1 += nb2.UseMySkill(m1.SkillID, m1.Y, m1.X);
				pop2 += nb1.UseEnemySkill(m1.SkillID, m1.Y, m1.X);
			}

			pop2 += nb1.MoveNinja(0, m1.move[0]);
			pop2 += nb1.MoveNinja(1, m1.move[1]);
			pop1 += nb2.MoveNinja(0, m2.move[0]);
			pop1 += nb2.MoveNinja(1, m2.move[1]);

			int my, mx, ey, ex;
			my = mx = ey = ex = -1;
			if (m1.SkillID == (int)Skills.JIBUNSHIN)
			{
				my = m1.Y;
				mx = m1.X;
			}
			if (m2.SkillID == (int)Skills.TEKIBUNSHIN)
			{
				ey = m2.Y;
				ex = m2.X;
			}
			nb1.MoveDog(my, mx, ey, ex);

			my = mx = ey = ex = -1;
			if (m2.SkillID == (int)Skills.JIBUNSHIN)
			{
				my = m2.Y;
				mx = m2.X;
			}
			if (m1.SkillID == (int)Skills.TEKIBUNSHIN)
			{
				ey = m1.Y;
				ex = m1.X;
			}
			nb2.MoveDog(my, mx, ey, ex);

			nb1.PopEnemy(pop1);
			nb2.PopEnemy(pop2);

			return new Game(nb1, nb2);

		}

		bool NeedJibunshin()
		{
			return false;
		}

		Move GetEasyMoving(int border, Board b)
		{
			Move Result = new Move();
			Board MyBoard = new Board(b);
			string m1 = MyBoard.EasyMoving(0, border);
			if (m1 == "-1") return null;
			MyBoard.MoveNinja(0, m1);
			//Console.Error.WriteLine(m1);
			string m2 = MyBoard.EasyMoving(1, border);
			if (m2 == "-1") return null;
			MyBoard.MoveNinja(1, m2);
			Result = new Move(-1, 0, 0, new string[] { m1, m2 });
			return Result;
		}

		Move GetEasyMoving(int border)
		{
			Move Result = new Move();
			Board MyBoard = new Board(board[0]);
			string m1 = MyBoard.EasyMoving(0, border);
			if (m1 == "-1") return null;
			MyBoard.MoveNinja(0, m1);
			//Console.Error.WriteLine(m1);
			string m2 = MyBoard.EasyMoving(1, border);
			if (m2 == "-1") return null;
			MyBoard.MoveNinja(1, m2);
			Result = new Move(-1, 0, 0, new string[] { m1, m2 });
			return Result;
		}

		public Move GetEasyMovingJIBUNSHIN()
		{
			return GetEasyMovingJIBUNSHIN(board[0]);
		}

		public Move GetEasyMovingJIBUNSHIN(Board b)
		{
			Move ret = GetEasyMoving(1, b);
			if ((ret == null && b.Power >= SkillCost[(int)Skills.JIBUNSHIN]))
			{
				Board MyBoard = new Board(b);
				var pos = DoJibunshin(MyBoard);

				ret = GetEasyMoving(0, MyBoard);
				if (ret != null)
				{
					ret.SkillID = (int)Skills.JIBUNSHIN;
					ret.Y = pos.Item1;
					ret.X = pos.Item2;
				}
			}
			if (ret == null) return null;
			return ret;
		}


		public Tuple<int, int> DoJibunshin(Board MyBoard)
		{
			var pos = MyBoard.FindBunshinPos();
			MyBoard.MoveDog(pos.Item1, pos.Item2, -1, -1, false);
			MyBoard.SetNGPoint();
			MyBoard.Power -= SkillCost[(int)Skills.JIBUNSHIN];
			return pos;
		}

		int RT;
		Stopwatch sw;
		Board FirstBoard;
		public Move GetMove(int RemainTime)
		{
			RT = RemainTime - 50;
			sw = new Stopwatch();
			sw.Start();

			BestMove = null;
			MoveMemo = new int[2, 2];
			FirstBoard = board[0];
			bestPoint = -1;

			skill = new Tuple<int, int, int>(-1, 0, 0);
			MoveRecall(0, 0, FirstBoard);
			if (board[0].Power >= SkillCost[(int)Skills.JIBUNSHIN])
			{
				Board JibunshinBoard = new Board(FirstBoard);
				var pos = DoJibunshin(JibunshinBoard);
				skill = Tuple.Create((int)Skills.JIBUNSHIN, pos.Item1, pos.Item2);
				MoveRecall(0, 0, JibunshinBoard);
			}

			if (BestMove == null) return new Move();
			return BestMove;
		}

		void MoveRecall(int m1move, int m2move, Board nowBoard)
		{
			if (m1move == 2)
			{
				if (m2move == 0)
				{
					//m1check
					if (nowBoard.IsNGPOINT(nowBoard.NinjaY[0], nowBoard.NinjaX[0]))
					{
						return;
					}
				}
				if (m2move == 2)
				{
					//m2check
					if (nowBoard.IsNGPOINT(nowBoard.NinjaY[1], nowBoard.NinjaX[1]))
					{
						return;
					}

					//OK
					checkBoard();
					return;
				}
				//2
				for (int k = 0; k < 4; k++)
				{
					int add = nowBoard.checkMoveNinja(1, k);
					if (add < 0) continue;
					MoveMemo[1, m2move] = k;
					Board nextBoard = new Board(nowBoard);
					nextBoard.MoveNinja(1, k);
					MoveRecall(m1move, m2move + 1, nextBoard);

					if (m2move == 0)
					{
						MoveMemo[1, 1] = -1;
						MoveRecall(m1move, 2, nextBoard);
					}
				}
			}
			else
			{
				for (int k = 0; k < 4; k++)
				{
					int add = nowBoard.checkMoveNinja(0, k);
					if (add < 0) continue;
					MoveMemo[0, m1move] = k;
					Board nextBoard = new Board(nowBoard);
					nextBoard.MoveNinja(0, k);
					MoveRecall(m1move + 1, m2move, nextBoard);

					if (m1move == 0)
					{
						MoveMemo[0, 1] = -1;
						MoveRecall(2, m2move, nextBoard);

					}
				}
			}
		}

		Move BestMove;
		long bestPoint = -1;

		void checkBoard()
		{
			Board b = new Board(FirstBoard);
			Move M = new Move(skill.Item1, skill.Item2, skill.Item3, MoveMemoToStringArray());

			int SumPower = 0;
			//Console.WriteLine(skill.Item1 + " " + M.move[0] + " " + M.move[1]);
			SumPower -= b.Power;
			b = b.puchiMove(M);
			SumPower += b.Power;
			if (M.SkillID >= 0) SumPower += SkillCost[M.SkillID];

			if (b.IsDog(b.NinjaY[0], b.NinjaX[0])) return;
			if (b.IsDog(b.NinjaY[1], b.NinjaX[1])) return;

			long NowPoint = 0;
			//Console.Error.WriteLine("start!");
			//Console.Error.WriteLine(M.ToString() + " " + b.Power);

			for (int t = 0; t < 10; t++)
			{
				//Console.Error.WriteLine(t + " " + b.NinjaY[0] + " " + b.NinjaX[0] + " " + b.Power);
				Move nextMove = GetEasyMovingJIBUNSHIN(b);
				if (nextMove == null) break;
				//Console.Error.WriteLine(nextMove.ToString());
				SumPower -= b.Power;
				b = b.puchiMove(nextMove);
				SumPower += b.Power;
				if (nextMove.SkillID >= 0) SumPower += SkillCost[nextMove.SkillID];

				//if (b.IsDog(b.NinjaY[0], b.NinjaX[0])) return;
				//if (b.IsDog(b.NinjaY[1], b.NinjaX[1])) return;
				NowPoint += 1000000L * (t + 1) + b.Power + SumPower;
				//if (t == 9) Console.WriteLine(skill.Item1 + " " + M.move[0] + " " + M.move[1] + NowPoint);
				if (NowPoint > bestPoint)
				{
					bestPoint = NowPoint;
					BestMove = M;
					//Console.Error.WriteLine("HighScore!");
					//Console.Error.WriteLine(skill.Item1 + " " + M.move[0] + " " + M.move[1] + " " +  NowPoint + " " + b.Power);
				}
			}
		}

		Tuple<int, int, int> skill;
		int[,] MoveMemo;
		string S4 = "ULRD";

		string[] MoveMemoToStringArray()
		{
			string[] ret = new string[2];
			for (int i = 0; i < 2; i++)
			{
				ret[i] = "";
				for (int j = 0; j < 2; j++)
				{
					if (MoveMemo[i, j] >= 0) ret[i] += S4[MoveMemo[i, j]];
				}
			}
			return ret;
		}


		int GetRemainTime()
		{
			return RT - (int)sw.ElapsedMilliseconds;
		}
	}

	class Move
	{
		public int SkillID, Y, X;
		public string[] move;

		public Move(int SkillID, int Y, int X, string[] move)
		{
			this.SkillID = SkillID;
			this.Y = Y;
			this.X = X;
			this.move = (string[])move.Clone();
		}

		public Move()
		{
			SkillID = -1;
			Y = -1;
			X = -1;
			move = new string[2];
			move[0] = "";
			move[1] = "";
		}

		public string ToString()
		{
			return String.Format("{0} {1} {2} {3} {4}", move[0], move[1], SkillID, Y, X);
		}
	}

}

class Scanner
{
	string[] s;
	int i;

	char[] cs = new char[] { ' ' };

	public Scanner()
	{
		s = new string[0];
		i = 0;
	}

	public string next()
	{
		if (i < s.Length) return s[i++];
		string st = Console.ReadLine();
		while (st == "") st = Console.ReadLine();
		s = st.Split(cs, StringSplitOptions.RemoveEmptyEntries);
		i = 0;
		return next();
	}

	public int nextInt()
	{
		return int.Parse(next());
	}

	public long nextLong()
	{
		return long.Parse(next());
	}

	public double nextDouble()
	{
		return double.Parse(next());
	}

}