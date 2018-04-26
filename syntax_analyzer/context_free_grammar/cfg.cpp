#include "cfg.h"
#include <fstream>
#include <utility>
#include <iostream>
#include <string>

cfg::cfg ()
    : non_terminals (), terminals (), rules () {

}

cfg::cfg (std::string grammar_file)
    : non_terminals (), terminals (), rules () {

}



void cfg::parse (std::string grammar_file) {
    std::ifstream grammar_in_file (grammar_file.c_str ());
    // Checks if grammar file exists or not.
    if (!grammar_in_file.good ()) {
        //TODO: Report "File doesn't exist!" error.
    }
    std::string current_line, previous_line;
    bool first_line = true;
    while (std::getline (grammar_in_file, current_line)) {
        if (first_line) {
            previous_line += current_line;
            first_line = false;
        } else {
            if (current_line[0] == '#') {
                std::cout << previous_line << std::endl;
                //TODO: Process the rule line.
                previous_line.clear ();
                previous_line += current_line;
            } else {
                previous_line += " " + current_line;
            }
        }
    }
}
void cfg::add_rule () {

}

void cfg::left_factor ()
{
    // for (auto rule : rules)
    // {
    //     std::map <std::vector<std::string>, std::vector <cfg_production>> common_factors;
    //     for (auto prod : rule.get_productions())
    //     {
    //         std::std::vector<std::string> v;
    //         v.push_back(prod.get_symbols()[0].get_name());
    //         common_factors[v].push_back (prod);
    //     }
    //     rule.empty_productions ();
    //     size_t counter = 1;
    //     std::vector <cfg_production> new_productions;
    //     for (auto entry : common_factors)
    //     {
    //         if (entry.second.size() == 1)
    //         {
    //             new_productions.push_back (entry.second.front());
    //         }
    //         else
    //         {
    //             std::vector<cfg_symbol> v;
    //             int i;
    //             for (i = 0; i < entry.first.size(); i++)
    //             {
    //                v.push_back(entry.second.front()[i]);
    //             }
    //             cfg_symbol new_sym(rule.get_lhs_symbol().get_name()
    //                 + std::to_string(counter), rule.get_lhs_symbol().get_type())
    //             v.push_back(new_sym);
    //             cfg_production new_prod(rule.get_lhs_symbol(), v);
    //             new_productions.push_back (new_prod);
    //
    //             std::vector<cfg_symbol> rest_symbols;
    //             add_rule(new_sym, )
    //
    //         }
    //     }
    // }
}

void cfg::remove_left_recursion ()
{

}

std::vector <cfg_symbol> cfg::get_non_terminals () {
    return non_terminals;
}

std::vector <cfg_symbol> cfg::get_terminals () {
    return terminals;
}

std::vector <cfg_rule> cfg::get_rules () {
    return rules;
}

cfg_symbol cfg::get_start_symbol () {
    return start_symbol;
}

bool cfg::is_ll_1 () {
    return true;
}

void
cfg::process_first_set(int prod_symbol_index, std::shared_ptr<cfg_set> first_set,
                       std::shared_ptr<cfg_production> prod) {
    if (prod_symbol_index == prod->get_symbols().size()) {
        return;
    }
    auto curr_prod_symbol = prod->get_symbols()[prod_symbol_index];

    // Cycle case
    if (curr_prod_symbol.get_name() == prod->get_lhs_symbol().get_name()) {
        return;
    }

    if (curr_prod_symbol.get_type() == TERMINAL) {
        first_set->add_symbol(curr_prod_symbol.get_name(), curr_prod_symbol, prod);
        first_set->add_symbol(prod->get_lhs_symbol().get_name(), curr_prod_symbol, prod);
        return;
    }

    // If current prod symbol isn't processed yet, process it.
    if (first_set->empty(curr_prod_symbol.get_name())) {
        // Go to curr_prod_symbol rule and iterate over all its prods and calc its first set.
        for (auto production : cfg::grammar[curr_prod_symbol].get_productions()) {
            process_first_set(0, first_set, std::shared_ptr<cfg_production>(&production));
        }
    }

    // Here curr_proc_symbol's first_set should be already processed.
    auto symbols_map = first_set->get_set_map();
    for (auto symbol : symbols_map[curr_prod_symbol.get_name()]) {
        // Add all symbols of curr_prod_symbol to lhs symbol's first set
        first_set->add_symbol(prod->get_lhs_symbol().get_name(), symbol.first, prod);
    }

    // If curr_prod_symbol has eps in its first set, then continue processing this production.
    if (first_set->has_eps(curr_prod_symbol.get_name())) {
        cfg_symbol eps(EPS, TERMINAL); // TODO: Make this a constant.
        first_set->add_symbol(prod->get_lhs_symbol().get_name(), eps, prod);
        process_first_set(prod_symbol_index + 1, first_set, prod);
    }
}

