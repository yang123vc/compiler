#ifndef STATE_H
#define STATE_H

#include "../nfa_tools/lexical_rules.h"

#include <map>
#include <vector>

typedef unsigned int state_id;

enum state_type
{
  ACCEPTANCE,
  START,
  INTERMEDIATE
};

class State
{
protected:
  state_id s_id;
  state_type type;
  std::vector <regular_definition> definitions;
public:
  State (int id, state_type type, std::vector<regular_definition> definitions);
  virtual std::shared_ptr<State> get_next_state (char input) = 0;
  virtual void insert_state (std::string input, State* state) = 0;
};

class NFA_State : public State
{
public:
    NFA_State (int id, state_type type, std::vector<regular_definition> definitions);
    std::shared_ptr<State> get_next_state (char input) override;
    void insert_state (std::string input, State* state) override;
private:
 	std::map <std::string, std::vector<NFA_State>> transitions;
};

class DFA_State : public State
{
public:
    DFA_State (int id, state_type type, std::vector<regular_definition> definitions);
    std::shared_ptr<State> get_next_state (char input) override;
    void insert_state (std::string input, State* state) override;
private:
 	std::map <std::string, DFA_State> transitions;
};

#endif // STATE_H