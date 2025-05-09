// Group5.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>     // Include the iostream library for standard input/output operations.
#include <iomanip>      // Include the iomanip library for formatted output manipulation.
#include <fstream>      // Include the fstream library for file input/output operations.
#include <sstream>      // Include the sstream library for string stream manipulation.
#include <vector>       // Include the vector library for using dynamic arrays (vectors).

using namespace std;    // Use the standard namespace to avoid writing std:: before standard elements.

const int MAX_AIRPORTS = 200;   // Define a constant for the maximum number of airports (200).
const int INF = 1e9;            // Define a constant for infinity (1e9) to represent unreachable distances.

//==================== TASK 1 ====================//
// Create weighted directed graph from CSV input.
struct Edge {
    int destination; // Integer representing the destination airport's index.
    int distance;    // Integer representing the distance to the destination.
    int cost;        // Integer representing the cost to travel to the destination.
};

struct Airport {
    string code;        // String representing the three-letter airport code.
    string city;        // String representing the city where the airport is located.
    vector<Edge> edges; // Vector of Edge structs, representing connections from this airport to others.
};

Airport airports[MAX_AIRPORTS]; // Array of Airport structs to store airport data.
int airportCount = 0;          // Integer to keep track of the number of airports.

// Store codes and their index mapping
struct CodeIndexPair {
    string code; // String representing the airport code.
    int index; // Integer representing the index of the airport in the airports array.
};
CodeIndexPair airportCodeMap[MAX_AIRPORTS]; // Array of CodeIndexPair structs to map airport codes to indices.
int codeMapSize = 0;                       // Integer to track the number of entries in the airportCodeMap.

// Function to get the index of an airport given its code.
int getAirportIndex(const string& code) {
    // Iterate through the existing airport code map.
    for (int i = 0; i < codeMapSize; i++) {
        // If the code is found in the map, return its corresponding index.
        if (airportCodeMap[i].code == code) return airportCodeMap[i].index;
    }
    // If the code is not found, add it to the map and the airports array.
    airportCodeMap[codeMapSize].code = code;        // Add the new code to the map.
    airportCodeMap[codeMapSize].index = airportCount; // Add the index
    codeMapSize++;                                  // Increment the size of the map.
    return airportCount++;                            // Increment the total number of airports and return the new airport's index.
}

// Function to read airport data from a CSV file.
void readCSV(const string& filename) {
    ifstream file(filename); // Open the file specified by filename.
    string line;             // String to store each line read from the file.
    getline(file, line);    // Read and discard the header line of the CSV file.

    // Read the file line by line.
    while (getline(file, line)) {
        string originCode, destCode, originCity, destCity; // Strings to store data extracted from each line.
        int distance, cost;                                 // Integers to store distance and cost.
        size_t pos = 0;                                   // size_t to store position of ','

        // Extract origin airport code.
        pos = line.find(',');              // Find the first comma.
        originCode = line.substr(0, pos);  // Extract the substring before the comma.
        line.erase(0, pos + 1);            // Erase the extracted part and the comma.

        // Extract destination airport code.
        pos = line.find(',');              // Find the next comma.
        destCode = line.substr(0, pos);    // Extract the substring.
        line.erase(0, pos + 1);            // Erase.

        // Extract origin city (handles quoted city names).
        pos = line.find('"', 1);              // Find the position of the second quote.
        originCity = line.substr(1, pos - 1);  // Extract the city name.
        line.erase(0, pos + 2);            // Erase.

        // Extract destination city (handles quoted city names).
        pos = line.find('"', 1);
        destCity = line.substr(1, pos - 1);
        line.erase(0, pos + 2);

        // Extract distance.
        pos = line.find(',');
        distance = stoi(line.substr(0, pos)); // Convert the substring to an integer.
        line.erase(0, pos + 1);

        // Extract cost.
        cost = stoi(line);                   // Convert the remaining part of the line to an integer.

        // Get the indices of the origin and destination airports.
        int originIdx = getAirportIndex(originCode);
        int destIdx = getAirportIndex(destCode);

        // Store origin airport data.
        airports[originIdx].code = originCode;
        airports[originIdx].city = originCity;
        airports[originIdx].edges.push_back({ destIdx, distance, cost }); // Add the edge to the origin airport's edge list.

        // Store destination airport data if it hasn't been stored yet.
        if (airports[destIdx].code.empty()) {
            airports[destIdx].code = destCode;
            airports[destIdx].city = destCity;
        }
    }
    file.close(); // Close the file.
}

