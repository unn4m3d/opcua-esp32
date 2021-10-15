#pragma once
#include "UAPlatform.hpp"

namespace model
{
    template<class TP = DefaultPlatform, class TB = DefaultBackend>
    class Pin : public Node<bool, TP>
    {
    public:
        Pin(char* name, typename TB::pin_id_t num) : 
            Node<bool, TP>(name,
                TP::AccessLevel::ReadWrite,
                UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                UA_NODEID_STRING(1, name),
                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE)),
            m_pin(num)
            {}

        void mode(gpio_mode_t mode, bool pullup = false, bool pulldown = false)
        {
            TB::gpio_mode(m_pin, mode, pullup, pulldown);
        }
        
        virtual bool read() const
        {
            return TB::gpio_read(m_pin);
        }

        virtual bool write(const bool& v)
        {
            TB::gpio_write(static_cast<gpio_num_t>(m_pin), static_cast<uint32_t>(v));
            return true;
        }
    private:
        typename TB::pin_id_t m_pin;
    };
}