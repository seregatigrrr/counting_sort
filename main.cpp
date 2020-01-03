#include <iostream>
#include "tvector.h"
#include <cstring>
#include <limits>
struct TString{
    static const int n = 65;
    char str[n]{};
    TString(){
        memset(str, 0, n);
    };
    explicit TString(const char* const s){
        int i;
        for(i = 0; i < n - 1 && i < strlen(s); ++i){
            str[i] = s[i];
        }
        for(; i < n; ++i){
            str[i] = 0;
        }
    }

};
std::istream& operator>> (std::istream& in, TString s){
    in >> s.str;
    return in;
}
std::ostream& operator<< (std::ostream& out, TString s){
    out << s.str;
    return out;
}
struct TData{
    TData() = default;
    TData(int key_, char* str) : key(key_), value(str){}
    long int key = 0;
    TString value;
};


int main() {
    long int key;char str[TString::n];
    TVector<TData> v;
    int max = INT_MIN;
    while(std::cin >> key >> str){
        v.EmplaceBack(key,str);
        if(max < key){
            max = key;
        }
    }

    int c[max + 1];
    TVector<TData> res(v.Size());
    for(size_t i = 0; i < max + 1; ++ i){
        c[i] = 0;
    }
    for(size_t j = 0; j < v.Size(); ++j){
        ++c[v[j].key];
    }

    for(size_t i = 1; i < max + 1; ++i){
        c[i] += c[i-1];
    }

    for(int j = (int)v.Size() - 1; j > -1; --j){
        res[c[v[j].key] - 1] = v[j];
        --c[v[j].key];
    }
    for(auto& i: res){
        std::cout << i.key << ' ' << i.value << '\n';
    }
    return 0;
}