//==================== TASK 2 ====================//
// Dijkstra's algorithm for shortest path (minimizing distance)
void findShortestPath(const string& originCode, const string& destCode) {
    int origin = -1, destination = -1; // Integers to store the indices of the origin and destination airports.

    // Find the indices of the origin and destination airports.
    for (int i = 0; i < airportCount; i++) {
        if (airports[i].code == originCode) origin = i;       //If found, set origin
        if (airports[i].code == destCode) destination = i; //If found, set destination
    }

    // If either the origin or destination airport is not found, print "None".
    if (origin == -1 || destination == -1) {
        cout << "Shortest route from " << originCode << " to " << destCode << ": None" << endl;
        return;
    }

    int dist[MAX_AIRPORTS], cost[MAX_AIRPORTS], prev[MAX_AIRPORTS]; // Arrays to store distances, costs, and predecessors.
    bool visited[MAX_AIRPORTS] = { false };                      // Array to track visited airports.

    // Initialize distances and costs to infinity, and predecessors to -1.
    for (int i = 0; i < airportCount; i++) {
        dist[i] = INF;  //Set the distance to INF
        cost[i] = INF;  //Set the cost to INF
        prev[i] = -1; //Set previous to -1
    }

    // Distance and cost from the origin to itself are 0.
    dist[origin] = 0;
    cost[origin] = 0;

    // Iterate through all airports to find the shortest paths.
    for (int count = 0; count < airportCount - 1; count++) {
        int u = -1;         // Integer to store the index of the current airport with the minimum distance.
        int minDist = INF; // Integer to store the minimum distance found so far.

        // Find the unvisited airport with the smallest distance.
        for (int i = 0; i < airportCount; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i]; // Update minimum distance
                u = i;           // Update the index
            }
        }

        // If no unvisited airport is reachable, break the loop.
        if (u == -1) break;
        visited[u] = true; // Mark the current airport as visited.

        // Update the distances and costs of the adjacent airports.
        for (int j = 0; j < airports[u].edges.size(); j++) {
            Edge edge = airports[u].edges[j]; // Get the edge
            int v = edge.destination;     // Get the destination
            // If a shorter path to v is found, update its distance, cost, and predecessor.
            if (!visited[v] && dist[u] + edge.distance < dist[v]) {
                dist[v] = dist[u] + edge.distance; // Update distance
                cost[v] = cost[u] + edge.cost;     // Update cost
                prev[v] = u;                       // Update previous
            }
        }
    }

    // If the destination is not reachable, print "None".
    if (dist[destination] == INF) {
        cout << "Shortest route from " << originCode << " to " << destCode << ": None" << endl;
        return;
    }

    // Reconstruct the shortest path.
    vector<int> path;       // Vector to store the indices of the airports in the shortest path.
    int at = destination; // Integer to track the current airport while backtracking.
    while (at != -1) {
        path.insert(path.begin(), at); // Insert the current airport at the beginning of the path.
        at = prev[at];                 // Move to the previous airport in the path.
    }

    // Print the shortest path, its length, and its cost.
    cout << "Shortest route from " << originCode << " to " << destCode << ": ";
    for (int i = 0; i < path.size(); i++) {
        cout << airports[path[i]].code; // Print the airport code.
        if (i != path.size() - 1)
            cout << " -> ";
    }
    cout << ". The length is " << dist[destination] << ". The cost is $" << cost[destination] << "." << endl;
}