std::shared_ptr<cfg_set> cfg::get_first_set() {
    std::shared_ptr<cfg_set> first_set = std::make_shared<cfg_set>();

    /// Build first set
    for (auto non_terminal : cfg::non_terminals) {
        std::cout << "Current non_terminal = " << non_terminal.get_name() << "\n";
        if (!first_set->empty(non_terminal.get_name())) {
            continue;
        }
        auto rule = cfg::grammar[non_terminal];
        std::cout << "Current rule has lhs = " << rule.get_lhs_symbol().get_name() << "\n";
        for (auto production : cfg::grammar[non_terminal].get_productions()) { // Iterate over all productions from this non-terminal
            std::cout << "Current production's lhs = " << production.get_lhs_symbol().get_name() << "\n";
            process_first_set(0, first_set, std::shared_ptr<cfg_production>(&production));
        }
    }
    return first_set;
}

const std::unordered_set <cfg_symbol, cfg_symbol::hasher, cfg_symbol::comparator>
                        & cfg::get_cfg_symbols () const {
    return cfg::cfg_symbols;
}

void cfg::set_cfg_symbols (
        const std::unordered_set <cfg_symbol, cfg_symbol::hasher
            , cfg_symbol::comparator> & cfg_symbols) {
    cfg::cfg_symbols = cfg_symbols;
}

void cfg::process_follow_set(cfg_symbol non_terminal, std::shared_ptr<cfg_set> follow_set) {
    if (non_terminal.get_name() == cfg::start_symbol.get_name()) {
        cfg_symbol s_$(END_MARKER);
        follow_set->add_symbol(non_terminal.get_name(), s_$, nullptr);
    }
    for (auto production : cfg::cfg_symbol_productions[non_terminal]) {
        for (int i = 0; i < production.get_symbols().size(); i++) {
            auto symbol = production.get_symbols()[i];
            if (symbol.get_name() == non_terminal.get_name()) {
                if (i + 1 == production.get_symbols().size()) {
                    // This non_terminal is the last symbol in the production.
                    if (follow_set->empty(production.get_lhs_symbol().get_name())) {
                        // Calc Follow(lhs) if not calculated
                        process_follow_set(production.get_lhs_symbol(), follow_set);
                    }
                    // Add everything in Follow(lhs) to Follow(non_terminal)
                    /// TODO: Make this a function in the cfg_set class.
                    auto follow_set_map = follow_set->get_set_map();
                    for (auto symbol : follow_set_map[production.get_lhs_symbol().get_name()]) {
                        follow_set->add_symbol(non_terminal.get_name(), symbol.first, nullptr);
                    }
                } else {
                    // There is a symbol after this non_terminal in the production.

                    bool has_eps = true;
                    int curr_pos = i + 1;
                    while (has_eps) {
                        if (curr_pos == production.get_symbols().size()) {
                            break;
                        }
                        has_eps = false;
                        // Put everything in first of the next symbol in the follow of non_terminal
                        auto cfg_first_set_map = cfg::get_first_set()->get_set_map();
                        auto next_first_set = cfg_first_set_map[production.get_symbols()[curr_pos++].get_name()];
                        for (auto symbol : next_first_set) {
                            if (symbol.first.get_name() != EPS) {
                                follow_set->add_symbol(non_terminal.get_name(), symbol.first, nullptr);
                            } else {
                                has_eps = true;
                            }
                        }
                    }
                    if (has_eps) {
                        // If eps is in first of last symbol, add everything in Follow(lhs) to Follow(non_terminal)
                        if (follow_set->empty(production.get_lhs_symbol().get_name())) {
                            // Calc Follow(lhs) if not calculated
                            process_follow_set(production.get_lhs_symbol(), follow_set);
                        }
                        // Add everything in Follow(lhs) to Follow(non_terminal)
                        // TODO: Make this a function in cfg_set
                        auto follow_set_map = follow_set->get_set_map();
                        for (auto symbol : follow_set_map[production.get_lhs_symbol().get_name()]) {
                            follow_set->add_symbol(non_terminal.get_name(), symbol.first, nullptr);
                        }
                    }
                }
                break;
            }
        }
    }
}

std::shared_ptr<cfg_set> cfg::get_follow_set() {
    std::shared_ptr<cfg_set> follow_set = std::make_shared<cfg_set>();
    for (auto non_terminal : cfg::non_terminals) {
        if (!follow_set->empty(non_terminal.get_name())) {
            continue;
        }
        process_follow_set(non_terminal, follow_set);
    }
    return follow_set;
}

void cfg::set_non_terminals(const std::vector<cfg_symbol> &non_terminals) {
    cfg::non_terminals = non_terminals;
}

const std::unordered_map<cfg_symbol, cfg_rule, cfg_symbol::hasher, cfg_symbol::comparator> &cfg::get_grammar() const {
    return grammar;
}

void
cfg::set_grammar(const std::unordered_map<cfg_symbol, cfg_rule, cfg_symbol::hasher, cfg_symbol::comparator> &grammar) {
    cfg::grammar = grammar;
}
