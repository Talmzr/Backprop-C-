
#pragma once

#include "node.h"




class Neuron{

    public:
        Neuron(std::vector<std::shared_ptr<Node>> weights, std::shared_ptr<Node> value, std::shared_ptr<Node> bias);
        double get_bias();
        
        std::vector <std::shared_ptr <Node>>& get_weights();

        void set_weights_value(double newvalue, size_t index);
        void reset_weights_gradient(size_t index);

        void set_bias_value(double newbias_v);
        void reset_bias_gradient();

        void set_value(double newvalue);
        std::shared_ptr<Node>& get_value();

    private:
    std::vector<std::shared_ptr<Node>> weights;
    
    std::shared_ptr<Node> value;
    std::shared_ptr<Node> bias;
    
    
};

    std::shared_ptr<Node> weights_values_compute(std::vector<std::shared_ptr<Node>>& inputs, std::vector<std::shared_ptr<Node>>& weights);

    std::shared_ptr<Node> neuron_compute(std::vector<std::shared_ptr<Node>>& inputs, std::vector<std::shared_ptr<Node>>& weights, std::shared_ptr<Node>& bias);

    std::shared_ptr<Neuron> neuron_creator(std::vector<std::shared_ptr<Node>>& inputs);











    class Layer{
        public:
        Layer(std::vector<std::shared_ptr<Neuron>> layer);

       std::vector<std::shared_ptr<Neuron>>& get_layer();

        private:

        std::vector<std::shared_ptr<Neuron>> layer;

    };