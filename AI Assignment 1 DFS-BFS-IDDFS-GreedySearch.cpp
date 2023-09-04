#include <bits/stdc++.h>
using namespace std;

unordered_map<string, pair<float, float>> positions;
unordered_map<string, vector<pair<string, float>>> roads;
unordered_map<string, bool> visited;
unordered_map<string, string> parent;
unordered_map<string, float> cost;
unordered_map<string, float> heuristic;

int nodesExpanded = 0;
float pathCost = 0;
bool destCityReached = false;
bool noChildToVisit = true;
string minHeuristic;
float minimum = float(INT_MAX);

//------------------------Utility Functions--------------------------

// Funtion to reset used variables & data structures
void Reset() {
  nodesExpanded = 0;
  pathCost = 0;
  destCityReached = false;

  for (auto i = parent.begin(); i != parent.end(); i++) {
    i->second = "";
  }
  for (auto i = visited.begin(); i != visited.end(); i++) {
    i->second = false;
  }
  for (auto i = cost.begin(); i != cost.end(); i++) {
    i->second = 0;
  }
}

void PrintPositions() {
  for (auto i = positions.begin(); i != positions.end(); i++) {
    cout << i->first << " " << i->second.first << ", " << i->second.second
         << '\n';
  }
}

void PrintRoads() {
  for (auto i = roads.begin(); i != roads.end(); i++) {
    cout << i->first;
    for (auto j = i->second.begin(); j != i->second.end(); j++) {
      cout << " (" << j->first << " " << j->second << ") ";
    }
    cout << '\n';
  }
}

void StorePositions() {
  string city;
  float x, y;
  ifstream myFile("positions.txt");
  if (myFile.is_open()) {
    while (myFile >> city >> x >> y) {
      positions[city].first = x;
      positions[city].second = y;
    }
    myFile.close();
  }
}

void StoreRoads() {
  string city1, city2;
  float dist;
  ifstream myFile("roads.txt");
  if (myFile.is_open()) {
    while (myFile >> city1 >> city2 >> dist) {
      roads[city1].push_back(make_pair(city2, dist));
      // if bidirectional
      roads[city2].push_back(make_pair(city1, dist));
    }
    myFile.close();
  }
}

//------------------------DFS--------------------------

void DFS(string srcCity, string destCity) {
  visited[srcCity] = true;
  nodesExpanded += 1;
  // cout << srcCity << " ";

  // Add condition to break when destination is reached
  if (srcCity == destCity) {
    destCityReached = true;
  }

  for (auto i = roads[srcCity].begin(); i != roads[srcCity].end(); i++) {
    if (!visited[i->first] && destCityReached == false) {
      parent[i->first] = srcCity;
      cost[i->first] = i->second;
      DFS(i->first, destCity);
    }
  }
}

//------------------------Iterative Deepening--------------------------

// A Utility function for IDDFS
bool DLS(string srcCity, string destCity, int limit) {
  visited[srcCity] = true;
  nodesExpanded += 1;
  // cout << srcCity << " ";

  if (srcCity == destCity)
    return true;

  // If reached the maximum depth, stop recursing.
  if (limit <= 0)
    return false;

  // Recur for all the vertices adjacent to source vertex
  for (auto i = roads[srcCity].begin(); i != roads[srcCity].end(); ++i) {
    if (!visited[i->first]) {
      parent[i->first] = srcCity;
      cost[i->first] = i->second;
      if (DLS(i->first, destCity, limit - 1) == true)
        return true;
    }
  }

  return false;
}

bool IDDFS(string srcCity, string destCity, int max_depth) {
  // Repeatedly depth-limit search till the
  // maximum depth.
  for (int i = 0; i <= max_depth; i++) {
    Reset();
    parent[srcCity] = srcCity;
    if (DLS(srcCity, destCity, i) == true)
      return true;
    // cout << '\n';
  }
  return false;
}

//------------------------Heuristic--------------------------

void HeuristicFunction(string destCity) {
  for (auto i = positions.begin(); i != positions.end(); i++) {
    float xSquared =
        pow((positions[destCity].first - positions[i->first].first), 2);
    float ySquared =
        pow((positions[destCity].second - positions[i->first].second), 2);

    heuristic[i->first] = sqrt((xSquared + ySquared));
  }
}

