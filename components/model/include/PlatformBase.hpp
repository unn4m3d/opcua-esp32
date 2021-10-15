#pragma once

namespace model
{
    class NodeBase;

    template<typename T, typename NodeID>
    class PlatformBase
    {
    public:
        typedef NodeID node_id_t;
        typedef NodeBase* node_handle_t;

        PlatformBase(T* s) : self(s){}

        template<typename Node>
        NodeID registerNode(Node* n)
        {
            return self->registerNodeImpl(n);
        }


    private:
        T* self;
    };

    
}