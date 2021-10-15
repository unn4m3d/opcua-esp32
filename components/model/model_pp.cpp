#include "model.hpp"
#include "Pin.hpp"

model::UAPlatform platform(nullptr);


model::Pin<model::UAPlatform, model::ESP32Backend>
    led0("Led0", GPIO_NUM_0),
    led1("Led1", GPIO_NUM_2);

extern "C" void registerNodes(UA_Server* srv)
{
    platform.setServer(srv);
    led0.registerNode(&platform);
    led1.registerNode(&platform);

    led0.mode(GPIO_MODE_INPUT_OUTPUT);
    led1.mode(GPIO_MODE_INPUT);
}