void PrintHeuristic() {
  for (auto i = heuristic.begin(); i != heuristic.end(); i++) {
    cout << i->first << " " << i->second << endl;
  }
}

//------------------------Greedy--------------------------

bool Greedy(string srcCity, string destCity) {
  visited[srcCity] = true;
  nodesExpanded += 1;
  // cout << srcCity << " ";

  if (srcCity == destCity) {
    return true;
  }

  minimum = float(INT_MAX);
  float cost = 0;

  for (auto i = roads[srcCity].begin(); i != roads[srcCity].end(); i++) {
    if (heuristic[i->first] < minimum) {
      minimum = heuristic[i->first];
      minHeuristic = i->first;
      cost = i->second;
    }
  }

  if (visited[minHeuristic]) {
    return false;
  }

  parent[minHeuristic] = srcCity;
  pathCost += cost;
  return Greedy(minHeuristic, destCity);
}

//------------------------BFS--------------------------

bool BFS(string srcCity, string DestCity) {
  deque<string> myDeque;
  parent[srcCity] = srcCity;
  visited[srcCity] = true;
  myDeque.push_back(srcCity);
  string currentCity;

  while (!myDeque.empty()) {
    nodesExpanded += 1;
    currentCity = myDeque.front();
    // cout << currentCity << " ";
    myDeque.pop_front();
    if (currentCity == DestCity) {
      return true;
    }

    for (auto i = roads[currentCity].begin(); i != roads[currentCity].end();
         i++) {
      if (!visited[i->first]) {
        parent[i->first] = currentCity;
        cost[i->first] = i->second;
        visited[i->first] = true;
        myDeque.push_back(i->first);
      }
    }
  }

  return false;
}

//------------------------Interface--------------------------

string PrintPath(string city) {
  if (city != parent[city]) {
    pathCost += cost[city];
    return PrintPath(parent[city]) + " -> " + city;
  } else
    return city;
}

void PrintStats(string srcCity, string destCity) {
  parent[srcCity] = srcCity;
  string path = PrintPath(destCity);
  cout << nodesExpanded << " " << pathCost << " " << path << '\n';
}

void Interface() {
  // Preprocessing (Reading data from files)
  StorePositions();
  StoreRoads();

  string srcCity, destCity;
  int selection, max_depth;

  // User prompt
  cout << "Enter the name of the city you want to start at: ";
  cin >> srcCity;
  cout << "Enter the name of the city you want a path to: ";
  cin >> destCity;

  cout << "\n1 - DFS\n2 - BFS\n3 - IDDFS\n4 - Uniform Cost\n5 - Greedy\n6 - "
          "A*\n";
  cout << "Enter corresponding number for your desired search algorithm: ";
  cin >> selection;
  cout << '\n';

  // Algorithm Selections
  switch (selection) {
  case 1:
    DFS(srcCity, destCity);
    PrintStats(srcCity, destCity);

    break;

  case 2:
    if (BFS(srcCity, destCity)) {
      PrintStats(srcCity, destCity);
    } else {
      cout << "BFS Search couldn't find a path.\n\n";
    }

    break;

  case 3:
    max_depth = 0;
    cout << "Please specify the maximum depth (Starting from zero): ";
    cin >> max_depth;
    if (IDDFS(srcCity, destCity, max_depth)) {
      PrintStats(srcCity, destCity);
    } else {
      cout << "Expanded Nodes: " << nodesExpanded << '\n';
      cout << "A solution was not found at the specified depth.\n";
    }

    break;

  case 5:
    HeuristicFunction(destCity);
    // PrintHeuristic();
    if (Greedy(srcCity, destCity)) {
      PrintStats(srcCity, destCity);
    } else {
      cout << "Greedy Search couldn't find a path as it has entered an "
              "infinite loop.\n\n";
    }

    break;

  default:
    cout << "Incorrect number was entered...\n";
  }
}

//------------------------main--------------------------
int main() {

  Interface();

  return 0;
}