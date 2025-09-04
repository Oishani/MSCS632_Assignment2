#include <bits/stdc++.h>
#include <thread>
#include <chrono>
using namespace std;

static bool LEAK = true; // set false for NO-LEAK run

size_t rss_kb() {
    ifstream f("/proc/self/status");
    string s; while (getline(f, s)) {
        if (s.rfind("VmRSS:", 0) == 0) {
            istringstream iss(s);
            string k; size_t kb; string unit; iss >> k >> kb >> unit; return kb;
        }
    }
    return 0;
}
void print_rss(const string& label, size_t baseline) {
    size_t now = rss_kb();
    long long delta = (long long)now - (long long)baseline;
    cout << label << ": " << now << " kB (Δ from baseline: " << delta << " kB)\n";
}

int main() {
    ios::sync_with_stdio(false);
    cout << "=== C++: manual memory + RSS ===\n";
    size_t base = rss_kb();
    cout << "RSS baseline: " << base << " kB\n";

    // ---- Scenario A: allocate then free ----
    auto tA = chrono::steady_clock::now();
    size_t N = 64 * 1024 * 1024;                   // ~64MB
    char* buf = new char[N];
    memset(buf, 0xAA, N);                          // commit pages
    print_rss("After alloc (A)", base);

    delete[] buf;                                  // manual free
    this_thread::sleep_for(chrono::milliseconds(50));
    print_rss("After delete (A)", base);
    cout << "Elapsed (A): " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now()-tA).count() << " ms\n\n";

    // ---- Scenario B: LEAK vs NO-LEAK in a scope ----
    auto tB = chrono::steady_clock::now();
    {
        char* leak = new char[N];
        memset(leak, 0xAA, N);
        print_rss("After alloc (B)", base);

        if (!LEAK) { delete[] leak; }              // NO-LEAK path
        else { /* intentionally leak: do not delete */ }
    }
    this_thread::sleep_for(chrono::milliseconds(50));
    print_rss("After scope (B)", base);
    cout << "Elapsed (B): " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now()-tB).count() << " ms\n";

    // ---- ASan UAF demo ----
    // int* q = new int(7); delete q; cout << *q << "\n"; // <-- uncomment to trigger UAF

    cout << "Note: Prefer RAII (std::unique_ptr) to make frees automatic.\n";
    return 0;
}