//==================== TASK 3 ====================//
// Find all shortest paths from origin to all airports in a specific state (city substring)
void usingState(const string& originCode, const string& destCity) {
    string destState = destCity.substr(destCity.length() - 2, 2); // Extract the state abbreviation (last two characters of destCity).
    cout << "\nShortest route from " << originCode << " to " << destState << " state airports are: " << endl;
    cout << "\n" << left << setw(30) << "Path" << setw(10) << "Length" << setw(10) << "Cost" << endl;
    int origin = -1;                                          // Integer to store the index of the origin airport.
    vector<int> destination;                                 // Vector to store indices of airports in the destination state.
    // Find the index of the origin airport and the indices of airports in the destination state.
    for (int i = 0; i < airportCount; ++i) {
        if (airports[i].code == originCode) origin = i;
        if (airports[i].city.substr(airports[i].city.length() - 2, 2) == destState) destination.push_back(i);
    }

    // Iterate through each destination airport in the specified state.
    for (int count : destination) {
        vector<int> dist(airportCount, INF), cost(airportCount, INF), prev(airportCount, -1); // Initialize distance, cost, and predecessor arrays.
        vector<bool> visited(airportCount, false);                                         // Initialize visited array.
        dist[origin] = cost[origin] = 0;                                                  // Set distance and cost from origin to itself to 0.

        // Dijkstra's algorithm to find the shortest path to the current destination airport.
        for (int step = 0; step < airportCount - 1; ++step) {
            int u = -1, minDist = INF;
            for (int i = 0; i < airportCount; ++i)
                if (!visited[i] && dist[i] < minDist) minDist = dist[i], u = i;

            if (u == -1) break;
            visited[u] = true;

            for (const Edge& edge : airports[u].edges) {
                int v = edge.destination;
                if (!visited[v] && dist[u] + edge.distance < dist[v]) {
                    dist[v] = dist[u] + edge.distance;
                    cost[v] = cost[u] + edge.cost;
                    prev[v] = u;
                }
            }
        }
        // If the destination airport is not reachable, print "None".
        if (dist[count] == INF) {
            cout << "Shortest route from " << originCode << " to " << airports[count].code << ": None" << endl;
            continue;
        }
        else {
            // Reconstruct the shortest path.
            vector<int> path;
            for (int at = count; at != -1; at = prev[at])
                path.insert(path.begin(), at);
            string pathstr;
            for (int i = 0; i < path.size(); i++) {
                pathstr += airports[path[i]].code;
                if (i != path.size() - 1)
                    pathstr += "->";
            }
            // Print the shortest path, its length, and its cost.
            cout << left << setw(30) << pathstr << setw(10) << dist[count] << setw(10) << cost[count] << endl;
        }
    }

}
//==================== TASK 4 ====================//
// Find shortest path with exact number of stops
void dfs(int current, int destination, int stopsLeft, int distanceSoFar,
    int costSoFar, int& distance, int& cost, vector<bool>& visited, vector<int>& path,
    vector<int>& bestPath) {
    // If the number of stops left is negative, return (backtrack).
    if (stopsLeft < 0) return;

    visited[current] = true;     // Mark the current airport as visited.
    path.push_back(current);      // Add the current airport to the current path.

    // If the destination is reached and the required number of stops is met.
    if (current == destination && stopsLeft == 0) {
        // If the current path is shorter than the best path found so far.
        if (distanceSoFar < distance) {
            distance = distanceSoFar; // Update the shortest distance.
            cost = costSoFar;         // Update the corresponding cost.
            bestPath = path;          // Update the best path.
        }
    }
    else {
        // Explore all neighbors of the current airport.
        for (const Edge& edge : airports[current].edges) {
            int next = edge.destination; // Get the destination airport of the edge.
            // If the neighbor has not been visited, recursively call dfs.
            if (!visited[next]) {
                dfs(next, destination, stopsLeft - 1, distanceSoFar + edge.distance, costSoFar + edge.cost, distance, cost, visited, path, bestPath);
            }
        }
    }

    path.pop_back();       // Remove the current airport from the path (backtrack).
    visited[current] = false; // Mark the current airport as unvisited.
}
void withNoOfStops(const string& originCode, const string& destCode, int stops) {
    int origin = -1, destination = -1; // Integers to store the indices of the origin and destination airports.
    // Find the indices of the origin and destination airports.
    for (int i = 0; i < airportCount; i++) {
        if (airports[i].code == originCode) origin = i;
        if (airports[i].code == destCode) destination = i;
    }

    // If either the origin or destination airport is not found, print "None".
    if (origin == -1 || destination == -1) {
        cout << "\nShortest route from " << originCode << " to " << destCode << ": None" << endl;
        return;
    }

    vector<bool> visited(airportCount, false); // Array to track visited airports.
    vector<int> bestPath, path;                // Vectors to store the best path and current path.
    int distance = INF, cost = INF;            // Initialize distance and cost to infinity.

    // Call the dfs function to find the shortest path with the specified number of stops.
    dfs(origin, destination, stops + 1, 0, 0, distance, cost, visited, path, bestPath);

    // If no path with the specified number of stops is found, print "None".
    if (distance == INF)
        cout << "\nShortest route from " << originCode << " to " << destCode << " with " << stops << " stops: None";
    else {
        // Print the shortest path, its length, and its cost.
        cout << "\nShortest route from " << originCode << " to " << destCode << " with " << stops << " stops: ";
        for (int i = 0; i < bestPath.size(); i++) {
            cout << airports[bestPath[i]].code;
            if (i != bestPath.size() - 1)
                cout << "->";
        }
        cout << ". The length is " << distance << ". The cost is $" << cost << "." << endl;
    }
}

