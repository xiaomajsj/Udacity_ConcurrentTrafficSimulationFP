#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
   std::unique_lock<std::mutex> _recLock(_MsgMutex);
  _MsgCond.wait(_recLock, [this]{ return !_queue.empty();});
  T msg=std::move(_queue.back());
  _queue.pop_back();
  --_msgNum;
  return msg; 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
  std::lock_guard<std::mutex> _sendLock(_MsgMutex);
  while(!_queue.empty()){
  _queue.pop_back();
  }
  _queue.push_back(std::move(msg));
  std::cout<<"Message #"<<msg<<" will be added in the queue."<<std::endl;
  ++_msgNum;
  _MsgCond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _lightPhaseQueue = std::make_shared<MessageQueue<TrafficLightPhase>>();
}
TrafficLight::~TrafficLight()
{
  
}

void TrafficLight::waitForGreen()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase Recmsg = _lightPhaseQueue->receive();
        std::cout<<"receiving light msg as= "<<Recmsg<<std::endl;
        if(Recmsg==TrafficLightPhase::green)
        {std::cout<<"now is green= "<<Recmsg<<std::endl;
        break;}
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    while(true){
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    

        std::default_random_engine generator(std::random_device{}());
        std::uniform_int_distribution<int> distribution(4000, 6000);
        int LightDuration = distribution(generator);

        TrafficLightPhase msg= this->getCurrentPhase();
        _lightPhaseQueue->send(std::move(msg));

        std::this_thread::sleep_for(std::chrono::milliseconds(LightDuration));
  
           switch (_currentPhase)
          {
            case TrafficLightPhase::red: 
            _currentPhase=TrafficLightPhase::green;
            break;
            case TrafficLightPhase::green: 
            _currentPhase=TrafficLightPhase::red;
            break;
          }
    }
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
}

