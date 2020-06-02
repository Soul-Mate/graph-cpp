#include <iostream>
#include <vector>
#include <set>
#include <exception>
#include <memory>

// graph data structure use map
class GraphMap {
public:
    GraphMap(int vertexCount, std::vector<std::vector<int>> edges) throw();

    ~GraphMap() = default;

    std::vector<int> adjacency(int v) const;

    inline int V() const { return v_; };

private:
    int v_;
    std::vector<std::set<int>> g_;
};

GraphMap::GraphMap(int vertexCount, std::vector<std::vector<int>> edges) throw() {
    v_ = vertexCount;
    g_ = std::vector<std::set<int>>(vertexCount, std::set<int>());

    for (auto it = edges.begin(); it != edges.end(); it++) {
        auto from = (*it)[0];
        if (from < 0 && from >= vertexCount) {
            throw std::logic_error("invalid vertex");
        }

        int to = (*it)[1];
        if (to < 0 && to >= vertexCount) {
            throw std::logic_error("invalid vertex");
        }


        std::set<int> &set = g_[from];

        // parallel edge
        if (g_[from].find(to) != g_[from].end()) {
            throw std::logic_error("not support parallel edge");
        }

        // self-loop edge
        if (to == from) {
            throw std::logic_error("not support self-loop edge");
        }

        g_[from].insert(to);
        g_[to].insert(from);
    }
}

std::vector<int> GraphMap::adjacency(int v) const {
    std::vector<int> adj;
    std::set<int> gset = g_[v];
    for (auto it = gset.begin(); it != gset.end(); it++) {
        adj.push_back(*it);
    }

    return std::move(adj);
}

class HamiltonPath {
public:
    explicit HamiltonPath(std::shared_ptr<GraphMap> g);

    ~HamiltonPath() = default;

    bool operator()(int s);

private:

    bool dfs(int s, int v);

    bool all_visited();

    std::shared_ptr<GraphMap> g_;

    std::vector<bool> visited_;
};

HamiltonPath::HamiltonPath(std::shared_ptr<GraphMap> g) {
    g_ = g;
    visited_ = std::vector<bool>(g_->V(), false);
}

bool HamiltonPath::operator()(int s) {
    return dfs(s, s);
}

bool HamiltonPath::dfs(int s, int v) {
    visited_[v] = true;

    std::vector<int> adj = g_->adjacency(v);
    for (int w :  adj) {
        if (!visited_[w]) {
            if (dfs(s, w)) {
                return true;
            }

        } else if (w == s && all_visited()) {
            return true;
        }
    }

    visited_[v] = false;
    return false;
}

bool HamiltonPath::all_visited() {
    for (bool v : visited_) {
        if (!v) {
            return false;
        }
    }

    return true;
}


int main() {
    int v = 4;
    std::vector<std::vector<int>> edges;
    edges.push_back({0, 1});
    edges.push_back({0, 2});
    edges.push_back({0, 3});
    edges.push_back({1, 2});
    edges.push_back({1, 3});
    try {
        std::shared_ptr<GraphMap> g = std::make_shared<GraphMap>(v, edges);
        HamiltonPath hamiltonLoop(g);
        std::cout << "has hamilton loop? " << (hamiltonLoop(0) == 1 ? "true" : " false") << std::endl;
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}
