#pragma once
#include "PlatformBase.hpp"
#include "open62541.h"
#include "Node.hpp"

namespace model
{
    class UAPlatform : public PlatformBase<UAPlatform, UA_NodeId>
    {
    public:
        typedef UA_StatusCode status_t;

        enum class AccessLevel
        {
            Read = UA_ACCESSLEVELMASK_READ,
            Write = UA_ACCESSLEVELMASK_WRITE,
            ReadWrite = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE
        };

        UAPlatform(UA_Server* srv) : PlatformBase(this), m_server(srv) {}

        template<typename T>
        node_id_t registerNodeImpl(Node<T, UAPlatform>* node)
        {
            UA_VariableAttributes attr = UA_VariableAttributes_default;
            attr.displayName = UA_LOCALIZEDTEXT("en-US", node->m_name);
            attr.dataType = UA_TYPES[ua_type_id<T>::value].typeId;
            attr.accessLevel = static_cast<uint8_t>(node->m_access);

            UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, node->m_name);

            UA_DataSource servo0;
            servo0.read = &readState<T>;
            servo0.write = &writeState<T>;
            node_id_t new_id;
            UA_Server_addDataSourceVariableNode(m_server, node->m_currentId, node->m_parentId,
                                                node->m_parentRefId, currentName,
                                                node->m_varTypeId, attr,
                                                servo0, static_cast<void*>(node), &new_id);
                                            
            return new_id;
        }

        template<typename T>
        static status_t readStateImpl(const Node<T, UAPlatform>& node, UA_Server* srv, UA_DataValue* value)
        {
            auto r = node.read();
            UA_Variant_setScalarCopy(&value->value, &r, &UA_TYPES[ua_type_id<T>::value]);
            value->hasValue = true;
            return UA_STATUSCODE_GOOD;
        }

        template<typename T>
        static status_t writeStateImpl(Node<T, UAPlatform>& node, UA_Server* srv, const UA_DataValue* value)
        {
            return node.write(*((T*)(value->value.data))) ? UA_STATUSCODE_GOOD : UA_STATUSCODE_BADUNEXPECTEDERROR;
        }

        void setServer(UA_Server* s) { m_server = s; }

    private:
        template<typename T>
        static UA_StatusCode
        readState(UA_Server *server,
                        const UA_NodeId *sessionId, void *sessionContext,
                        const UA_NodeId *nodeId, void *nodeContext,
                        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                        UA_DataValue *dataValue)
        {
            auto node = (Node<T, UAPlatform>*)nodeContext;
            if(node)
                return readStateImpl<T>(*node, server, dataValue);
            else
                return UA_STATUSCODE_BADNODEIDINVALID;
        } 
        
        template<typename T>
        static UA_StatusCode
            writeState(UA_Server *server,
                        const UA_NodeId *sessionId, void *sessionContext,
                        const UA_NodeId *nodeId, void *nodeContext,
                        const UA_NumericRange *range, const UA_DataValue *dataValue)
            {
                auto node = (Node<T, UAPlatform>*)nodeContext;
                if(node)
                    return writeStateImpl<T>(*node, server, dataValue);
                else
                    return UA_STATUSCODE_BADNODEIDINVALID;
            }

        UA_Server* m_server;
    };

    typedef UAPlatform DefaultPlatform;
}