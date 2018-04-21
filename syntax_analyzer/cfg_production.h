#ifndef COMPILER_CFG_PRODUCTION_H
#define COMPILER_CFG_PRODUCTION_H

#include <vector>
#include <memory>
#include "cfg_symbol.h"
#include "cfg_rule.h"

class cfg_rule;
class cfg_symbol;

class cfg_production
{
private:
    std::string name;
    std::vector<cfg_symbol> prod;
    std::shared_ptr<cfg_rule> rule; // parent rule
public:
    /// getters
    std::string get_name();
    std::vector<cfg_symbol> get_symbols();


};

#endif //COMPILER_CFG_PRODUCTION_H