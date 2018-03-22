#include <iostream>
#include <utility>
#include "nfa.h"
#include "nfa_state.h"

nfa::nfa(std::shared_ptr<state> start_state, std::vector<std::shared_ptr<state>> acceptance_states, int total_states)
        : fa(start_state, acceptance_states, total_states) {
    nfa::start_state = start_state;
    nfa::acceptance_states = acceptance_states;
    nfa::total_states = total_states;
}

nfa::nfa(std::vector<regular_definition> defs, int id1, int id2)
    : fa()
{
    std::vector<regular_definition> eps = build_epsilon_transition();

    std::shared_ptr<nfa_state> s0 = std::make_shared<nfa_state>(nfa_state (id1, START, defs));
    std::shared_ptr<nfa_state> sf = std::make_shared<nfa_state>(nfa_state (id2, ACCEPTANCE, eps));

    for (auto rd : defs)
    {

        for (auto c : rd.sequence.get_characters())
        {
//            std::cout << s0->get_id() << "--" << c.first << "-->" << sf->get_id() << std::endl;
            s0->insert_state (std::string("") + c.first, sf);
        }

        for (auto range : rd.sequence.get_ranges())
        {
            s0->insert_state (range->get_range_string(), sf);
        }
    }

    start_state = s0;
    acceptance_states.push_back(sf);
}

void nfa::dfs (std::shared_ptr<state> curr_state, std::vector<bool> &visited,
               std::shared_ptr<std::ofstream> vis, bool update_acceptance_states)
{
    visited[curr_state->get_id()] = true;
    if (update_acceptance_states && curr_state->get_type() == ACCEPTANCE)
    {
        acceptance_states.push_back(curr_state);
    }

    std::map<std::string, std::vector<std::shared_ptr<nfa_state>>> transitions
            = std::static_pointer_cast<nfa_state>(curr_state)->get_transitions();

//    std::cout << "Current state = " << curr_state->get_id() << "\n";
//    for (auto trans : transitions) {
//        std::cout << "Key = " << trans.first << ", Destinations: ";
//        for (auto curr : trans.second)
//        {
//            std::cout << curr->get_id() << " ";
//        }
//        std::cout << "\n";
//    }
//    std::cout << "\n";

    for (auto edge : transitions)
    {
        std::string label = edge.first;
        std::vector<std::shared_ptr<nfa_state>> next_states = edge.second;
        for (auto state : next_states)
        {
            // Visualize
            if (vis != nullptr) {
                if (label.empty()) {
                    label = "ϵ";
                }
                *vis << curr_state->get_id() << " -> " << state->get_id() << " [ label = \"" << label << "\" ];\n";
            }
            if (!visited[state->get_id()]) {
                dfs(state, visited, vis, update_acceptance_states);
            }
        }
    }
}


void nfa::unify(std::shared_ptr<nfa> nfa2)
{
    std::vector<regular_definition> v;
    regular_definition eps;
    char_set empty_char_set;

    eps.name = "eps";
    eps.sequence = empty_char_set;

    v.push_back(eps);

    std::shared_ptr<nfa_state> s0 = std::make_shared<nfa_state>(nfa_state (0, START, v));
    std::shared_ptr<nfa_state> sf = std::make_shared<nfa_state>(nfa_state (5, ACCEPTANCE, v));

    std::shared_ptr<nfa_state> nfa2_s0 = std::static_pointer_cast<nfa_state>(nfa2->get_start_state());
    std::shared_ptr<nfa_state> nfa1_s0 = std::static_pointer_cast<nfa_state>(start_state);

    nfa1_s0->set_type(INTERMEDIATE);
    nfa2_s0->set_type(INTERMEDIATE);

    s0->insert_state(EPSILON, nfa1_s0);
    s0->insert_state(EPSILON, nfa2_s0);

    
    std::shared_ptr<nfa_state> nfa2_sf = std::static_pointer_cast<nfa_state>(nfa2->get_acceptance_states().front());
    std::shared_ptr<nfa_state> nfa1_sf = std::static_pointer_cast<nfa_state>(acceptance_states.front());

    nfa2_sf->set_type(INTERMEDIATE);
    nfa1_sf->set_type(INTERMEDIATE);

    nfa1_sf->insert_state(EPSILON, sf);
    nfa2_sf->insert_state(EPSILON, sf);

    start_state = s0;
    
    acceptance_states.clear();
    acceptance_states.push_back(sf);
}

void nfa::concat(std::shared_ptr<nfa> nfa2)
{
    acceptance_states.front()->insert_state(EPSILON, nfa2->get_start_state());
      
    nfa2->get_start_state()->set_type(INTERMEDIATE);
    get_acceptance_states().front()->set_type(INTERMEDIATE);
  
    acceptance_states.clear();
    acceptance_states.push_back(nfa2->get_acceptance_states().front());
}

void nfa::plus()
{
    std::shared_ptr<nfa> nfa2(new nfa(*this));
    nfa2->update_acceptance_states();
    nfa2->renamify(acceptance_states.front()->get_id() + 1);
    nfa2->star();
    concat(nfa2);
}

void nfa::star()
{
    start_state->insert_state(EPSILON, acceptance_states.front());
    acceptance_states.front()->insert_state(EPSILON, start_state);
}

std::vector<regular_definition> nfa::build_epsilon_transition()
{
    std::vector<regular_definition> v;
    regular_definition eps;
    eps.name = "eps";
    char_set empty_char_set;
    eps.sequence = empty_char_set;
    v.push_back(eps);
    return v;
}


void renamify_dfs (std::shared_ptr<state> curr_state, std::map<std::shared_ptr<state>, bool> &visited,
                   std::shared_ptr<std::ofstream> vis, state_id id)
{
    visited[curr_state] = true;

    std::map<std::string, std::vector<std::shared_ptr<nfa_state>>> transitions
            = std::static_pointer_cast<nfa_state>(curr_state)->get_transitions();

    for (auto edge : transitions)
    {
        std::string label = edge.first;
        std::vector<std::shared_ptr<nfa_state>> next_states = edge.second;
        for (auto state : next_states)
        {
            if (!visited[state]) {
                state->set_id(id);
                std::cout << state->get_id() << std::endl;
                renamify_dfs(state, visited, vis, id + 1);
            }
        }
    }
}

void nfa::renamify (state_id starting_id)
{
//    std::map<std::shared_ptr<state>, bool> visited;
//    renamify_dfs (start_state, visited, nullptr, starting_id);
    start_state->set_id(starting_id);
    acceptance_states.front()->set_id(starting_id + 1);
}