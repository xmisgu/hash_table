#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <forward_list>
#include <time.h>

template <typename T>
class HashTable {
public:

    HashTable() {
        n = 0;
        table.resize(1);
    }
    ~HashTable(){
        delete_all();
    }
    int get_hash_value(const std::string& key) {
        int hashed_key = 0;
        for (unsigned int i = 0; i < key.size(); i++) {
            hashed_key = hashed_key + (int)key[i] * pow(31, key.size() - (i + 1));
        }
        hashed_key = hashed_key % table.capacity();
        return hashed_key;
    }

    void add(const std::string& key,const T& value) {
        float load_factor = n / table.capacity();
        if (load_factor >= 0.75) {
            table.resize(table.capacity() * 2);
            rehash();
        }
        int hashed_key = get_hash_value(key);
        if (!table[hashed_key].empty()) {
            for (auto a : table[hashed_key]) {
                if (a.first == key) {
                    a = std::make_pair(key, value);
                    return;
                }
            }
        }
        
        table[hashed_key].push_front(std::make_pair(key, value));            
        n++;
    }

    std::pair<std::string, T>* find(const std::string& key) {
        int hashed_key = get_hash_value(key);

        for (auto& x : table[hashed_key]) {
            if (x.first == key) {
                return &x;
            }
        }
        return nullptr;
    }

    bool delete_item(const std::string& key) {
        int hashed_key = get_hash_value(key);

        for (auto& x : table[hashed_key]) {
            if (x.first == key) {
                table[hashed_key].remove(x);
                n--;
                return 1;
            }
        }
        return 0;
    }

    void delete_all() {
        for (unsigned int i = 0; i < table.size(); i++) {
            table[i].clear();
            table[i].resize(0);
        }
        table.clear();
        table.shrink_to_fit();
        table.resize(1);
        n = 0;
    }

    std::string statistics() {
        std::ostringstream output;
        int non_empty_indecees = 0;
        int max_len = 0;
        int min_len = 100000000;
        float avg_len = 0;

        output << "stats: \n";
        for (unsigned int i = 0; i < table.size(); i++) {
            if (!table[i].empty()) {
                int tmp_len = 0;
                for (auto& a : table[i]) {
                    tmp_len++;
                }
                if (tmp_len > max_len) {
                    max_len = tmp_len;
                }
                else if (tmp_len < min_len) {
                    min_len = tmp_len;
                }
                avg_len += tmp_len;
                non_empty_indecees++;
            }
        }
        output << "list min size: " << min_len << "\n";
        output << "list max size: " << max_len << "\n";
        output << "non-null indeces: " << non_empty_indecees << "\n";
        output << "list avg size: " << (avg_len / table.capacity()) << "\n";

        return output.str();
    }

    std::string display() {
        std::ostringstream output;
        output << "hash table: \n";
        output << "current size: " << n << "\n";
        output << "max size: " << table.capacity() << "\n";
        output << "table: \n{\n";

        for (unsigned int i = 0; i < 10; i++) {
            if (!table[i].empty()) {
                output << i << ": ";
                for (auto& a : table[i]) {
                    output << a.first << " -> " << a.second << ", ";
                }
                output << ";\n";
            }
        }
        output << "}\n";
        output << statistics();

        return output.str();
    }

    
private:
    void rehash() {
        std::vector<std::forward_list<std::pair<std::string, T>>> tmp_table;
        tmp_table.resize(table.size());
        for (unsigned int i = 0; i < table.size(); i++) {
            if (!table[i].empty()) {
                for (auto& a : table[i]) {
                    int hashed_key = get_hash_value(a.first);
                    tmp_table[hashed_key].push_front(a);                  
                }       
            }
        }
        table = tmp_table;
    }

    std::vector<std::forward_list<std::pair<std::string, T>>> table;
    unsigned int n;
};

struct TableValues {
    int d1;
    char d2;
    TableValues(int a, char b) {
        d1 = a;
        d2 = b;
    }
};

std::ostream& operator << (std::ostream& out, const TableValues& obj) {
    out << "{d1: " << obj.d1 << ", d2: " << obj.d2 << "}";
    return out;
}

bool operator < (const TableValues& a, const TableValues& b) {
    return a.d1 < b.d1 || a.d1 == b.d1 && a.d2 < b.d2;
}
bool operator > (const TableValues& a, const TableValues& b) {
    return a.d1 > b.d1 || a.d1 == b.d1 && a.d2 > b.d2;
}

bool operator ==(const TableValues& a, const TableValues& b) {
    return (!(a < b) && !(b < a));
}

std::string get_rand_key() {
    std::string key;
    for (int i = 0; i < 6; i++) {
        key += ('a' + rand() % 26);
    }
    return key;
}

int main()
{
    HashTable<int> table;
    srand(time(NULL));
    
    const int MAX_ORDER = 7;
    for (int o = 1; o <= MAX_ORDER; o++) {
        const int n = pow(10, o);

        std::cout << "===============================================" << std::endl;
        std::cout << "                    TEST" << o << std::endl;
        std::cout << "===============================================" << std::endl;

        clock_t t1 = clock();
        for (int i = 0; i < n; i++) {
            table.add(get_rand_key(), i);
        }
        clock_t t2 = clock();
        std::cout << table.display() << std::endl;
        std::cout << "-----------------------------------------------" << std::endl;
        double time = (t2 - t1) / (double)CLOCKS_PER_SEC;
        std::cout << "Calkowity czas dodawania: " << time << std::endl;
        std::cout << "Sredni czas dodawania: " << time / n << std::endl;
        std::cout << "-----------------------------------------------" << std::endl;
        
        const int m = pow(10, 4);
        int hits = 0;

        t1 = clock();
        for (int i = 0; i < m; i++) {
            auto entry = table.find(get_rand_key());
            if (entry) {
                hits++;
            }
        }
        t2 = clock();

        std::cout << "Liczba trafien: " << hits << std::endl;
        time = (t2 - t1) / (double)CLOCKS_PER_SEC;
        std::cout << "Calkowity czas wyszukiwania: " << time << std::endl;
        std::cout << "Sredni czas wyszukiwania: " << time / n << std::endl;
        std::cout << "-----------------------------------------------" << std::endl;
        
        std::cout << table.statistics();
        table.delete_all();
        std::cout << "===============================================\n\n\n" << std::endl;

    }
    system("pause");
}
