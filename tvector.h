#pragma once

#include <utility>
#include <exception>
#include <stdexcept>
#include <limits>
template <typename T>
struct TMemory;

template<typename T>
class TVector{
public:
    using size_type = size_t;
    TVector() = default;
    explicit TVector(size_type n) : data(n){
        for(size_type i = 0; i < n; ++i){
            Construct(data + i);
        }
        sz = n;
    }
    TVector(size_type count, const T& value): data(count){
        for(size_type i = 0; i < count; ++i){
            Construct(data + i, value);
        }
        sz = count;
    }
    TVector(const TVector<T>& other): data(other.sz){
        for(size_type i = 0; i < other.sz; ++i){
            Construct(data + i, other[i]);
        }
        sz = other.sz;
    }
    TVector(TVector&& other) noexcept {
        Swap(other);
    }
    ~TVector() {
        for (size_type i = 0; i < sz; ++i) {
            Destroy(data + i);
        }
    }
    TVector(std::initializer_list<T> init): data(init.size()){
        for(size_type i = 0; i < init.size(); ++i){
            Construct(data + i, *(init.begin() + i));
        }
        sz = init.size();
    }

    TVector& operator =(TVector<T>&& other) noexcept{
        Swap(other);
        return *this;
    }
    TVector& operator= (const TVector<T>& other){
        if(other.sz > data.cp) {
            TVector<T> temp(other);
            Swap(temp);
        }
        else{
            for(size_type i = 0; i < std::min(sz, other.sz); ++i){
                data[i] = other[i];
            }
            if(sz > other.sz){
                for(size_type i = sz; i!= other.sz; --i){
                    Destroy(data + i - 1);
                }
            }
            else if(sz < other.sz){
                for(size_type i = sz; i < other.sz; ++i){
                    Construct(data + i, other[i]);
                }
            }
            sz = other.sz;
        }
        return *this;
    }
    bool operator== (const TVector& other) const{
        if(other.sz != sz){
            return false;
        }
        for (size_type i = 0; i < sz; ++i){
            if(data[i] != other[i]){
                return false;
            }
        }
        return true;
    }

    //Вместимость
    size_type Capacity() const {
        return data.cp;
    }
    size_type Size() const{
        return sz;
    }
    bool Empty() const{
        return ( sz == 0 );
    }
    /*size_t MaxSize() const{
        return std::numeric_limits<size_t >::max(); //не распознает size_t как тип. как заменить его на то, чьим элиасом он является?
    }*/

    // Доступ к элементам
    T& At(const size_type pos){
        if(!(pos > 0 &&  pos < sz)){
            throw std::out_of_range("index is out of range");//здесь можно написать например sstream ss; ss << pos <<" > " << sz, но страшна. что такое поток как не контейнер?А А А
        }
        return data[pos];
    }
    const T& At(size_type pos) const{
        if(!(pos > 0 &&  pos < sz)){
            throw std::out_of_range("index is out of range");//здесь можно написать например sstream ss; ss << pos <<" > " << sz, но страшна. что такое поток как не контейнер? А А А
        }
        return data[pos];
    }
    const T& operator[](size_type i) const{
        return data[i];
    }
    T& operator[] (const size_type i){
        return data[i];
    }
    T& Back(){
        return data[sz - 1];
    }
    const T& Back() const{
        return data[sz - 1];
    }

    T& Front(){
        return *data.buf;
    }
    const T& Front() const{
        return *data.buf;
    }

    T* Data(){
        return &Front();
    }
    const T* Data() const {
        return &Front();
    }

    //Итераторы
    T* begin() { //WARNING, ЖЕРТВА CODESTYLE
        return data.buf;
    }
    T* end(){ //WARNING, ЖЕРТВА CODESTYLE
        return data + sz;
    }
    const T* begin() const { //WARNING, ЖЕРТВА CODESTYLE
        return data.buf;
    }
    const T* end() const{ //WARNING, ЖЕРТВА CODESTYLE
        return data + sz;
    }

    //Модификаторы
    void Clear(){
        for (size_type i = 0; i < sz; ++i) {
            Destroy(data + i);
        }
        sz = 0;
    }
    void PushBack(const T& elem){
        if(sz == data.cp){
            Reserve(sz == 0? 1 : sz * 2);
        }
        Construct(data + sz, elem);
        ++sz;
    }
    void PushBack(T&& elem){
        if(sz == data.cp){
            Reserve(sz == 0? 1 : sz * 2);
        }
        Construct(data + sz, std::move(elem));
        ++sz;
    }
    void PopBack(){
        if(sz > 0) {
            Destroy(data + sz - 1);
        }
        --sz;
    }
    T* Erase(T* pos){
        if( pos >= begin() && pos < end()) {
            for (T* i = pos; i != end() - 1; ++i) {
                *i = *(i + 1);
            }
            this->PopBack();
            return pos;
        }
    }
    T* Erase(T* first, T* last){ //вот бы в новом стандарте параметры вычислялись в детерм порядке...
        if(first < last && first >= begin() && last <= end()){
            size_type step = last - first;
            for(T* i = first; i != end() - step; ++i){
                *i = *(i + step);
            }
            for(size_type i = 0; i < step; ++i){
                Destroy(end() - i - 1);
            }
            sz = sz - step;
            return first;
        }
        //exception
    }

