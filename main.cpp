#include <utility>
#include <unordered_map>
#include <queue>
#include <vector>
#include <iostream>

using std::pair;
using std::make_pair;
using std::hash;
using std::unordered_map;
using std::priority_queue;
using std::vector;

typedef pair<int, int> PII;
typedef pair<unsigned, unsigned> PUU;

const int mod = 1000000007;

struct TreeNode {
    TreeNode *left, *right;
    unsigned val;
};

struct khash {
    unsigned operator()(const PUU &k) const {
        return hash<unsigned>()(k.first) ^ (hash<unsigned>()(k.second) << 1);
    }
};

unordered_map<PUU, unsigned, khash> id;

unsigned GetId(int a, int b) {
    if (id.find(PUU(a, b)) == id.end()) {
        unsigned s = id.size();
        return id[PUU(a, b)] = s;
    }
    return id[PUU(a, b)];
}

TreeNode *MakeNode(TreeNode *a, TreeNode *b) {
    return new TreeNode({a, b, GetId(a->val, b->val)});
}

pair<TreeNode *, bool> AddPowerOf2(TreeNode *u, int b, int s = 128 * 1024) {
    if (s == 32)
        return make_pair(new TreeNode({0, 0, u->val + (1 << b)}), u->val > u->val + (1 << b));
    if (b < s / 2) {
        pair<TreeNode *, bool> r = AddPowerOf2(u->left, b, s / 2);
        if (r.second) {
            pair<TreeNode *, bool> q = AddPowerOf2(u->right, 0, s / 2);
            return make_pair(MakeNode(r.first, q.first), q.second);
        }
        return make_pair(MakeNode(r.first, u->right), 0);
    } else {
        pair<TreeNode *, bool> r = AddPowerOf2(u->right, b - s / 2, s / 2);
        return make_pair(MakeNode(u->left, r.first), r.second);
    }
}

long long powers_of_2[64 * 1024 + 3];

long long GetActualLength(TreeNode *u, int s = 128 * 1024) {
    if (s == 32) return u->val % mod;
    return (GetActualLength(u->left, s / 2) + GetActualLength(u->right, s / 2) * powers_of_2[s / 2]) % mod;
}

TreeNode *ZeroNode(int s = 128 * 1024) {
    if (s == 32)
        return new TreeNode({0, 0, 0});
    TreeNode *v = ZeroNode(s / 2);
    return new TreeNode({v, v, 0});
}

bool Less(const TreeNode *a, const TreeNode *b) {
    if (a->val == b->val) return 0;
    if (!a->left) return a->val < b->val;
    if (a->right->val == b->right->val) return Less(a->left, b->left);
    return Less(a->right, b->right);
}

const int MAXN = 100010;

vector<PII> AdjList[MAXN];
TreeNode *Dist[MAXN];
bool Visited[MAXN];
bool Reach[MAXN];
int Parent[MAXN];

struct CompareByKey {
    bool operator()(pair<TreeNode *, int> a, pair<TreeNode *, int> b) {
        return Less(b.first, a.first);
    }
};

priority_queue<pair<TreeNode *, int>, vector<pair<TreeNode *, int>>, CompareByKey> Heap;

void check(int p, int u, TreeNode *new_dist) {
    if (Visited[u]) return;
    if (!Reach[u] || Less(new_dist, Dist[u])) {
        Reach[u] = 1;
        Parent[u] = p;
        Dist[u] = new_dist;
        Heap.push(make_pair(new_dist, u));
    }
}

int main() {
    id[PII(0, 0)] = 0;
    powers_of_2[0] = 1;
    for (size_t i = 1; i < 64 * 1024 + 3; ++i) {
        powers_of_2[i] = powers_of_2[i - 1] * 2 % mod;
    }

    int node_count, edge_count;
    std::cin >> node_count;
    std::cin >> edge_count;
    for (size_t edge_ind = 0; edge_ind < edge_count; ++edge_ind) {
        int tail, arrow, log_cost;
        std::cin >> tail;
        std::cin >> arrow;
        std::cin >> log_cost;
        AdjList[tail].push_back(PII(arrow, log_cost));
        AdjList[arrow].push_back(PII(tail, log_cost));
    }
    int s, t;
    std::cin >> s;
    std::cin >> t;

    check(0, s, ZeroNode());
    while (!Heap.empty()) {
        int u = Heap.top().second;
        Heap.pop();
        if (Visited[u]) continue;
        Visited[u] = 1;
        for (PII e : AdjList[u])
            check(u, e.first, AddPowerOf2(Dist[u], e.second).first);
    }

    if (!Reach[t]) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << GetActualLength(Dist[t]) << std::endl;
        vector<int> path;
        int u = t;
        while (u) {
            path.push_back(u);
            u = Parent[u];
        }
        std::cout << path.size() << std::endl;
        while (!path.empty()) {
            std::cout << path.back() << ' ';
            path.pop_back();
        }
        std::cout << std::endl;
    }

    return 0;
}
