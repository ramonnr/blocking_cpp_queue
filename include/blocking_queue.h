#include <limits>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <limits.h>

namespace blockingqueue{

    constexpr size_t st_inf = std::numeric_limits<size_t>::max();

    template<typename T>
    class BlockingQueue {
    private:
    public:

        //Defaults to infinite size (size_t::max)
        BlockingQueue(size_t size = st_inf) : _size_max(size){}

        //Pops front element, blocks until a value is available.
        T pop(){
            std::unique_lock<std::mutex> lk(_m);
            _cv.wait(lk, [this]{ return _data.size() > 0; });
            auto t = _data.front();
            _data.erase(_data.begin());
            lk.unlock();
            return t;
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

    private:
        std::vector<T> _data;
        const size_t _size_max;
        std::condition_variable _cv;
        std::mutex _m;
    };
}

