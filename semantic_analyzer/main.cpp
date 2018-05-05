#include <iostream>
#include <vector>
#include "intermediate_code_generation/semantic_rules.h"
#include "../syntax_analyzer/predictive_parser.h"



int main (int argc, char *argv[]) {
// S -> .code s, .next in
// B -> .code s, .false in, .true in

// S[S.next] -> if ( @action B[.true, .false] {B.code}) then S1 {S1.code} {S.code}



//    initial stack:
//    A S{print A.n1, print A.n0} $

//    A -> B S{stack[top - 1].n0 = n0, stack[top - 1].n1 = n1} S{print A.n1, print A.n0}
//    B -> 0 B1 S{[top - 1].n0 = B1.n0 + 1, [top - 1].n1 = B1.n1}
//    B -> 1 B1 S{[top - 1].n1 = B1.n1 + 1, [top - 1].n0 = B1.n0}
//    B -> '\L' @{[top - 1].n0 = 0, [top - 1].n1 = 0}


    cfg_symbol A("A", NON_TERMINAL);
    cfg_symbol B("B", NON_TERMINAL);
    cfg_symbol B1("B", NON_TERMINAL);

    cfg_symbol s_0("0", TERMINAL);
    cfg_symbol s_1("1", TERMINAL);
    cfg_symbol eps(EPS, TERMINAL);

    int ones;
    int zeros;

    // SYNTHESISE RECORDS
    cfg_symbol synthesize_record1("record_B", SYNTHESISED);
    synthesize_record1.set_action(
            [] (std::vector<cfg_symbol>& stack) {
//                stack[top - 1].n0 = n0, stack[top - 1].n1 = n1
                stack[stack.size() - 2].add_attribute("n0", stack.back().get_attribute("n0"));
                stack[stack.size() - 2].add_attribute("n1", stack.back().get_attribute("n1"));
            }
    );

    cfg_symbol synthesize_record2("record_A", SYNTHESISED);
    synthesize_record2.set_action(
            [&ones, &zeros] (std::vector<cfg_symbol>& stack) {
//                S{print A.n1, print A.n0}
//                std::cout << stack.back().get_attribute("n1") << std::endl;
//                std::cout << stack.back().get_attribute("n0") << std::endl;
                zeros = std::atoi(stack.back().get_attribute("n0").c_str());
                ones = std::atoi(stack.back().get_attribute("n1").c_str());
            }
    );

    cfg_symbol synthesize_record3("record_B1_0", SYNTHESISED);
    synthesize_record3.set_action(
            [] (std::vector<cfg_symbol>& stack) {
//                S{[top - 1].n0 = B1.n0 + 1, [top - 1].n1 = B1.n1}
                stack[stack.size() - 2].
                        add_attribute("n0", std::to_string(std::atoi(stack.back().get_attribute("n0").c_str()) + 1));
                stack[stack.size() - 2].add_attribute("n1", stack.back().get_attribute("n1"));
            }
    );

    cfg_symbol synthesize_record4("record_B1_1", SYNTHESISED);
    synthesize_record4.set_action(
            [] (std::vector<cfg_symbol>& stack) {
                stack[stack.size() - 2].
                        add_attribute("n1", std::to_string(std::atoi(stack.back().get_attribute("n1").c_str()) + 1));
                stack[stack.size() - 2].add_attribute("n0", stack.back().get_attribute("n0"));
            }
    );

    cfg_symbol action1("@action1", ACTION);
    action1.set_action(
            [] (std::vector<cfg_symbol>& stack) {
//                @{[top - 1].n0 = 0, [top - 1].n1 = 0}
                stack[stack.size() - 2].add_attribute("n0", "0");
                stack[stack.size() - 2].add_attribute("n1", "0");
            }
    );


    std::vector<cfg_symbol> A_prod_vector;

    std::vector<cfg_symbol> B_prod1_vector;
    std::vector<cfg_symbol> B_prod2_vector;
    std::vector<cfg_symbol> B_prod3_vector;

    A_prod_vector.push_back(B);
    A_prod_vector.push_back(synthesize_record1);
    A_prod_vector.push_back(synthesize_record2);

    B_prod1_vector.push_back(s_0);
    B_prod1_vector.push_back(B1);
    B_prod1_vector.push_back(synthesize_record3);


    B_prod2_vector.push_back(s_1);
    B_prod2_vector.push_back(B1);
    B_prod2_vector.push_back(synthesize_record4);

    B_prod3_vector.push_back(eps);
    B_prod3_vector.push_back(action1);

    cfg_production prod_A(A, A_prod_vector);

    cfg_production prod1_B(B, B_prod1_vector);
    cfg_production prod2_B(B, B_prod2_vector);
    cfg_production prod3_B(B, B_prod3_vector);


    std::unordered_map<std::pair<std::string, std::string>, cfg_production,
            parsing_table_hasher, parsing_table_comparator> table;

    table[{"A", "0"}] = prod_A;
    table[{"A", "1"}] = prod_A;
    table[{"A", "$"}] = prod_A;

    table[{"B", "0"}] = prod1_B;
    table[{"B", "1"}] = prod2_B;
    table[{"B", "$"}] = prod3_B;

    std::shared_ptr<parsing_table> p_table = std::make_shared<parsing_table>(table);
    p_table->draw("parsing_table.txt");

    std::vector<std::string> input_buffer{
            "0",
            "1",
            "1",
            "1",
            "1",
            "1",
            "$"};

    predictive_parser parser(A, p_table, input_buffer);
    parser.parse();

    parser.write_derivations("actions.log");
    parser.write_debug_stack("debug_stack.log");


    std::cout << "number of zeros = " << zeros << std::endl;
    std::cout << "number of ones = " << ones << std::endl;
}