#include <bits/stdc++.h>
using namespace std;

#define NO_OF_CHARS 256

// Preprocessing for the strong good suffix rule
void preprocess_strong_suffix(vector<int>& shift, vector<int>& bpos, const string& pat, int len_pat)
{
    int id_current_prefix = len_pat, id_current_suffix = len_pat + 1;
    bpos[id_current_prefix] = id_current_suffix;

    while (id_current_prefix > 0) {
        while (id_current_suffix <= len_pat && pat[id_current_prefix - 1] != pat[id_current_suffix - 1]) {
            if (shift[id_current_suffix] == 0) {
                shift[id_current_suffix] = id_current_suffix - id_current_prefix; 
            }
            id_current_suffix = bpos[id_current_suffix];
        }
        id_current_prefix--;
        id_current_suffix--;
        bpos[id_current_prefix] = id_current_suffix; 
    }
}

// Preprocessing for case 2 and case 3
void preprocess_case_2_3(vector<int>& shift, vector<int>& bpos, const string& pat, int m)
{
    int j = bpos[0];
    for (int i = 0; i <= m; i++) {
        if (shift[i] == 0)
            shift[i] = j;
        if (i == j)
            j = bpos[j]; 
    }
}

// Boyer-Moore Bad Character Heuristic
void badCharHeuristic(string str, int size, int badchar[NO_OF_CHARS])
{
    int i;
    for (i = 0; i < NO_OF_CHARS; i++)
        badchar[i] = -1;
    for (i = 0; i < size; i++)
        badchar[(int)str[i]] = i;
}

// Boyer-Moore Search Algorithm
vector<int> boyerMooreSearch(string txt, string pat) {
    int m = pat.size();
    int n = txt.size();
    int badchar[NO_OF_CHARS];
    vector<int> bpos(m + 1), shift(m + 1, 0);
    vector<int> result;

    badCharHeuristic(pat, m, badchar);
    preprocess_strong_suffix(shift, bpos, pat, m);
    preprocess_case_2_3(shift, bpos, pat, m);
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pat[j] == txt[s + j]) j--;
        if (j < 0) {
            result.push_back(s);  
            s += max(shift[0], (s + m < n) ? m - badchar[txt[s + m]] : 1);
        } else {
            s += max(shift[j + 1], max(1, j - badchar[txt[s + j]]));
        }
    }
    return result;
}

// KMP Preprocessing (LPS array)
void computeLPSArray(const string& pat, vector<int>& lps) {
    int m = pat.size();
    int length = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pat[i] == pat[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// KMP Search Function
vector<int> kmpSearch(const string& txt, const string& pat) {
    int n = txt.size();
    int m = pat.size();
    vector<int> lps(m);
    vector<int> result;
    computeLPSArray(pat, lps);

    int i = 0, j = 0;
    while (i < n) {
        if (pat[j] == txt[i]) {
            i++;
            j++;
        }
        if (j == m) {
            result.push_back(i - j);
            j = lps[j - 1];
        } else if (i < n && pat[j] != txt[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    return result;
}

// Naive Search Function
vector<int> naiveSearch(const string& txt, const string& pat) {
    int n = txt.size();
    int m = pat.size();
    vector<int> result;

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && txt[i + j] == pat[j]) {
            j++;
        }
        if (j == m) {
            result.push_back(i);  // Pattern found at index i
        }
    }
    return result;
}

// Function to generate random test case
void generateRandomTest(string &txt, string &pat, int n, int m) {
    string chars = "ABABCD";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, chars.size() - 1);

    // Generate random text of length n
    txt.clear();
    for (int i = 0; i < n; i++) {
        txt += chars[dis(gen)];
    }

    // Generate random pattern of length m
    pat.clear();
    for (int i = 0; i < m; i++) {
        pat += chars[dis(gen)];
    }
}

// Function to measure and compare the execution time of all three algorithms
void measureSearchTimes(int K, int n, int m, string sample_chars) {
    double naiveTime = 0.0, boyerMooreTime = 0.0, kmpTime = 0.0;

    for (int i = 0; i < K; i++) {
        string txt, pat;
        generateRandomTest(txt, pat, n, m);  // Generate random text and pattern

        // Measure time for Naive Search
        auto start = chrono::high_resolution_clock::now();
        naiveSearch(txt, pat);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> naiveDuration = end - start;
        naiveTime += naiveDuration.count();

        // Measure time for Boyer-Moore Search
        start = chrono::high_resolution_clock::now();
        boyerMooreSearch(txt, pat);
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> boyerMooreDuration = end - start;
        boyerMooreTime += boyerMooreDuration.count();

        // Measure time for KMP Search
        start = chrono::high_resolution_clock::now();
        kmpSearch(txt, pat);
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> kmpDuration = end - start;
        kmpTime += kmpDuration.count();
    }

    // Calculate mean times
    cout << "Average Naive Search Time over " << K << " loops: " << naiveTime / K << " seconds" << endl;
    cout << "Average Boyer-Moore Search Time over " << K << " loops: " << boyerMooreTime / K << " seconds" << endl;
    cout << "Average KMP Search Time over " << K << " loops: " << kmpTime / K << " seconds" << endl;
}

int main() {
    int K = 5;    // Number of iterations to measure
    int n = 10000; // Length of text
    int m = 8;    // Length of pattern
    string sample_chars = "ABABCD";  // Characters for generating text

    measureSearchTimes(K, n, m, sample_chars);

    return 0;
}