//==================== TASK 5 ====================//
// Count and display direct connections (inbound + outbound), sort manually
struct AirportConnection {
    string code;       // String to store the airport code.
    int outbound = 0;  // Integer to store the number of outbound connections.
    int inbound = 0;   // Integer to store the number of inbound connections.
    int total;       // Integer to store the total number of connections.
};

void FlightConnections() {
    AirportConnection connections[MAX_AIRPORTS]; // Array to store connection information for each airport.
    //Initialize connections
    for (int i = 0; i < airportCount; i++) {
        connections[i].outbound = airports[i].edges.size(); // Outbound connections = number of edges.
        connections[i].inbound = 0;                         // Initialize inbound to 0.
        connections[i].code = airports[i].code;           // Store the airport code.
        connections[i].total = 0;                          // Initialize total to 0.
    }
    //Counts inbound connection
    for (int i = 0; i < airportCount; i++) {
        for (int j = 0; j < airports[i].edges.size(); j++) {
            int destinationIdx = airports[i].edges[j].destination; // Get destination
            connections[destinationIdx].inbound++;                 // Increment inbound for destination.
        }
    }
    //Adds connection
    for (int i = 0; i < airportCount; i++) {
        connections[i].total = connections[i].inbound + connections[i].outbound; //Calculate total connections.
    }

    // Count and display direct connections (inbound + outbound), sort manually

    //Outer loop for sorting
    //Inner loop for comparing and swapping
    for (int i = 0; i < airportCount - 1; i++) {
        for (int j = i + 1; j < airportCount; j++) {
            if (connections[j].total > connections[i].total) {
                AirportConnection temp = connections[i];
                connections[i] = connections[j];
                connections[j] = temp;
            }
        }
    }

    cout << "Airport     Connections\n";
    for (int i = 0; i < airportCount; i++) {
        cout << "  " << connections[i].code << "            " << connections[i].total << endl;
    }
}

//==================== TASK 6 ====================//
// Create undirected graph from G following edge rules
struct UndirectedGraph {
    string code;        // Airport code
    string city;        // City name
    vector<Edge> edges; // Vector of edges
};

UndirectedGraph undirectedGraph[MAX_AIRPORTS]; // Array to store the undirected graph.

// Function to check if there is an edge from 'from' to 'to'.
bool hasEdge(int from, int to) {
    for (int i = 0; i < airports[from].edges.size(); i++) {
        if (airports[from].edges[i].destination == to) {
            return true; // Return true if edge exists
        }
    }
    return false; // Otherwise return false
}

