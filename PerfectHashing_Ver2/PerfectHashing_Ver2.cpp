#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <list>
#include <windows.h>
#include <psapi.h>
#include <functional>

using namespace std;

struct Value {
    string key;
    string val;
};

class PerfectHashing {
private:
    vector<vector<Value>> table;
    vector<int> setAHash1;
    vector<vector<int>> setAHash2;
    vector<bool> hasLevel2Hash;
    int m = 0; // Kích thước của bảng băm cấp 1
    mt19937 gen;

    bool isSpaceOnly(string& s) {
        return all_of(s.begin(), s.end(), [](char ch) {
            return isspace(static_cast<unsigned char>(ch));
            });
    }

    size_t findTableSize(int size) { // Tìm kích thước bảng băm phù hợp
        size_t res = 1;
        int n = 0;
        while (res <= size) {
            res = res << 1;
            n++;
        }
        return res;
    }

    int universalHash(const string& key, const vector<int>& setA, int mod) {
        long long sum = 0;
        int len = min(key.size(), setA.size());
        for (int i = 0; i < len; ++i) {
            int val = static_cast<int>(key[i]);
            if (val < 0 || val > 127) return -1;
            sum += val * setA[i];
        }
        return sum % mod;
    }

public:
    PerfectHashing() : gen(random_device{}()) {}

    void build(const string& path) {
        vector<Value> data;
        readFile(path, data);

        m = findTableSize(data.size());
        table.assign(m, vector<Value>());

        // Tạo ngẫu nhiên một tập hợp A1 với độ dài bằng key dài nhất 
        size_t maxKeyLength = 0;
        for (const auto& item : data) {
            maxKeyLength = max(maxKeyLength, item.key.length());
        }
        setAHash1.resize(maxKeyLength);
        uniform_int_distribution<> dis(0, m - 1);
        for (int& num : setAHash1) {
            num = dis(gen);
        }

        // Hash cấp 1
        for (const auto& cur : data) {
            int index = universalHash(cur.key, setAHash1, m);
            if (index < 0) continue;
            table[index].push_back(cur);
        }

        // Đặt cờ kiểm tra việc có xảy ra hàm băm cấp 2 hay không
        hasLevel2Hash.resize(m, false);
        // Hash cấp 2
        setAHash2.resize(m);

        for (int i = 0; i < m; ++i) {
            int sizeCollision = table[i].size();

            if (sizeCollision < 2) {
                continue;
            }

            hasLevel2Hash[i] = true;

            // Bảng băm cấp 2 với độ lớn bằng bình phương số lượng collision tại index thứ ith xảy ra ở hàm băm 1 để tỉ lệ xảy ra collision là tối ưu
            int sizeTable2 = sizeCollision * sizeCollision;
            vector<pair<Value, bool>> res(sizeTable2);
            uniform_int_distribution<> dis2(0, sizeTable2 - 1);
            bool collision;
            int cnt = 0;

            // Lặp đến khi tạo ra được tập hợp A2 băm các collision tại index ith để không còn xảy ra collision ở hàm băm 2
            do {
                if (cnt > 1000) { 
                    // Trường hợp lặp quá nhiều lần (hơn 1000 lần) vẫn chưa tìm ra được hàm băm phù hợp do số lượng collision quá lớn thì tăng kích thước bảng băm lên 2 lần
                    sizeTable2 *= 2;
                    cnt = 0;
                }
                cnt++;
                collision = false;
                res.resize(sizeTable2);
                fill(res.begin(), res.end(), make_pair(Value{ "", "" }, false));

                // Tạo ngẫu nhiên tập hợp A2
                setAHash2[i].clear();
                setAHash2[i].resize(maxKeyLength);
                for (int& num : setAHash2[i]) {
                    num = dis2(gen);
                }

                for (const auto& s : table[i]) {
                    int index2 = universalHash(s.key, setAHash2[i], sizeTable2);
                    
                    if (res[index2].second) {
                        if (res[index2].first.key == s.key) { // Trường hợp trùng khóa thì ghi đè value mới cho khóa
                            res[index2].first.val = s.val;
                            continue;
                        }
                        else { //Trường hợp xảy ra collision
                            collision = true;
                            break;
                        }
                    }
                    res[index2] = { s, true };
                }
            } while (collision);

            table[i].clear();
            table[i].resize(sizeTable2);
            for (int j = 0; j < sizeTable2; j++) {
                table[i][j] = res[j].first;
            }
        }
    }

    void readFile(const string& path, vector<Value>& vec) {
        ifstream ifs(path);
        if (!ifs.is_open()) {
            cout << "Can't open file: " << path << endl;
            return;
        }
        string temp;
        Value cur;
        while (getline(ifs, temp)) {
            if (temp.empty() || isSpaceOnly(temp) || temp.size() < 3) continue;
            stringstream ss(temp);
            ss >> cur.key;
            cur.val = temp;
            vec.push_back(cur);
        }
        ifs.close();
    }

    string search(const string& key) {
        int index1 = universalHash(key, setAHash1, table.size());

        if (index1 >= table.size() || table[index1].empty()) {
            return "Not found";
        }

        if (!hasLevel2Hash[index1]) {
            // Không có băm cấp 2, kiểm tra key
            if (table[index1].begin()->key == key) {
                return table[index1].begin()->val;
            }
            return "Not found";
        }

        // Có dùng băm cấp 2
        int index2 = universalHash(key, setAHash2[index1], table[index1].size());
        if (index2 >= table[index1].size() || table[index1][index2].key != key) {
            return "Not found";
        }

        return table[index1][index2].val;
    }

