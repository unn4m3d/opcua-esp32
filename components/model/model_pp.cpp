#include "model.hpp"
#include "Node.hpp"

model::Pin
    led0("Led0", 0),
    led1("Led1", 1);

extern "C" void registerNodes(UA_Server* srv)
{
    led0.registerNode(srv);
    led1.registerNode(srv);

    led0.mode(GPIO_MODE_INPUT_OUTPUT);
    led1.mode(GPIO_MODE_INPUT_OUTPUT);
}