#include "StateMachine.h"

class MasterSm : public StateMachine {
public:
    MasterSm(
        z_thread_stack_element * threadStack,
        uint32_t threadStackSize_B,
        void (*threadFnAdapter)(void *, void *, void *),
        StateMachineController * smc);

private:
    State root;
    void Root_Entry();
    void Root_Event(Event* event);
    void Root_Exit();
};