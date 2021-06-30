#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#define INF 1000
#define OUTPUT_CD ((clock_t)100)
#define OS_CLOCK ((long long)10000000 / (long long)CLOCKS_PER_SEC)
#define FULLTIME ((long long)CLOCKS_PER_SEC * (long long)(OS_CLOCK* 9.9))
#define RETURN_TIME 1
//#define RETURN_TIME ((long long)CLOCKS_PER_SEC * 10000 / 10000000))
using namespace std;
enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
};
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

class OthelloBoard {
public:
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    int board[8][8];
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
//                    disc_count[cur_player] += discs.size();
//                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
        disc_count[EMPTY] = 0;
        disc_count[BLACK] = 0;
        disc_count[WHITE] = 0;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
//        disc_count[cur_player]++;
//        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int val = getval();
//                int white_discs = disc_count[WHITE];
//                int black_discs = disc_count[BLACK];
                if (val == 0) winner = EMPTY;
                else if (val > 0) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
    int getval(void){
        disc_count[EMPTY] = 0;
        disc_count[BLACK] = 0;
        disc_count[WHITE] = 0;
        for(int i = 0; i < 8; i ++) for(int j = 0; j < 8; j ++) disc_count[board[i][j]] ++;
        return disc_count[BLACK] - disc_count[WHITE];
    }
};

int player, pos = 0;
const int SIZE = 8;
clock_t preOutput = 0;
int board[8][8];
std::vector<Point> next_valid_spots;
OthelloBoard now;

int Alpha_Beta_Pruning(int A, int B, Point point, std::ofstream& fout, long long time){
    if(clock() - preOutput >= OUTPUT_CD){
        Point p = next_valid_spots[pos];
        fout << p.x << " " << p.y << std::endl;
        fout.flush();
        preOutput = clock();
    }
    now.done = false;
    now.put_disc(point);
    if(time <= RETURN_TIME || now.done) return now.getval();
    int tboard[8][8];
    for(int i = 0; i < 8; i ++) for(int j = 0; j < 8; j ++) tboard[i][j] = now.board[i][j];
    vector<Point>spots = now.next_valid_spots;
    int valid_spots_num = now.next_valid_spots.size();
    int play = now.cur_player;
    if(play == BLACK){
        int value = -INF;
        for(auto p : spots){
            now.cur_player = play;
            for(int i = 0; i < 8; i ++) for(int j = 0; j < 8; j ++) now.board[i][j] = tboard[i][j];
            value = max(value, Alpha_Beta_Pruning(A, B, p, fout, time / valid_spots_num));
            A = max(A, value);
            if(A >= B) break;
        }
        return value;
    }else {
        int value = INF;
        for(auto p : spots){
            now.cur_player = play;
            for(int i = 0; i < 8; i ++) for(int j = 0; j < 8; j ++) now.board[i][j] = tboard[i][j];
            value = min(value, Alpha_Beta_Pruning(A, B, p, fout, time / valid_spots_num));
            B = min(B, value);
            if(A <= B) break;
        }
        return value;
    }
}
void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    for(int i = 0; i < 8; i ++) for(int j = 0; j < 8; j ++) now.board[i][j] = board[i][j];
    int value = -INF, A = -INF, B = INF, idx = 0;
    now.cur_player = player;
    for(auto pnt : next_valid_spots){
        for(int i = 0; i < 8; i ++) for(int j = 0; j < 8; j ++) now.board[i][j] = board[i][j];
        now.cur_player = player;
        value = max(value, Alpha_Beta_Pruning(A, B, pnt, fout, FULLTIME / next_valid_spots.size()));
        if(value > A) pos = idx;
        A = max(A, value);
        idx++;
    }
    Point p = next_valid_spots[pos];
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
