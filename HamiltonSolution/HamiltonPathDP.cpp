#include <iostream>
#include <vector>
#include <set>
#include <exception>
#include <memory>
#include <algorithm>

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
    explicit HamiltonPath(std::shared_ptr<GraphMap> g) throw();

    ~HamiltonPath() = default;

    std::vector<int> operator()(int s, int t);

private:

    void dfs(int s, int t, int p);

    bool all_visited();

    std::shared_ptr<GraphMap> g_;

    uint64_t visited_;

    std::vector<int> pre_;

    int path_end_;
};

HamiltonPath::HamiltonPath(std::shared_ptr<GraphMap> g) throw()
        : g_(g),
          visited_(0),
          pre_(std::vector<int>(g_->V(), -1)),
          path_end_(-1) {
    if (g_->V() > sizeof visited_ * 8) {
        throw std::overflow_error("vertex more than state compression range");
    }

}

std::vector<int> HamiltonPath::operator()(int s, int t) {
    std::vector<int> paths;

    // start dfs, target vertex is s, start vertex is s and vertex s parent is himself
    dfs(s, t, s);
    if (path_end_ == -1) {
        return paths;
    }

    paths.push_back(path_end_);
    for (auto cur = pre_[path_end_]; cur != s; cur = pre_[cur]) {
        paths.push_back(cur);
    }
    paths.push_back(s);

    std::reverse(paths.begin(), paths.end());

    return paths;
}

void HamiltonPath::dfs(int s, int t, int p) {
    visited_ = visited_ | (1 << s);
    pre_[s] = p;
    if (s == t && all_visited()) {
        path_end_ = s;
        return;
    }

    std::vector<int> adj = g_->adjacency(s);
    for (int w :  adj) {
        if ((visited_ & (1 << w)) == 0) {
            dfs(w, t, s);
            if (path_end_ != -1) {
                return;
            }

        } else if (w == t && all_visited()) {
            path_end_ = w;
            return;
        }
    }
    pre_[s] = -1;
    visited_ = visited_ - (1 << s);
//    visited_[s] = false;
}

bool HamiltonPath::all_visited() {
    for (int i = 0; i < g_->V(); i++) {
        if ((visited_ & (1 << i)) == 0) {
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
        HamiltonPath hamiltonPath(g);
        std::vector<int> paths = hamiltonPath(0, 3);

        std::cout << "Hamilton Path: ";
        for (auto it = paths.begin(); it != paths.end(); it++) {
            std::cout << *it;
            if (it != paths.end() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}
