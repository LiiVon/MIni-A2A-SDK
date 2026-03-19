#pragma once

#include "../../include/models/agent_card.hpp"
#include "../../include/core/http_client.hpp"
#include <string>
#include <vector>

namespace a2a
{
    class A2ACardResolver 
    {
    public:
        explicit A2ACardResolver(const std::string& base_url,
            const std::string& agent_card_path = "/.well-known/agent-card.json");
        ~A2ACardResolver();

        A2ACardResolver(const A2ACardResolver&) = delete;
        A2ACardResolver& operator=(const A2ACardResolver&) = delete;
        A2ACardResolver(A2ACardResolver&&) noexcept;
        A2ACardResolver& operator=(A2ACardResolver&&) noexcept; 

        AgentCard get_agent_card();
        std::string get_agent_card_url() const;
    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}