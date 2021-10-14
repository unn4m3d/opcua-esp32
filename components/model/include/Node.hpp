#pragma once
#include <type_traits>
#include "open62541.h"
#include <driver/gpio.h>
#define UA_TYPEID_IMPL(T, ID) template<> struct ua_type_id< T > : std::integral_constant<int, ID>{}

namespace model
{

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
    {
    public:
        virtual void registerNode(UA_Server*) = 0;

        static UA_StatusCode
            readState(UA_Server *server,
                            const UA_NodeId *sessionId, void *sessionContext,
                            const UA_NodeId *nodeId, void *nodeContext,
                            UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                            UA_DataValue *dataValue)
            {
                auto node = (NodeBase*)nodeContext;
                return node->readStateImpl(server, dataValue);
            } 
        
        static UA_StatusCode
            setState(UA_Server *server,
                        const UA_NodeId *sessionId, void *sessionContext,
                        const UA_NodeId *nodeId, void *nodeContext,
                        const UA_NumericRange *range, const UA_DataValue *dataValue)
            {
                auto node = (NodeBase*)nodeContext;
                return node->setStateImpl(server, dataValue);
            }
        private:
            virtual UA_StatusCode readStateImpl(UA_Server*, UA_DataValue*) = 0;
            virtual UA_StatusCode setStateImpl(UA_Server*, const UA_DataValue*) = 0;

    };

    template<typename T>
    class Node : public NodeBase
    {   
    public:
        Node(char* name,
            const uint8_t access,
            const UA_NodeId pId,
            const UA_NodeId cId,
            const UA_NodeId prId,
            const UA_NodeId vtId) :
            m_name(name),
            m_access(access),
            m_parentId(pId),
            m_currentId(cId),
            m_parentRefId(prId),
            m_varTypeId(vtId)
            {}

        virtual void registerNode(UA_Server* server)
        {
            UA_VariableAttributes attr = UA_VariableAttributes_default;
            attr.displayName = UA_LOCALIZEDTEXT("en-US", m_name);
            attr.dataType = UA_TYPES[ua_type_id<T>::value].typeId;
            attr.accessLevel = m_access;

            UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, m_name);

            UA_DataSource servo0;
            servo0.read = &NodeBase::readState;
            servo0.write = &NodeBase::setState;
            UA_Server_addDataSourceVariableNode(server, m_currentId, m_parentId,
                                                m_parentRefId, currentName,
                                                m_varTypeId, attr,
                                                servo0, (void*)this, NULL);
        }

        virtual T read() = 0;
        virtual bool write(const T&) = 0;
        

        virtual UA_StatusCode readStateImpl(UA_Server*, UA_DataValue* value)
        {
            auto r = read();
            UA_Variant_setScalarCopy(&value->value, &r, &UA_TYPES[ua_type_id<T>::value]);
            value->hasValue = true;
            return UA_STATUSCODE_GOOD;
        }

        virtual UA_StatusCode setStateImpl(UA_Server*, const UA_DataValue* value)
        {

            return write(*((T*)(value->value.data))) ? UA_STATUSCODE_GOOD : UA_STATUSCODE_BADUNEXPECTEDERROR;
        }
    private:
        

        char* m_name;
        const uint8_t m_access;
        const UA_NodeId m_parentId, m_currentId, m_parentRefId, m_varTypeId;
    };

    class Pin : public Node<bool>
    {
    public:
        Pin(char* name, uint8_t num) : 
            Node<bool>(name,
                UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE,
                UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                UA_NODEID_STRING(1, name),
                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE)),
            m_pin(num)
            {}

        void mode(gpio_mode_t mode, bool pullup = false, bool pulldown = false)
        {
            gpio_config_t cfg{
                .pin_bit_mask = 1ull << m_pin,
                .mode = mode,
                .pull_up_en = (pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE),
                .pull_down_en = (pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE),
                .intr_type = GPIO_INTR_DISABLE
            };

            gpio_config(&cfg);
        }
        
        virtual bool read()
        {
            return gpio_get_level(static_cast<gpio_num_t>(m_pin));
        }

        virtual bool write(const bool& v)
        {
            gpio_set_level(static_cast<gpio_num_t>(m_pin), static_cast<uint32_t>(v));
            return true;
        }
    private:
        uint8_t m_pin;
    };
}