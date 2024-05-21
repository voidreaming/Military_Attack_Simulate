#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <tuple>
#include <algorithm>
#include <limits>
#include <ctime>

using namespace std;

//基本的数据结构

struct Base {
    int x, y;
    int fuel, missiles, defense, value;
};

struct Fighter {
    int id;
    int x, y;
    int fuel_capacity, missile_capacity;
    int fuel, missiles;
    int range = 1;
    bool actionTaken = false;
};

struct Command {
    string type;
    int id;
    int dir_or_count;
    int count;

    Command(string t, int i, int d, int c = 0) : type(t), id(i), dir_or_count(d), count(c) {}

    string toString() const {
        return type + " " + to_string(id) + " " + to_string(dir_or_count) + (type == "attack" ? " " + to_string(count) : "");
    }
};


//处理数据输入

int n, m;
vector<string> map;   //记录战地地图
vector<vector<int>> record; //记录己方基地信息
vector<Base> blue_bases;
vector<Base> red_bases;
vector<Fighter> fighters;

// int res[55][55];

void parseInput() {
    cin >> n >> m;
    map.resize(n);
    for (int i = 0; i < n; ++i) {
        cin >> map[i];
        // cout << "map[" << i << "]" << ":" << map[i] <<endl;
    }

    int blue_base_count;
    cin >> blue_base_count;
    blue_bases.resize(blue_base_count);

    record.resize(n);
    for (int i = 0; i < n; i++)
        record[i].resize(m, -1);

    for (int i = 0; i < blue_base_count; ++i) {
        cin >> blue_bases[i].x >> blue_bases[i].y
            >> blue_bases[i].fuel >> blue_bases[i].missiles
            >> blue_bases[i].defense >> blue_bases[i].value;
        // cout << "cin blue bases successfully" << endl;
        // record[i].resize
            record[blue_bases[i].x][blue_bases[i].y]=i;
    }

    int red_base_count;
    cin >> red_base_count;
    red_bases.resize(red_base_count);
    for (int i = 0; i < red_base_count; ++i) {
        cin >> red_bases[i].x >> red_bases[i].y
            >> red_bases[i].fuel >> red_bases[i].missiles
            >> red_bases[i].defense >> red_bases[i].value;
        // cout << "red bases" << red_base_count << endl;
    }

    int fighter_count;
    cin >> fighter_count;
    fighters.resize(fighter_count);
    for (int i = 0; i < fighter_count; ++i) {
        fighters[i].id = i;
        cin >> fighters[i].x >> fighters[i].y
            >> fighters[i].fuel_capacity >> fighters[i].missile_capacity;
        // cout << "cin fighthers bases successfully" << endl;
        fighters[i].fuel = 0;
        fighters[i].missiles = 0;
    }
}

//处理指令输出

void outputCommand(const Command& command) {
    cout << command.toString() << endl;
    
}


//处理元动作，包括确定方向、确定最近距离、move、attack

vector<int> dx = {-1, 1, 0, 0};
vector<int> dy = {0, 0, -1, 1};

int getDirection(int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        if (y1 < y2) return 3; // right
        else return 2; // left
    } else {
        if (x1 < x2) return 1; // down
        else return 0; // up
    }
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void moveFighter(Fighter& fighter, int dir) {
    int newX = fighter.x + dx[dir];
    int newY = fighter.y + dy[dir];

    // Check boundaries and map content for valid move
    if (newX >= 0 && newX < n && newY >= 0 && newY < m && map[newX][newY] != '#') {
        fighter.x = newX;
        fighter.y = newY;
        fighter.fuel--;  // Decrement fuel after moving
        Command move_cmd("move", fighter.id, dir);
        outputCommand(move_cmd);
    } 

    fighter.actionTaken = true;
    // else {
    //     cout << "[WARNING] move " << fighter.id << " " << dir << ": Invalid move" << endl;
    // }
}

void attackBase(Fighter& fighter, Base& base, int dir) {
    // cout << "execute attack command" << endl;
    // Calculate the effective distance to the base
    int distance = calculateDistance(fighter.x, fighter.y, base.x, base.y);
    int missiles_used = min(fighter.missiles, base.defense);
    if (distance <= 1 && fighter.missiles > 0 && base.defense > 0) {  // Assuming attack range is 1
        // int missiles_used = min(fighter.missiles, base.defense);
        fighter.missiles -= missiles_used;
        base.defense -= missiles_used;
        Command attack_cmd("attack", fighter.id, dir, missiles_used);
        outputCommand(attack_cmd);

        if (base.defense <= 0) {
            cerr << "Base at (" << base.x << ", " << base.y << ") destroyed" << endl;
        }
    }

    fighter.actionTaken = true;
    // } else {
    //     cout << "[WARNING] attack " << fighter.id << " " << dir << " " << missiles_used << ": Invalid attack" << endl;
    // }
}