    void clear() { // Xóa bảng băm
        m = 0;
        table.clear();
        table.shrink_to_fit();
    }
};


class ModuloHashing {
private:
    vector<list<Value>> table;
    size_t m = 0; // kích thước bảng băm có giá trị là 2^n

    bool isSpaceOnly(string& s) {
        return all_of(s.begin(), s.end(), [](char ch) {
            return isspace(static_cast<unsigned char>(ch));
            });
    }

    size_t findTableSize(int size) {
        size_t res = 1;
        int n = 0;
        while (res <= size) {
            res = res << 1;
            n++;
        }
        return res;
    }

    int hashFunction(const string& key) {
        uint32_t k = 0;
        for (int i = 0; i < key.size(); ++i) { //Băm từng kí tự
            int val = static_cast<int>(key[i]);
            if (val < 0 || val > 127) return -1;
            k += val;
        }
        return k % m;
    }

public:
    ModuloHashing() {}

    void build(const string& path) {
        vector<Value> data;
        readFile(path, data);
        m = findTableSize((int)data.size()); // Tìm kích thước của bảng băm, đồng thời gán lại n là bậc lũy thừa cơ số 2 với 2^n = m
        table.assign(m, list<Value>());

        for (const auto& cur : data) {
            insert(cur);
        }
    }

    void readFile(const string& path, vector<Value>& vec) {
        ifstream ifs(path);
        if (!ifs.is_open()) {
            cerr << "Can't open file: " << path << endl;
            return;
        }
        string temp;
        Value cur;
        while (getline(ifs, temp)) {
            if (temp.empty() || isSpaceOnly(temp) || temp.size() < 3) continue;
            stringstream ss(temp);
            ss >> cur.key;
            cur.val = temp;
            vec.push_back(cur);
        }
        ifs.close();
    }

    bool insert(const Value& data) {
        int index = hashFunction(data.key);
        if (index < 0 || index >= m) { // index không hợp lệ
            return false;
        }
        for (auto& cur : table[index]) {
            if (cur.key == data.key) {
                cur.val = data.val;
                return true; // Cập nhật lại giá trị vì key đã tồn tại
            }
        }
        table[index].push_back(data); // Trường hợp key chưa tồn tại
        return true;
    }

    string search(const string& key) {
        int index = hashFunction(key);
        if (index < 0 || table[index].empty()) { // index không hợp lệ hoặc không tại key trong bảng băm
            return "Not Found";
        }
        auto it = table[index].begin();
        while (it != table[index].end() && it->key != key) {
            it++;
        }
        if (it == table[index].end()) { // Trường hợp key không tồn tại
            return "Not Found";
        }
        else { //Trường hợp key tồn tại, trả về giá trị của key
            return it->val;
        }
    }

    void clear() { // Xóa bảng băm
        m = 0;
        table.clear();
        table.shrink_to_fit();
    }
};


SIZE_T getMemoryUsageMB() { //Hàm đo bộ nhớ đã sử dụng
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / (1024 * 1024); // Đơn vị: MB
    }
    return 0;
}


int main() {
    string path;
    std::cout << "Input the path of file: ";
    cin >> path;
    std::cout << endl;


    // Hashing
    PerfectHashing PH;
    auto start = chrono::high_resolution_clock::now();
    SIZE_T before = getMemoryUsageMB();
    PH.build(path);
    SIZE_T after = getMemoryUsageMB();
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::cout << "PERFECT HASHING: " << endl;
    std::cout << "Run time: " << duration.count() << "ms\t\t" << "Memory: " << after - before << " MB" << endl;
    std::cout << endl;

    ModuloHashing MH;
    start = chrono::high_resolution_clock::now();
    before = getMemoryUsageMB();
    MH.build(path);
    after = getMemoryUsageMB();
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::cout << "MODULO HASHING: " << endl;
    std::cout << "Run time: " << duration.count() << "ms\t\t" << "Memory: " << after - before << " MB" << endl;
    std::cout << endl;
    std::cout << "------------------------------------------------------" << endl << endl;


    // Search
    int n;
    std::cout << "Input the numbers of keys which you want to SEARCH: ";
    cin >> n;
    vector<string> vec(n);
    std::cout << "Input " << n << " keys: " << endl;
    for (string& s : vec) {
        cin >> s;
    }
    std::cout << endl;

    std::cout << "Result of Perfect Hashing:" << endl << endl;
    start = chrono::high_resolution_clock::now();
    for (string& s : vec) {
        std::cout << PH.search(s) << endl << endl;
    }
    end = chrono::high_resolution_clock::now();
    auto ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << endl << endl;


    std::cout << "Result of Modulo Hashing:" << endl << endl;
    start = chrono::high_resolution_clock::now();
    for (string& s : vec) {
        std::cout << MH.search(s) << endl << endl;
    }
    end = chrono::high_resolution_clock::now();
    ans = chrono::duration_cast<chrono::microseconds>(end - start);
    std::cout << "Run time: " << ans.count() << " microseconds\t\t" << endl << endl;
    std::cout << "------------------------------------------------------" << endl << endl;

    PH.clear();
    MH.clear();

    return 0;
}