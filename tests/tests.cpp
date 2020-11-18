#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../include/blocking_queue.h"
#include <thread>
#include <array>
using namespace blockingqueue;

TEST_CASE( "Max size is adhered to"){
    BlockingQueue<int> bq(1);
    REQUIRE(bq.size() == 0);
    bq.push(1);
    REQUIRE(bq.size() == 1);
    bq.push(2);
    REQUIRE(bq.size() == 1);

    BlockingQueue<int> bq_bigger(2);
    for(int i = 0; i < 10; i++){
        bq_bigger.push(i);
        REQUIRE(bq_bigger.size() <= 2);
    }
}

TEST_CASE( "Overwrites from front"){
    BlockingQueue<int> bq(2);
    for(int i = 0; i < 10; i++){
        bq.push(i);
    }
    REQUIRE( bq.pop() == 8 );
    REQUIRE( bq.pop() == 9 );
}

TEST_CASE( "Single producer/consumer" ){
    BlockingQueue<int> bq;
    auto t = std::thread([&]{
        for(int i = 0; i < 20; i++){
            bq.push(i);
        }
    });

    for(int i = 0; i < 20; i++){
        REQUIRE( bq.pop() == i);
    }
    t.join();
}

TEST_CASE( "Single producer multiple consumers" ){
   BlockingQueue<int> bq;

    std::array<std::thread, 4> consumers;
    for(int i = 0; i < 4; i++){
        consumers[i] = std::thread([&]{
                    for(int i = 0; i < 25; i++){
                        bq.pop();
                    }});
    }

    for(int i = 0; i < 100; i++){
        bq.push(i);
    }
    for(int i = 0; i < consumers.size(); i++){
        consumers[i].join();
    }
}

TEST_CASE( "Multiple producers single consumer" ){
    BlockingQueue<int> bq;
    std::array<std::thread, 4> producers;
    for(int i = 0; i < producers.size(); i++){
        producers[i] = std::thread([&]{
            for(int i = 0; i < 25; i++){
                bq.push(i);
            }
        });
    }
    for(int i = 0; i < producers.size() * 25; i++){
        bq.pop();
    }
    for(int i = 0; i < producers.size(); i++){
        producers[i].join();
    }
    REQUIRE( bq.size() == 0 );
}