// Function to get the cost of the edge from 'from' to 'to'.
int getCost(int from, int to) {
    for (int i = 0; i < airports[from].edges.size(); i++) {
        if (airports[from].edges[i].destination == to) {
            return airports[from].edges[i].cost; // Return the cost of the edge
        }
    }
    return INF; // Return INF if edge does not exist
}

// Function to build the undirected graph.
void buildUndirectedGraph() {
    for (int i = 0; i < airportCount; i++) {
        undirectedGraph[i].code = airports[i].code; // Copy airport code
        undirectedGraph[i].city = airports[i].city; // Copy city name
    }

    // To avoid duplicating edges
    bool added[MAX_AIRPORTS][MAX_AIRPORTS] = { false };

    for (int u = 0; u < airportCount; u++) {
        for (int i = 0; i < airports[u].edges.size(); i++) {
            int v = airports[u].edges[i].destination;
            if (!added[u][v] && !added[v][u]) {
                bool uTOv = hasEdge(u, v); // Check if edge u to v exists
                bool vTOu = hasEdge(v, u); // Check if edge v to u exists
                int cost = INF;

                // Apply edge rules
                if (uTOv && vTOu) {
                    cost = min(getCost(u, v), getCost(v, u)); // Cost is min of both directions
                }
                else if (uTOv) {
                    cost = getCost(u, v); // Cost is cost from u to v
                }
                else if (vTOu) {
                    cost = getCost(v, u); // Cost is cost from v to u
                }

                // Add undirected edge if valid
                if (cost != INF) {
                    undirectedGraph[u].edges.push_back({ v, cost, cost }); // Using 'distance' field to store 'cost'
                    undirectedGraph[v].edges.push_back({ u, cost, cost });
                    added[u][v] = added[v][u] = true;
                }
            }
        }
    }
}

//Print test
void GuGraph() {
    cout << "Graph G_u:\n";
    for (int i = 0; i < airportCount; i++) {
        cout << undirectedGraph[i].code << " -> ";
        for (int j = 0; j < undirectedGraph[i].edges.size(); j++) {
            int destIdx = undirectedGraph[i].edges[j].destination;
            int cost = undirectedGraph[i].edges[j].cost;
            cout << "(" << undirectedGraph[destIdx].code << ",$" << cost << ")";
            if (j < undirectedGraph[i].edges.size() - 1)
                cout << ",";
        }
        cout << "\n" << endl;
    }
}

//==================== TASK 7 ====================//
// Prim's MST on G_u
void primMST() {
    bool inMST[MAX_AIRPORTS] = { false }; // Array to track airports included in the MST.
    int key[MAX_AIRPORTS], parent[MAX_AIRPORTS];  // Arrays to store the minimum edge weights and parent nodes.

    // Initialize key values to infinity and parents to -1.
    for (int i = 0; i < airportCount; i++) {
        key[i] = INF;
        parent[i] = -1;
    }

    key[0] = 0; // Start with the first airport.

    // Iterate until all airports are included in the MST.
    for (int count = 0; count < airportCount - 1; count++) {
        int u = -1;         // Integer to store the index of the airport with the minimum key value.
        int minKey = INF; // Integer to store the minimum key value.
        // Find the airport with the minimum key value among the unvisited airports.
        for (int v = 0; v < airportCount; v++) {
            if (!inMST[v] && key[v] < minKey) {
                minKey = key[v];
                u = v;
            }
        }

        if (u == -1) break; // If no such airport is found, break.
        inMST[u] = true;     // Include the selected airport in the MST.

        // Update the key values and parent nodes of the adjacent airports.
        for (int j = 0; j < undirectedGraph[u].edges.size(); j++) {
            int v = undirectedGraph[u].edges[j].destination;
            int weight = undirectedGraph[u].edges[j].cost;
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
            }
        }
    }

    // Print the edges of the MST and calculate the total cost.
    cout << "\nPrim's MST Edges:\n";
    int totalCost = 0;
    for (int i = 1; i < airportCount; i++) {
        if (parent[i] != -1) {
            cout << undirectedGraph[parent[i]].code << " - " << undirectedGraph[i].code << " ($" << key[i] << ")\n";
            totalCost += key[i];
        }
    }
    cout << "Total MST cost: $" << totalCost << "\n";
}

