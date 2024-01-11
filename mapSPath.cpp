#include "header/mainwindow.h"

// 将角度转换为弧度
double MainWindow::toRadians(double degrees) { return degrees * (M_PI / 180.0); }

// 使用 Haversine 公式计算两个经纬度点之间的距离
double MainWindow::calcuDist(double lat1, double lon1, double lat2, double lon2) {
    const double EARTH_RADIUS_KM = 6371.0;  // 地球半径（单位：千米）
    double dLat = toRadians(lat2 - lat1);
    double dLon = toRadians(lon2 - lon1);
    double a = sin(dLat / 2) * sin(dLat / 2) + cos(toRadians(lat1)) * cos(toRadians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_KM * c;  // 球面上的距离（单位：千米）
}

double MainWindow::updateShortestPath(dijkstra_tag) {
    // 使用dijkstra算法计算最短路径
    // selectedVertex.front() -> selectedVertex.back()
    // 将最短路径上的顶点加入visitedVertex

    int n = adj.size();
    ll src = selectedVertex.front();
    ll dest = selectedVertex.back();

    map<ll, ll> prev;
    map<ll, double> dist;
    map<ll, bool> visited;
    priority_queue<pdll, vector<pdll>, greater<pdll>> pq;
    for (const auto &[u, _] : adj) {
        prev[u] = u;
        dist[u] = 1e9;
        visited[u] = false;
    }

    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (u == dest) break;
        if (visited[u]) continue;
        visited[u] = true;
        for (auto v : adj[u]) {
            if (visited[v]) continue;
            double w = calcuDist(nodes[u].lat, nodes[u].lon, nodes[v].lat, nodes[v].lon);
            if (dist[v] > dist[u] + w) {
                prev[v] = u;
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    ll cur = dest;
    visitedVertex.insert(src);
    while (prev[cur] != cur) {
        visitedVertex.insert(cur);
        cur = prev[cur];
    }
    return dist[dest];
}

double MainWindow::updateShortestPath(AStar_tag) {
    // 使用A*算法计算最短路径，代价=距离+启发式函数
    // selectedVertex.front() -> selectedVertex.back()
    // 将最短路径上的顶点加入visitedVertex

    int n = adj.size();
    ll src = selectedVertex.front();
    ll dest = selectedVertex.back();

    map<ll, ll> prev;
    map<ll, double> dist;
    map<ll, bool> visited;
    priority_queue<pdll, vector<pdll>, greater<pdll>> pq;
    for (const auto &[u, _] : adj) {
        prev[u] = u;
        dist[u] = 1e9;
        visited[u] = false;
    }

    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (u == dest) break;
        if (visited[u]) continue;
        visited[u] = true;
        for (auto v : adj[u]) {
            if (visited[v]) continue;
            double w = calcuDist(nodes[u].lat, nodes[u].lon, nodes[v].lat, nodes[v].lon);
            if (dist[v] > dist[u] + w) {
                prev[v] = u;
                dist[v] = dist[u] + w;
                double h = calcuDist(nodes[v].lat, nodes[v].lon, nodes[dest].lat, nodes[dest].lon);
                pq.push({dist[v] + h, v});
            }
        }
    }

    ll cur = dest;
    visitedVertex.insert(src);
    while (prev[cur] != cur) {
        visitedVertex.insert(cur);
        cur = prev[cur];
    }
    return dist[dest];
}