#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <thread>
#include <stdlib.h>
#include <condition_variable>
#include "TrafficObject.h"
#include <time.h>

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{
public:
int getMsgnum(){return _msgNum;}

T receive();
void send(T &&msg);


private:
std::mutex _MsgMutex;
std::condition_variable _MsgCond;
std::deque<T> _queue;
int _msgNum;
    
};


enum TrafficLightPhase{
   red=0,
   green
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();
   

    // typical behaviour methods
    void waitForGreen();
    void simulate();
private:
    // typical behaviour methods
    void cycleThroughPhases();


    std::condition_variable _condition;
    std::mutex _mutex;

    TrafficLightPhase _currentPhase;
  


    int LightDuration;

    std::shared_ptr<MessageQueue<TrafficLightPhase>> _lightPhaseQueue;

};

#endif