//==================== TASK 8 ====================//
// Kruskal's MST (with manual disjoint set)
struct KruskalEdge {
    int u, v, cost; // Struct to represent an edge with its endpoints and cost.
};
KruskalEdge edges[MAX_AIRPORTS * MAX_AIRPORTS]; // Array to store all edges.
int edgeCount = 0;                             // Integer to store the number of edges.
int parentSet[MAX_AIRPORTS];                   // Array for the disjoint set data structure.
// Function to find the set an element belongs to.
int findSet(int i) {
    while (i != parentSet[i]) i = parentSet[i]; // Path compression
    return i;
}
// Function to merge two sets.
void unionSet(int u, int v) {
    int setU = findSet(u);
    int setV = findSet(v);
    if (setU != setV) parentSet[setU] = setV; // Merge the sets if they are different.
}
// Function to implement Kruskal's algorithm.
void kruskalMST() {
    edgeCount = 0;
    // Collect all edges from the undirected graph.
    for (int u = 0; u < airportCount; u++) {
        for (int j = 0; j < undirectedGraph[u].edges.size(); j++) {
            int v = undirectedGraph[u].edges[j].destination;
            if (u < v) { // Avoid duplicate edges
                edges[edgeCount++] = { u, v, undirectedGraph[u].edges[j].cost };
            }
        }
    }

    // Manual sort (selection sort)
    for (int i = 0; i < edgeCount - 1; i++) {
        for (int j = i + 1; j < edgeCount; j++) {
            if (edges[i].cost > edges[j].cost) {
                KruskalEdge temp = edges[i];
                edges[i] = edges[j];
                edges[j] = temp;
            }
        }
    }

    // Initialize the disjoint set.
    for (int i = 0; i < airportCount; i++) parentSet[i] = i;

    // Build the MST.
    cout << "\nKruskal's MST Edges:\n";
    int totalCost = 0, count = 0;
    for (int i = 0; i < edgeCount && count < airportCount - 1; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        // If the endpoints of the edge belong to different sets, include the edge in the MST.
        if (findSet(u) != findSet(v)) {
            unionSet(u, v); // Merge the sets.
            cout << undirectedGraph[u].code << " - " << undirectedGraph[v].code << " ($" << edges[i].cost << ")\n";
            totalCost += edges[i].cost; // Add the cost to the total.
            count++;                   // Increment the edge count.
        }
    }
    cout << "Total MST cost: $" << totalCost << "\n";
}

int main() {
    // EDIT FILE PATH FOR NEW COMPUTER
    //Here is template

    //readCSV("C:FILEPATH put the file path here to use .csv file\\airports.csv");
    readCSV("airports.txt");
    // Task 2 test
    //Change "ABE" and "DTW" to your desired airports for different results.
    cout << "Task 2" << endl;
    cout << "\n";
    findShortestPath("ABE", "DTW");
    findShortestPath("ABQ", "IAH");
    findShortestPath("ABE", "MIA");
    findShortestPath("ABE", "EYW");

    cout << "\n";

    //Task 3 test
    cout << "Task 3" << endl;
    usingState("ABE", "Miami, FL");

    cout << "\n";

    // Task 4 test
    //Change numbers at end to adjust how many stops
    cout << "Task 4" << endl;
    withNoOfStops("ALB", "AUS", 2);
    cout << "\n";

    // Task 5 test
    cout << "Task 5" << endl;
    cout << "\n";
    FlightConnections();

    cout << "\n";

    // Task 6 test
    buildUndirectedGraph();

    // Task 7 test
    cout << "Task 7" << endl;
    primMST();

    cout << "\n";

    //Task 8 test
    cout << "Task 8" << endl;
    kruskalMST();

    return 0;
}
