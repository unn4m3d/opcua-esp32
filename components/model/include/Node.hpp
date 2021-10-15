#pragma once
#include <type_traits>
#include "open62541.h"
#include <driver/gpio.h>
#include "ESP32Backend.hpp"

#define UA_TYPEID_IMPL(T, ID) template<> struct ua_type_id< T > : std::integral_constant<int, ID>{}

namespace model
{
    //class DefaultPlatform;

    template<typename T>
    struct ua_type_id : std::integral_constant<int, T::nonexistent> {}; // Fallback

    UA_TYPEID_IMPL(bool, UA_TYPES_BOOLEAN);
    UA_TYPEID_IMPL(int8_t, UA_TYPES_SBYTE);
    UA_TYPEID_IMPL(uint8_t, UA_TYPES_BYTE);
    UA_TYPEID_IMPL(int16_t, UA_TYPES_INT16);
    UA_TYPEID_IMPL(uint16_t, UA_TYPES_UINT16);
    UA_TYPEID_IMPL(int32_t, UA_TYPES_INT32);
    UA_TYPEID_IMPL(uint32_t, UA_TYPES_UINT32);
    // TODO


    class NodeBase
    {};

    template<typename T, typename TP>
    class Node : public NodeBase
    {   
    public:
        Node(char* name,
            const typename TP::AccessLevel access,
            const typename TP::node_id_t pId,
            const typename TP::node_id_t cId,
            const typename TP::node_id_t prId,
            const typename TP::node_id_t vtId) :
            m_name(name),
            m_access(access),
            m_parentId(pId),
            m_currentId(cId),
            m_parentRefId(prId),
            m_varTypeId(vtId)
            {}

        virtual void registerNode(TP* platform)
        {
            platform->registerNode(this);            
        }

        virtual T read() const = 0;
        virtual bool write(const T&) = 0;
    private:
        char* m_name;
        const typename TP::AccessLevel m_access;
        const typename TP::node_id_t m_parentId, m_currentId, m_parentRefId, m_varTypeId;
        friend TP;
    };
}