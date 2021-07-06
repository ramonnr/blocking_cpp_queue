#pragma once
#include <limits>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <limits.h>
#include <optional>

namespace blockingqueue{

    constexpr size_t st_inf = std::numeric_limits<size_t>::max();

    template<typename T>
    class BlockingQueue {

    public:

        //Defaults to infinite size (size_t::max)
        BlockingQueue(size_t size = st_inf) : _size_max(size){}

        //Pops front element, blocks until a value is available.
        std::optional<T> pop(){
            std::unique_lock<std::mutex> lk(_m);
            _cv.wait(lk, [this]{return (_data.size() > 0) || _wake_blocked;});
            if( _wake_blocked ){
                _wake_blocked = false;
                return {};
            }
            auto t = _data.front();
            _data.erase(_data.begin());
            lk.unlock();
            return t;
        }

        //return front element if it exists
        std::optional<T> peek(){
            std::lock_guard<std::mutex> lk(_m);
            if(_data.size() > 0){
                return _data.front();
            }
            return {};
        }

        //Pops up to n items
        std::vector<T> pop_range(std::size_t n){
            std::lock_guard<std::mutex> lk(_m);
            auto first = _data.begin();
            auto last = _data.begin() + std::min(n, _data.size());
            std::vector<T> cpy {first, last};
            _data.erase(first, last);
            return cpy;
        }

        //Pushes t. Pops front element if size has reached the limit
        void push(const T& t){
            {
                std::lock_guard<std::mutex> lk(_m);
                if(_data.size() == _size_max){ _data.erase(_data.begin()); }
                _data.push_back(t);
                _cv.notify_one();
            }
        }

        //Number of currently held items
        size_t size(){
            std::lock_guard<std::mutex> lk(_m);
            return _data.size();
        }

        //Erases all elements
        void clear(){
            std::lock_guard<std::mutex> lk(_m);
            _data.clear();
        }

        void wake_up_blocked(){
            std::lock_guard<std::mutex> lk(_m);
            _wake_blocked = true;
            _cv.notify_one();
        }

    private:
        std::vector<T> _data;
        const size_t _size_max;
        std::condition_variable _cv;
        std::mutex _m;
        bool _wake_blocked = false;
    };
}

