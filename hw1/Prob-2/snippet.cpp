int solve() {
    for (int i = 0; i < n + 1; ++i) {
        G[0][i] = G[i][0] = i;
    }

    for (int i = 1; i < n + 1; ++i) {
        int cost = n + G[i-1][0];
        int rev = n;
        for (int j = 1; j < n + 1; ++j) {
            int s = X[i] == Y[j] ? 0 : 1;
            G[i][j] = std::min(G[i-1][j-1] + s, G[i-1][j] + 1);
            --rev;
            if (!(cost < G[i][j]+rev)) {
                cost = G[i][j]+rev;
            }
            G[i][j] = cost - (n-j);
        }
    }
    return G[n][n];
}
