#ifndef NFA_H
#define NFA_H


#include "finite_automata.h"
#include "nfa_state.h"

class nfa : public fa {
public:
    explicit nfa(std::shared_ptr<state> start_state,
        std::vector<std::shared_ptr<state>> acceptance_states, int total_states);
    explicit nfa(std::vector<regular_definition> defs, int id1, int id2);
    void dfs (std::shared_ptr<state> state, std::vector<bool> &visited,
              std::shared_ptr<std::ofstream> vis, bool update_acceptance_states) override;
    void unify(std::shared_ptr<nfa>);
    void concat(std::shared_ptr<nfa>);
    void plus();
    void star();

    void renamify(state_id starting_id);

    static std::vector<regular_definition> build_epsilon_transition();
};


#endif // NFA_H