#ifndef DFA_STATE_H
#define DFA_STATE_H

#include "state.h"

class dfa_state : public state {
private:
    std::map <std::string, std::shared_ptr<dfa_state>> transitions;
public:
    dfa_state (int id, state_type type, std::vector<regular_definition> definitions);
    void insert_state (std::string input, std::shared_ptr<state> const& state) override;
    std::shared_ptr<dfa_state> get_next_state (char input);
    std::map <std::string, std::shared_ptr<dfa_state>> get_transitions();
};


#endif // DFA_STATE_H