#ifndef STATE_H
#define STATE_H

#include "../nfa_tools/lexical_rules.h"

#include <map>
#include <vector>
#include <memory>

#define EPSILON ""
typedef unsigned int state_id;

enum state_type
{
  ACCEPTANCE,
  START,
  INTERMEDIATE
};

class state
{
protected:
    state_id id;
    state_type type;
    char_set state_input;
public:
    state (state_id id, state_type type, char_set st_ip);
    state(const state&);

    virtual void insert_transition (std::string input, std::shared_ptr<state> const& state) = 0;
    /**
     * makes a copy of the state which used in copying nfa.
     * @return shared_ptr to a copy state.
     */
    virtual std::shared_ptr<state> copy() = 0;

    // getters
    const state_id& get_id() const;
    const state_type& get_type() const;

    // setters
    void set_type(state_type);
    void set_id(state_id);
};

#endif // STATE_H