    T* Insert(const T* pos, const T& value) {
        if (pos >= begin() && pos <= end()) {
            size_type index = pos - begin();
            Resize(sz + 1);
            for (T* i = end() - 1; i > begin() + index; --i) {
                *i = *(i - 1);
            }
            data[index] = value;
            return data + index;
        }
        //exception
    }
    T* Insert(const T* pos, T&& value ){
        if (pos >= begin() && pos <= end()) {
            size_type index = pos - begin();
            Resize(sz + 1);
            for (T* i = end() - 1; i > begin() + index; --i) {
                *i = *(i - 1);
            }
            data[index] = std::move(value);
            return data + index;
        }
        //exception
    }
    template< class InputIt >
    T* Insert(const T* pos, InputIt first, InputIt last ){
        if (pos >= begin() && pos <= end()){
            size_type index = pos - begin();
            size_type nElems = last - first;
            Resize(sz + nElems);
            for (T* i = end() - 1; i > begin() + index + nElems - 1; --i) {
                *i = *(i - nElems);
            }
            for(InputIt cur = first; cur != last; ++cur){
                data[index + cur - first] = *cur;
            }
            return data + index;
        }
        //exception
    }
    T* Insert( T* pos, std::initializer_list<T> ilist ){
        return Insert(pos, ilist.begin(), ilist.end());
    }
    void Resize(size_type n){
        Reserve(n);
        if(sz < n){
            for(size_type i = sz; i < n; ++i){
                Construct(data + sz);
            }
        }
        else{
            for(size_type i = sz; i!= n; --i){
                Destroy(data + i - 1);
            }
        }
        sz = n;
    }
    template <typename ... Args>
    void EmplaceBack( Args&& ... args ){
        if(sz == data.cp){
            Reserve(sz == 0 ? 1 : sz * 2);
        }
        T* elem = new (data + sz) T(std::forward<Args>(args)...);
        ++sz;
    }

    template< class... Args >
    T* Emplace( const T* pos, Args&&... args ) {
        if (pos >= begin() && pos <= end()) {
            size_type index = pos - begin();
            Resize(sz + 1);
            for (T *i = end() - 1; i > begin() + index; --i) {
                *i = *(i - 1);
            }
            Destroy(data + index);
            T* elem = new (data + index) T(std::forward<Args>(args)...);
            return elem;
        }
    }
    void Swap(TVector& other) noexcept{
        data.Swap(other.data);
        std::swap(sz, other.sz);
    }
private:
    TMemory<T> data;
    size_type sz = 0;

    static void Construct(void* buf){
        new (buf) T();
    }
    static void Construct(void* buf, const T& elem) {
        new (buf) T(elem);
    }
    static void  Construct(void* buf, T&& elem){
        new (buf) T(std::move(elem));
    }
    static void Destroy(T* buf){
        buf->~T();
    }
    void Reserve(const size_type n){
        if(n > data.cp ){
            TMemory<T> data2(n);
            for(size_type i = 0; i < sz; ++i){
                Construct(data2 + i, std::move(data[i]));
                Destroy(data + i);
            }
            data.Swap(data2);
        }
    }
};
template <typename T>
struct TMemory{
    using size_type = size_t;
    T* buf = nullptr;
    size_type cp = 0;

    TMemory() = default;
    static T* Allocate(size_type n) {
        return static_cast<T*>(operator new(n * sizeof(T)));
    }
    static void Deallocate(T* buf){
        operator delete(buf);
    }

    explicit TMemory(size_type n){
        buf = Allocate(n);
        cp = n;
    }

    ~TMemory(){
        Deallocate(buf);
    }

    T* operator+ (size_type i){
        return buf + i;
    }
    const T* operator+ (size_type i) const{
        return buf + i;
    }
    T& operator[] (size_type i){
        return buf[i];
    }

    const T& operator[] (size_type i) const{
        return buf[i];
    }

    void Swap(TMemory<T>& other) noexcept{
        std::swap(buf, other.buf);
        std::swap(cp, other.cp);
    }

    TMemory(TMemory&& other) noexcept{
        Swap(other);
    }
    TMemory& operator= (TMemory&& other) noexcept{
        Swap(other);
        return *this;
    }
};