void refuel(Fighter& fighter, Base& base) {

    if (base.x == fighter.x && base.y == fighter.y) {
        int fuel_needed = fighter.fuel_capacity - fighter.fuel;
        int fuel_to_add = min(fuel_needed, base.fuel);
        fighter.fuel += fuel_to_add;
        base.fuel -= fuel_to_add; // Optionally decrease the base's fuel store
        // cerr << "Fighter " << fighter.id << " refueled " << fuel_to_add << " units at base (" << base.x << ", " << base.y << ")" << endl;
        Command fuel_cmd("fuel", fighter.id, fuel_to_add);
        outputCommand(fuel_cmd);
        fighter.actionTaken = true;
        // break;
    }
    return;
    
}

void reload(Fighter& fighter, Base& base) {

    if (base.x == fighter.x && base.y == fighter.y) {
        // cerr << "fuel execute" << endl;
        int missiles_needed = fighter.missile_capacity - fighter.missiles;
        int missiles_to_add = min(missiles_needed, base.missiles);
        // cerr<<fighter.missiles<<"\n";
        fighter.missiles += missiles_to_add;
        base.missiles -= missiles_to_add; // Optionally decrease the base's missile store
        // cerr << "Fighter " << fighter.id << " reloaded " << missiles_to_add << " missiles at base (" << base.x << ", " << base.y << ")" << endl;
        Command missile_cmd("missile", fighter.id, missiles_to_add);
        outputCommand(missile_cmd);
        fighter.actionTaken = true;
        // break;
    }
    
}


//优化bfs搜索部分


vector<pair<int, int>> bfsPath(int sx, int sy, int tx, int ty) {
    // cout << "start execute bfs function" << endl;
    vector<vector<int>> dist(n, vector<int>(m, -1));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(m, {-1, -1}));
    queue<pair<int, int>> q;
    q.push({sx, sy});
    dist[sx][sy] = 0;

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        if (x == tx && y == ty) break;
        if (map[x][y] == '#') continue;
        for (int i = 0; i < 4; ++i) {

            int nx = x + dx[i], ny = y + dy[i];
            // cout << "map[nx][ny]: " << map[nx][ny] ;
            if (nx >= 0 && nx < n && ny >= 0 && ny < m  && dist[nx][ny] == -1) {
                dist[nx][ny] = dist[x][y] + 1;
                parent[nx][ny] = {x, y};
                q.push({nx, ny});
                // cout << "find a path" << endl;
            }

        }
    }

    vector<pair<int, int>> path;
    if (dist[tx][ty] == -1) return path; // no path found

    for (pair<int, int> at = {tx, ty}; at.first != -1; at = parent[at.first][at.second]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    return path;
}

unordered_map<int, unordered_map<int, vector<pair<int, int>>>> precomputedPaths;

void precomputePaths() {
    for (auto& fighter : fighters) {
        int min_dist = numeric_limits<int>::max();
        int closest_base_index = -1;
        vector<pair<int, int>> shortest_path;

        for (size_t i = 0; i < red_bases.size(); ++i) {
            auto& base = red_bases[i];
            vector<pair<int, int>> path = bfsPath(fighter.x, fighter.y, base.x, base.y);
            if (!path.empty() && path.size() < min_dist) {
                min_dist = path.size();
                shortest_path = path;
                closest_base_index = i;
            }
        }

        if (closest_base_index != -1) {
            precomputedPaths[fighter.id][closest_base_index] = shortest_path;
        }
    }
}

void simulate() {
    const int total_frames = 15; // Maximum number of frames

    for (int frame_number = 0; frame_number < total_frames; frame_number++) {
        
        cerr << "---------start frame:" << frame_number << " simulate-----------" << endl;
        precomputePaths();
        
        //rest action taken
        for (auto& fighter : fighters) {
            fighter.actionTaken = false;
        }

        for (auto& fighter : fighters) {
            cerr << "filghter "<<fighter.id << "start" << endl;
            int tmp_blue_base_id = record[fighter.x][fighter.y];
            Base tmp_blue_base = blue_bases[tmp_blue_base_id];
            reload(fighter, tmp_blue_base);
            refuel(fighter, tmp_blue_base);
            
            if (fighter.fuel <= 0) continue;

            for (const auto& base_paths : precomputedPaths[fighter.id]) {
                // cerr << "base path size" << base_paths.size()<< endl;
                int base_id = base_paths.first;
                const auto& path = base_paths.second;
                if(!path.empty()){
                    moveFighter(fighter, getDirection(path[0].first, path[0].second, path[1].first, path[1].second));
                }

                if (fighter.fuel > 0 && fighter.missiles >= red_bases[base_id].defense && !fighter.actionTaken && calculateDistance(fighter.x, fighter.y, red_bases[base_id].x, red_bases[base_id].y) <= fighter.range) {
                    attackBase(fighter, red_bases[base_id], getDirection(fighter.x, fighter.y, red_bases[base_id].x, red_bases[base_id].y));
                }
            }


        }

        cout << "OK" << endl;
    }
}



int main() {
    freopen("./data/testcase1.in","r",stdin);
    freopen("./output/testcase1.out","w",stdout);
    parseInput();
    simulate();
    return 0;
}
