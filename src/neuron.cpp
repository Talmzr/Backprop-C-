
#include "neuron.h"

    //class setting
    Neuron::Neuron(std::vector<std::shared_ptr<Node>> weights, std::shared_ptr<Node> value, std::shared_ptr<Node> bias): weights(weights), value(value), bias(bias){}

    std::vector<std::shared_ptr <Node>>& Neuron::get_weights() {return weights;}

    double Neuron::get_bias(){return bias->get_value();}

        
    void Neuron::set_weights_value(double newvalue, size_t index){
        weights[index]->set_value(newvalue);
    }

    void Neuron::reset_weights_gradient(size_t index){
        weights[index]->reset_gradient();
    }

    void Neuron::set_bias_value(double newbias_v){
        bias->set_value(newbias_v);        
    }

    void Neuron::reset_bias_gradient(){
        bias->reset_gradient();
    }


    void Neuron::set_value(double newvalue){
        value->set_value(newvalue);
    }

    std::shared_ptr<Node>& Neuron::get_value()
    {
        return value;
    }




    //class Layer

    Layer::Layer(std::vector<std::shared_ptr<Neuron>> layer): layer(layer){}


    std::vector<std::shared_ptr<Neuron>>& Layer::get_layer(){return layer;}




    //general functions

    std::vector<std::shared_ptr <Node>>&  Neuron::get_weights()
    {
        return weights;
    }

    std::shared_ptr<Node> weights_values_compute(std::vector<std::shared_ptr<Node>>& inputs, std::vector<std::shared_ptr<Node>>& weights)
    {

        std::vector<std::shared_ptr<Node>> node_chain = {inputs[0]  * inputs[1]};
            weights.push_back(inputs[1]);

        for (size_t i = 2; i<inputs.size(); i=i+2)
        {
            weights.push_back(inputs[i+1]);
            node_chain.push_back(node_chain.back() + inputs[i]  * inputs[i+1]);
                
        }

        return node_chain[inputs.size()/2 -1]  ;
    }


    std::shared_ptr<Node> neuron_compute(std::vector<std::shared_ptr<Node>>& inputs, std::vector<std::shared_ptr<Node>>& weights, std::shared_ptr<Node>& bias)
    {
        std::shared_ptr<Node> Z = weights_values_compute(inputs, weights)  + bias; 
        std::shared_ptr<Node> value = tanh(Z);
        return value;
    }



    std::shared_ptr<Neuron> neuron_creator(std::vector<std::shared_ptr<Node>>& inputs)
    {

        static std::mt19937 generator(42);                                   
        std::uniform_real_distribution<double> uniform(-1.0, 1.0);    
        std::shared_ptr<Node> bias = Node_Input(uniform(generator)); 

        std::vector<std::shared_ptr<Node>> weights;

        std::shared_ptr<Node> value = neuron_compute(inputs, weights, bias);

        std::shared_ptr n = std::make_shared<Neuron>(weights , value, bias);
        return n; 
    }




    void neuron_recompute(std::shared_ptr<Neuron> neuron, Layer prevlayer)
    {
        //fetch the values from the previous layer 
        std::vector<std::shared_ptr<Node>> values;

        for (size_t v = 0; v< prevlayer.get_layer().size(); v++)
        {
            values.push_back(prevlayer.get_layer()[v]->get_value());
        }

        if(!(values.size()==neuron->get_weights().size()))
        {
            std::cout << "values are not the same lenght of weights" << "\n";
        }
        

        double sum = 0;
        for (size_t i = 0; i<neuron->get_weights().size(); i++)
        {

            sum +=  neuron->get_weights()[i]->get_value() * values[i]->get_value();

        }

        
    
        neuron->set_value(tanh((sum + neuron->get_bias())));
        
    }



