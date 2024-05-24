#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <tuple>
#include <algorithm>
#include <limits>
#include <ctime>
#include<chrono>
#include<cassert>


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
int blue_base_count;
int red_base_count;
vector<string> map;   //记录战地地图
vector<vector<int>> record; 
vector<Base> blue_bases;
vector<Base> red_bases;
vector<Fighter> fighters;

// int res[55][55];

void parseInput() {
    //输出地图信息
    cin >> n >> m;
    map.resize(n);
    record.resize(n, vector<int>(m, -1));
    for (int i = 0; i < n; ++i) {
        cin >> map[i];
    }


    //输入我方蓝色基地信息
    cin >> blue_base_count;
    blue_bases.resize(blue_base_count);
    record.resize(n);
    for (int i = 0; i < n; i++)
        record[i].resize(m, -1);

    for (int i = 0; i < blue_base_count; ++i) {
        cin >> blue_bases[i].x >> blue_bases[i].y
            >> blue_bases[i].fuel >> blue_bases[i].missiles
            >> blue_bases[i].defense >> blue_bases[i].value;
            record[blue_bases[i].x][blue_bases[i].y] = i;
    }

    //输入敌方红色基地信息
    cin >> red_base_count;
    red_bases.resize(red_base_count);
    for (int i = 0; i < red_base_count; ++i) {
        cin >> red_bases[i].x >> red_bases[i].y
            >> red_bases[i].fuel >> red_bases[i].missiles
            >> red_bases[i].defense >> red_bases[i].value;

            record[red_bases[i].x][red_bases[i].y] = i ; 
    }

    //输入我方军舰信息
    int fighter_count;
    cin >> fighter_count;
    fighters.resize(fighter_count);
    for (int i = 0; i < fighter_count; ++i) {
        fighters[i].id = i;
        cin >> fighters[i].x >> fighters[i].y
            >> fighters[i].fuel_capacity >> fighters[i].missile_capacity;

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
    if (fighter.actionTaken) return;
    

    int newX = fighter.x + dx[dir];
    int newY = fighter.y + dy[dir];

    // Check boundaries and map content for valid move
    if (newX >= 0 && newX < n && newY >= 0 && newY < m && map[newX][newY] != '#') {
        fighter.x = newX;
        fighter.y = newY;
        
        fighter.fuel--;  // Decrement fuel after moving
        Command move_cmd("move", fighter.id, dir);
        outputCommand(move_cmd);
        fighter.actionTaken = true;
    } 


}


void attackBase(Fighter& fighter, Base& base, int dir) {
    if (fighter.actionTaken) return;  // Ensure no action is taken if one has already been executed this frame

    // Calculate the distance to ensure the base is within the attack range
    int distance = calculateDistance(fighter.x, fighter.y, base.x, base.y);
    if (distance <= fighter.range && fighter.missiles >= 0 && base.defense >= 0) {
        int missiles_used = min(fighter.missiles, base.defense);
        fighter.missiles -= missiles_used;  // Reduce the number of available missiles
        base.defense -= missiles_used;  // Reduce the base's defense


        // Create and output the attack command
        if(missiles_used > 0) {
            Command attack_cmd("attack", fighter.id, dir, missiles_used);
            outputCommand(attack_cmd);
        }

        // fighter.actionTaken = true;  // Mark that this fighter has taken an action this frame

        if (base.defense <= 0) {
            // cout << "Base at (" << base.x << ", " << base.y << ") destroyed" << endl;
            map[base.x][base.y] = '.';
            record[base.x][base.y] = -1;
        }
    }
}







void refuel(Fighter& fighter, Base& base) {

    if (base.x == fighter.x && base.y == fighter.y) {
        int fuel_needed = fighter.fuel_capacity - fighter.fuel;
        int fuel_to_add = min(fuel_needed, base.fuel);
        // cout << "Fighter " << fighter.id << " refueled " << fuel_to_add << " units at base (" << base.x << ", " << base.y << "), with base feul capacity : " << base.fuel << endl;
        assert(fuel_to_add >= 0);
        fighter.fuel += fuel_to_add;
        base.fuel -= fuel_to_add; // Optionally decrease the base's fuel store
        // cout << "AhhhhFighter " << fighter.id << " refueled " << fuel_to_add << " units at base (" << base.x << ", " << base.y << "), with base feul capacity : " << blue_bases[record[45][73]].fuel << endl;
        Command fuel_cmd("fuel", fighter.id, fuel_to_add);
        outputCommand(fuel_cmd);
        // fighter.actionTaken = true;
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
        // fighter.actionTaken = true;
        // break;
    }
    
}



pair<int, vector<pair<int, int>>> precomputedPaths (int sx, int sy) {
    // cerr << "t1" << endl;
    vector<vector<int>> dist(n, vector<int>(m, -1));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(m, {-1, -1}));
    queue<pair<int, int>> q;
    q.push({sx, sy});
    dist[sx][sy] = 0;
    int id = -1;

    while (!q.empty()) {
        // cerr << "t2" << endl;
        auto [x, y] = q.front();
        q.pop();
        if (map[x][y] == '#')
        {
            // cerr << "t3" << endl;
            id = record[x][y];
            break;
        }

        for (int i = 0; i < 4; ++i) {
            // cerr << "t4" << endl;

            int nx = x + dx[i], ny = y + dy[i];
            // cout << "map[nx][ny]: " << map[nx][ny] ;
            if (nx >= 0 && nx < n && ny >= 0 && ny < m  && dist[nx][ny] == -1) {
                // cerr << "t5" << endl;
                dist[nx][ny] = dist[x][y] + 1;
                parent[nx][ny] = {x, y};
                q.push({nx, ny});
                // cout << "find a path" << endl;
            }

        }
    }

    vector<pair<int, int>> path;
    if(id < 0) return {-1, path};
    for (pair<int, int> at = {red_bases[id].x, red_bases[id].y}; at.first != -1; at = parent[at.first][at.second]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    return {id, path};
}



void simulate() {
    const int total_frames = 15000; // Maximum number of frames

    for (int frame_number = 0; frame_number < total_frames; frame_number++) {
        
        cerr << "---------start frame:" << frame_number << " simulate-----------" << endl;
        // precomputePaths();
        if (frame_number > 2000)
        {
            cout<<"OK\n";
            continue;
        }
        
        //rest action taken
        for (auto& fighter : fighters) {
            fighter.actionTaken = false;
        }

        for (auto& fighter : fighters) {
            // cerr << "filghter "<<fighter.id << "start" << endl;
            
            //目前所在位置为己方基地，进行加油和加弹
            // cerr << "t1" << endl;
            int cur_pos_id = record[fighter.x][fighter.y];
            if (map[fighter.x][fighter.y] == '*'){   
                // Base tmp_blue_base = blue_bases[cur_pos_id];
                reload(fighter, blue_bases[cur_pos_id]);
                refuel(fighter, blue_bases[cur_pos_id]);
            }
            //目前所在位置四周为敌方基地，进行攻击
            // cerr << "t2" << endl;
            for(int d=0; d<4; d++){
                // cerr << "enter check" << endl;
                int nx = fighter.x + dx[d];
                int ny = fighter.y + dy[d];
                if (nx >= 0 && nx < n && ny >= 0 && ny < m){
                    // cerr << "r : " << record[nx][ny] << endl;
                    // cerr  << nx<<" "<<ny << endl;
                    // cerr << "mp: " << map[nx][ny] << endl;
                }

                if (nx >= 0 && nx < n && ny >= 0 && ny < m && record[nx][ny] != -1 && map[nx][ny] == '#'){
                    int baseIndex = record[nx][ny];
                    // cerr << "start attack" << endl;
                    attackBase(fighter, red_bases[baseIndex], d);
                }
            }
            
            
            if (fighter.fuel <= 0) continue;

            // int id = -1;
            // vector<pair<int, int>> precomputedPath;
            // cerr << "t3" << endl;
            auto [id, path] = precomputedPaths(fighter.x, fighter.y);
            // cerr << "t4" << endl;
            if(path.size()>1){
                moveFighter(fighter, getDirection(path[0].first, path[0].second, path[1].first, path[1].second));
            }


        }

        cout << "OK" << endl;
    }
}



int main() {
    freopen("./data/testcase3.in","r",stdin);
    freopen("./output/testcase3.out","w",stdout);
    auto start = std::chrono::high_resolution_clock::now();
    parseInput();

    simulate();
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - start;
    std::cerr<< ". The time now is: " << elapsed.count() << "seconds" << ". ";
    return 0;
}
