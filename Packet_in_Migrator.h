#pragma once
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "SwitchSession.h"
class PKTMigrator
{
public:
    PKTMigrator(switch_session* s_session, unsigned int eproxy_data_path_port)
    {
        s_session_ = s_session;
    };

    void switch_path()
    {
        BOOST_LOG_TRIVIAL(info) << "PKTMigrator | switch_path : switch control path";
        Message message = prepare_switch(eproxy_data_path_port);
        s_session_->write(message);
    }

    void draw_back()
    {
        BOOST_LOG_TRIVIAL(info) << "PKTMigrator | draw_back : draw back control path proxy";
        Message message = prepare_drawback();
        s_session_->write(message);
    }

private:
    Message prepare_switch(unsigned port)
    {
        size_t length = htons(sizeof(ofp_flow_mod) + sizeof(ofp_match) + sizeof(ofp_instruction_actions) + sizeof(ofp_action_output));
        char * buffer = new char[length];
        ofp_flow_mod* flow_mod = (ofp_flow_mod *) buffer;
        flow_mod->header.version = OFP_VERSION;
        flow_mod->header.type = OFPT_FLOW_MOD;
        flow_mod->header.xid = 0x12345678;
        flow_mod->cookie = COOKIE_SWITCH;
        flow_mod->cookie_mask = 0;
        flow_mod->table_id = 0;
        flow_mod->command = OFPFC_ADD;
        flow_mod->idle_timeout = 0;
        flow_mod->hard_timeout = 0;
        flow_mod->priority = 1;
        flow_mod->buffer_id = OFP_NO_BUFFER;
        flow_mod->out_port = eproxy_data_path_port;
        flow_mod->out_group = OFPG_ANY;
        flow_mod->flags = 0;
        flow_mod->pad[0] = 0;
        flow_mod->pad[1] = 0;

        //oxm field 
        ofp_match * match = &flow_mod->match; 
        match->type = OFPMT_OXM;
        match->length = htons( sizeof(ofp_match) - 4);
        match->pad[0] = 0;
        match->pad[1] = 0;
        match->pad[2] = 0;
        match->pad[3] = 0;

        //instruction
        ofp_instruction_actions * instruction = (ofp_instruction_actions *)(buffer + sizeof(ofp_flow_mod));
        instruction->type = OFPIT_APPLY_ACTIONS;
        instruction->len = htons(sizeof(ofp_instruction_actions) + sizeof(ofp_action_output)); 
        instruction->pad[0] = 0;
        instruction->pad[1] = 0;
        instruction->pad[2] = 0;
        instruction->pad[3] = 0;
        
        //action
        ofp_action_output * action = (ofp_action_output *)(buffer + sizeof(ofp_flow_mod) + sizeof(ofp_instruction_actions));
        action->type = OFPAT_OUTPUT;
        action->len = htons(sizeof(ofp_action_output));
        action->port = htonl(port);
        action->max_len = OFPCML_NO_BUFFER;
        action->pad[0] = 0;
        action->pad[1] = 0;
        action->pad[2] = 0;
        action->pad[3] = 0;
        action->pad[4] = 0;
        action->pad[5] = 0;

        flow_mod->header.length = length;


        Message m = std::make_shared<std::string>(buffer, ntohs(flow_mod->header.length));
        return m;
    };

    Message prepare_drawback()
    {
        //fix
//        ofp_flow_mod flow_mod;
//        flow_mod.cookie = COOKIE_SWITCH;
//        flow_mod.cookie_mask = 0;
//        flow_mod.table_id = 0;
//        flow_mod.command = OFPFC_DELETE;
//        flow_mod.idle_timeout = 0;
//        flow_mod.hard_timeout = 0;
//        flow_mod.priority = 1;
//        flow_mod.buffer_id = OFP_NO_BUFFER;
//        flow_mod.out_port = eproxy_data_path_port;
//        flow_mod.out_group = OFPG_ANY;
//        flow_mod.flags = 0;
        Message message = std::make_shared<std::string>();
        return message;
    };

    switch_session* s_session_;
    unsigned int eproxy_data_path_port;
    const uint64_t COOKIE_SWITCH = 0x627;
};
