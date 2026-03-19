#pragma once

#include "../../include/core/types.hpp"
#include <string>
#include <map>
#include <optional>

namespace a2a
{
    // @brief: Agent Capabilities
    struct AgentCapabilities
    {
        bool streaming = false;
        bool push_notification = false;
        bool task_management = true;
        
        std::string to_json() const;
        static AgentCapabilities from_json(const std::string& json_str);
    };

    // @brief: Agent Skill -- a union of capabilities
    struct AgentSkill
    {
        std::string name;
        std::string description;
        std::vector<std::string> input_modes;
        std::vector<std::string> output_modes;

        std::string to_json() const;
        static AgentSkill from_json(const std::string& json_str);
    };

    // @brief:Agent Provider Info
    struct AgentProvider
    {
        std::string name;
        std::string organization;
        std::optional<std::string> url;

        std::string to_json() const;
        static AgentProvider from_json(const std::string& json_str);
    };

    // @brief: Agent Card -- conveys key information about an agent
    class AgentCard
    {
        public:
        AgentCard() = default;

        // Getters
        const std::string& name() const { return name_; }
        const std::string& description() const { return description_; }
        const std::string& url() const { return url_; }
        const std::string& version() const { return version_; }
        const std::string& protocol_version() const { return protocol_version_; }
        const std::optional<std::string>& icon_url() const { return icon_url_; }
        const std::optional<std::string>& documentation_url() const { return documentation_url_; }
        const AgentCapabilities& capabilities() const { return capabilities_; }
        const std::vector<std::string>& default_input_modes() const { return default_input_modes_; } 
        const std::vector<std::string>& default_output_modes() const { return default_output_modes_; }
        const std::vector<AgentSkill>& skills() const { return skills_; }
        AgentTransport referred_transport() const { return  preferred_transport_; }
        const std::optional<AgentProvider>& provider() const { return provider_; }
        
        // Setters
        void set_name(const std::string& name) { name_ = name; }
        void set_description(const std::string& description) { description_ = description; }
        void set_url(const std::string& url) { url_ = url; }
        void set_version(const std::string& version) { version_ = version; }
        void set_protocol_version(const std::string& protocol_version) { protocol_version_ = protocol_version; }
        void set_icon_url(const std::optional<std::string>& icon_url) { icon_url_ = icon_url; }
        void set_documentation_url(const std::optional<std::string>& documentation_url) { documentation_url_ = documentation_url; }
        void set_capabilities(const AgentCapabilities& capabilities) { capabilities_ = capabilities; }
        void set_default_input_modes(const std::vector<std::string>& input_modes) { default_input_modes_ = input_modes; }
        void set_default_output_modes(const std::vector<std::string>& output_modes) { default_output_modes_ = output_modes; }
        void set_skills(const std::vector<AgentSkill>& skills) { skills_ = skills; }
        void set_preferred_transport(AgentTransport transport) { preferred_transport_ = transport; }
        void set_provider(const std::optional<AgentProvider>& provider) { provider_ = provider; }   
        void add_input_mode(const std::string& mode) { default_input_modes_.push_back(mode); }
        void add_output_mode(const std::string& mode) { default_output_modes_.push_back(mode); }
        void add_skill(const AgentSkill& skill) { skills_.push_back(skill); }

        // Serialization and deserialization
        std::string to_json() const;
        static AgentCard from_json(const std::string& json_str);

        static AgentCard create()
        {
            return AgentCard();
        }

        // fluent API methods
        AgentCard& with_name(const std::string& name)
        {
            name_ = name;
            return *this;
        }

        AgentCard& with_description(const std::string& description)
        {
            description_ = description;
            return *this;
        }

        AgentCard& with_url(const std::string& url)
        {
            url_ = url;
            return *this;
        }

        AgentCard& with_version(const std::string& version)
        {
            version_ = version;
            return *this;
        }

        AgentCard& with_capabilities(const AgentCapabilities& caps)
        {
            capabilities_ = caps;
            return *this;
        }

        AgentCard& with_input_mode(const std::string& mode)
        {
            default_input_modes_.push_back(mode);
            return *this;
        }

        AgentCard& with_output_mode(const std::string& mode)
        {
            default_output_modes_.push_back(mode);
            return *this;
        }

        AgentCard& with_skill(const AgentSkill& skill)
        {
            skills_.push_back(skill);
            return *this;
        }


        private:
        std::string name_;
        std::string description_;
        std::string url_;
        std::string version_;
        std::string protocol_version_ = "0.3.0";
        std::optional<std::string> icon_url_;
        std::optional<std::string> documentation_url_;
        AgentCapabilities capabilities_;
        std::vector<std::string> default_input_modes_ = {"text"};
        std::vector<std::string> default_output_modes_ = {"text"};
        std::vector<AgentSkill> skills_;
        AgentTransport  preferred_transport_ = AgentTransport::JsonRpc;
        std::optional<AgentProvider> provider_;
    };
}