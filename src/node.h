#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <ranges>
#include <cmath>



enum class Operation{add, multiply, subtract, divide, negate, power, exponent, log, tanh,relu, None};



class Node{
    public:
        Node(Operation op, double val,const std::vector <std::shared_ptr <Node>>& Parents);
        

        double get_value() const;
        double get_gradient() const;
        Operation get_operation() const;
        const std::vector <std::shared_ptr <Node>>& get_parents() const;

        void add_to_gradient(double newGradient);
        void set_value(double newvalue);



    private:
        const std::vector <std::shared_ptr <Node>> PrevLayer;
        double value;
        double Gradient;
        const Operation nodeop;
};



double ValArithm (Operation oparith,const std::shared_ptr <Node>& parent1,const std::shared_ptr <Node>& parent2);


    std::shared_ptr<Node> Node_child(Operation oper,const std::shared_ptr <Node>& parent1,const std::shared_ptr <Node>& parent2);



    std::shared_ptr<Node> operator+(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2);

    std::shared_ptr<Node> operator*(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2);

    std::shared_ptr<Node> operator-(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2);

    std::shared_ptr<Node> operator/(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2);

    std::shared_ptr<Node> operator-(const std::shared_ptr<Node>& node); //negate

    std::shared_ptr<Node> power(const std::shared_ptr<Node>& node, double exponent);

    std::shared_ptr<Node> exp(const std::shared_ptr<Node>& node);

    std::shared_ptr<Node> log(const std::shared_ptr<Node>& node);

    std::shared_ptr<Node> tanh(const std::shared_ptr<Node>& node);

    std::shared_ptr<Node> relu(const std::shared_ptr<Node>& node);

    std::shared_ptr<Node> operator+(const std::shared_ptr<Node>& parent1, double number);
    std::shared_ptr<Node> operator-(const std::shared_ptr<Node>& parent1, double number);
    std::shared_ptr<Node> operator*(const std::shared_ptr<Node>& parent1, double number);
    std::shared_ptr<Node> operator/(const std::shared_ptr<Node>& parent1, double number);
    std::shared_ptr<Node> operator+(double number,const std::shared_ptr<Node>& parent1);
    std::shared_ptr<Node> operator-(double number,const std::shared_ptr<Node>& parent1);
    std::shared_ptr<Node> operator*(double number,const std::shared_ptr<Node>& parent1);
    std::shared_ptr<Node> operator/(double number,const std::shared_ptr<Node>& parent1);




    std::shared_ptr<Node> Node_Input(double value);

    void graph(const std::shared_ptr<Node>& node);

    void gradient_p_addition(const std::shared_ptr<Node>& node);
    void gradient_p_multiply(const std::shared_ptr<Node>& node);
    void gradient_p_subtract(const std::shared_ptr<Node>& node);
    void gradient_p_divide(const std::shared_ptr<Node>& node);
    void gradient_p_negate(const std::shared_ptr<Node>& node);
    void gradient_p_power(const std::shared_ptr<Node>& node);
    void gradient_p_exponent(const std::shared_ptr<Node>& node);
    void gradient_p_log(const std::shared_ptr<Node>& node);
    void gradient_p_tanh(const std::shared_ptr<Node>& node);
    void gradient_p_relu(const std::shared_ptr<Node>& node);


    void topological(const std::shared_ptr<Node>& node, std::unordered_set<std::shared_ptr<Node>>& seennodes, std::vector<std::shared_ptr<Node>>& topoarray);

    void backward(const std::shared_ptr<Node>& node);

    void get_inputs(std::vector<std::shared_ptr<Node>>& topoarray, std::vector<std::shared_ptr<Node>>& inputarray);

    double replay_network(const std::shared_ptr<Node>& node);
    
    double Gradient_checker(std::shared_ptr<Node>& input, double h, const std::shared_ptr<Node>& output);

