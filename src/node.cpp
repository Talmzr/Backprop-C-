#include "node.h"







    Node::Node(Operation op, double val,const std::vector <std::shared_ptr <Node>>& Parents) : PrevLayer(Parents), value(val),Gradient(0),nodeop(op){
        }

    double Node::get_value() const {return value;}
    double Node::get_gradient() const {return Gradient;}
    Operation Node::get_operation() const {return nodeop;}
    const std::vector <std::shared_ptr <Node>>& Node::get_parents() const {return PrevLayer;}

    void Node::add_to_gradient(double newGradient) {Gradient += newGradient;}
    void Node::set_value(double newvalue) {value = newvalue;}


double ValArithm (Operation oparith,const std::shared_ptr <Node>& parent1,const std::shared_ptr <Node>& parent2){

    switch(oparith)
    {
        case Operation::add:    return (parent1->get_value() + parent2->get_value());
        case Operation::multiply:    return (parent1->get_value() * parent2->get_value());
        case Operation::subtract:   return (parent1->get_value() - parent2->get_value());
        case Operation::divide:    return (parent1->get_value() / parent2->get_value());
    }

    throw std::logic_error("Unable to compute with no operation");
}


    std::shared_ptr<Node> Node_child(Operation oper,const std::shared_ptr <Node>& parent1,const std::shared_ptr <Node>& parent2){

        double tempval = ValArithm(oper, parent1,parent2);
        std::vector <std::shared_ptr <Node>> parents = {parent1 , parent2};

        std::shared_ptr<Node> p = std::make_shared<Node>(oper,tempval, parents);
        return p;
    }



    std::shared_ptr<Node> operator+(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2){
        return Node_child(Operation::add , parent1,parent2);
    }

    std::shared_ptr<Node> operator*(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2){
        return Node_child(Operation::multiply , parent1,parent2);
    }

    std::shared_ptr<Node> operator-(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2){
        return Node_child(Operation::subtract , parent1,parent2);
    }

    std::shared_ptr<Node> operator/(const std::shared_ptr<Node>& parent1, const std::shared_ptr<Node>& parent2){
        return Node_child(Operation::divide , parent1,parent2);
    }

    std::shared_ptr<Node> operator-(const std::shared_ptr<Node>& node)
    {
        std::vector<std::shared_ptr<Node>> parent = {node};
        std::shared_ptr<Node> p = std::make_shared<Node>(Operation::negate, node->get_value() * (-1), parent);
        return p;
    }
    std::shared_ptr<Node> power(const std::shared_ptr<Node>& node, double exponent) //need to always to put in brackets to be used correctly
    {
        std::shared_ptr<Node> pow_node = Node_Input(exponent);
        std::vector<std::shared_ptr<Node>> parent = {node, pow_node};
        double exp = std::pow(node->get_value(), exponent);
        std::shared_ptr<Node> p = std::make_shared<Node>(Operation::power,exp, parent);
        return p;
    }
    std::shared_ptr<Node> exp(const std::shared_ptr<Node>& node)
    {
        double expon = std::exp(node->get_value());
        std::vector<std::shared_ptr<Node>> parent = {node};
        std::shared_ptr<Node> pow_node = std::make_shared<Node>(Operation::exponent, expon, parent);
        return pow_node;
    }
    std::shared_ptr<Node> log(const std::shared_ptr<Node>& node)
    {
        double expon = std::log(node->get_value());
        std::vector<std::shared_ptr<Node>> parent = {node};
        std::shared_ptr<Node> log_node = std::make_shared<Node>(Operation::log, expon, parent);
        return log_node;
    }
    std::shared_ptr<Node> tanh(const std::shared_ptr<Node>& node)
    {
        double expon = std::tanh(node->get_value());
        std::vector<std::shared_ptr<Node>> parent = {node};
        std::shared_ptr<Node> tanh_node = std::make_shared<Node>(Operation::tanh, expon, parent);
        return tanh_node;
    }
    std::shared_ptr<Node> relu(const std::shared_ptr<Node>& node)
    {
        std::vector<std::shared_ptr<Node>> parent = {node};
        if (node->get_value() > 0)
        {
            std::shared_ptr<Node> relu_node = std::make_shared<Node>(Operation::relu, node->get_value(), parent);
            return relu_node;
        }

        return std::make_shared<Node>(Operation::relu, 0, parent);
    }



    std::shared_ptr<Node> operator+(const std::shared_ptr<Node>& parent1, double number){
        return parent1 + Node_Input(number);
    }
    std::shared_ptr<Node> operator-(const std::shared_ptr<Node>& parent1, double number){
        return parent1 - Node_Input(number);
    }
    std::shared_ptr<Node> operator*(const std::shared_ptr<Node>& parent1, double number){
        return parent1 * Node_Input(number);
    }
    std::shared_ptr<Node> operator/(const std::shared_ptr<Node>& parent1, double number){
        return parent1 / Node_Input(number);
    }
    
    std::shared_ptr<Node> operator+(double number,const std::shared_ptr<Node>& parent1){
        return parent1 + Node_Input(number);
    }
    std::shared_ptr<Node> operator-(double number,const std::shared_ptr<Node>& parent1){
        return Node_Input(number) - parent1;
    }
    std::shared_ptr<Node> operator*(double number,const std::shared_ptr<Node>& parent1){
        return parent1 * Node_Input(number);
    }
    std::shared_ptr<Node> operator/(double number,const std::shared_ptr<Node>& parent1){
       return Node_Input(number)/ parent1 ;
    }











    std::shared_ptr<Node> Node_Input(double value){

        std::vector <std::shared_ptr<Node>> emp = {};
        std::shared_ptr<Node> p = std::make_shared<Node>(Operation::None,value, emp);
        return p;
    }
    

    void graph(const std::shared_ptr<Node>& node)
        {
                //the for loop doesn't run if there are no parents so the statement if parent>0 is not necessery
                for (const std::shared_ptr<Node>& n : node->get_parents())
                {
                    graph(n);
                }
            
            std::string operation;

            switch(node->get_operation()){
                case Operation::add:     operation = "add"; break;
                case Operation::multiply: operation = "multiply"; break;
                case Operation::subtract:     operation = "subtract"; break;
                case Operation::divide: operation = "divide"; break;
                case Operation::negate: operation = "negate"; break;
                case Operation::power: operation = "power"; break;
                case Operation::exponent: operation = "exponent"; break;
                case Operation::log: operation = "log"; break;
                case Operation::tanh: operation = "tanh"; break; 
                case Operation::relu: operation = "relu"; break;
                case Operation::None: operation  = "None"; break;
            }
            std::cout << " Node: " << node << " Value: " << node->get_value() << " op: " << operation << " Parents: ";

            for (const std::shared_ptr<Node>& j : node->get_parents()){
                std::cout << j << " ";
            }

            std::cout << " Gradient " << node->get_gradient();

            std::cout << "\n";

        }



        void topological(const std::shared_ptr<Node>& node, std::unordered_set<std::shared_ptr<Node>>& seennodes, std::vector<std::shared_ptr<Node>>& topoarray)
        { //return an array that starts with inputs and ends with outputs
            
                for (const std::shared_ptr<Node>& n : node->get_parents())
                {   
                    if(!seennodes.contains(n))
                    {
                    topological(n,seennodes,topoarray);
                    }
                }
            

            seennodes.insert(node);
            topoarray.push_back(node); //does this actually take place in the vector and set as they are the adresses to them

        }



        void gradient_p_addition(const std::shared_ptr<Node>& node) //how is the gradient is affected if parents were added
        {
           for(std::size_t n=0; n < (node->get_parents()).size(); n++)
            {
                (node->get_parents()[n])->add_to_gradient(node->get_gradient());
            } 
        }

        void gradient_p_multiply(const std::shared_ptr<Node>& node) //how is gradient is affected if parents were multi
        {
            for(std::size_t n=0; n < (node->get_parents()).size(); n++)
            {
                for (std::size_t j=0; j < (node->get_parents()).size(); j++)
                {
                (node->get_parents()[n])->add_to_gradient(node->get_gradient()*node->get_parents()[j]->get_value());
                }
            (node->get_parents()[n])->add_to_gradient(-(node->get_gradient()*node->get_parents()[n]->get_value()));
            }
        }

        void gradient_p_subtract(const std::shared_ptr<Node>& node) //how is gradient is affected if parents were multi
        {
            (node->get_parents()[0])->add_to_gradient(node->get_gradient());
            (node->get_parents()[1])->add_to_gradient(-node->get_gradient());

        }

        void gradient_p_divide(const std::shared_ptr<Node>& node) //how is gradient is affected if parents were multi
        {
            (node->get_parents()[0])->add_to_gradient(node->get_gradient()/node->get_parents()[1]->get_value());
            (node->get_parents()[1])->add_to_gradient(node->get_gradient()*(-node->get_parents()[0]->get_value()/ (node->get_parents()[1]->get_value()* node->get_parents()[1]->get_value())));

        }

        void gradient_p_negate(const std::shared_ptr<Node>& node)
        {
            node->get_parents()[0]->add_to_gradient(node->get_gradient()*-1);
        }


        void gradient_p_power(const std::shared_ptr<Node>& node) //gradient * exponent * node to power of exponent -1 
        {
            node->get_parents()[0]->add_to_gradient(node->get_gradient()* node->get_parents()[1]->get_value() * std::pow(node->get_parents()[0]->get_value(), node->get_parents()[1]->get_value()-1));
        }

        void gradient_p_exponent(const std::shared_ptr<Node>& node)
        {
            node->get_parents()[0]->add_to_gradient(node->get_gradient()*node->get_value());
        }
        
        void gradient_p_log(const std::shared_ptr<Node>& node)
        {
            node->get_parents()[0]->add_to_gradient(node->get_gradient() * 1/node->get_parents()[0]->get_value());
        }

        void gradient_p_tanh(const std::shared_ptr<Node>& node)
        {
            node->get_parents()[0]->add_to_gradient(node->get_gradient() * (1 - std::pow(node->get_value(),2)));
        }

        void gradient_p_relu(const std::shared_ptr<Node>& node)
        {
            if (node->get_value() > 0)
            {
                node->get_parents()[0]->add_to_gradient(node->get_gradient());
            }
        }




        void backward(const std::shared_ptr<Node>& node)
        {   
        
            std::unordered_set<std::shared_ptr<Node>> set;
            std::vector<std::shared_ptr<Node>> topo;

            topological(node, set, topo);

            topo[topo.size() - 1]->add_to_gradient(1);

            for (std::size_t i = topo.size(); i>0; i-- )
            {

                if (topo[i-1]->get_operation() == Operation::add)
                {
                    gradient_p_addition(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() == Operation::multiply)
                {
                    gradient_p_multiply(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() == Operation::subtract)
                {
                    gradient_p_subtract(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() == Operation::divide)
                {
                    gradient_p_divide(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() == Operation::negate)
                {
                    gradient_p_negate(topo[i-1]);
                }
                else if(topo[i-1]->get_operation() == Operation::power)
                {
                    gradient_p_power(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() == Operation::exponent)
                {
                    gradient_p_exponent(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() == Operation::log)
                {
                    gradient_p_log(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() ==Operation::tanh)
                {
                    gradient_p_tanh(topo[i-1]);
                }
                else if (topo[i-1]->get_operation() ==Operation::relu)
                {
                    gradient_p_relu(topo[i-1]);
                }

            }

        }


        void get_inputs(std::vector<std::shared_ptr<Node>>& topoarray, std::vector<std::shared_ptr<Node>>& inputarray)
        {
            for (const std::shared_ptr<Node> n : topoarray)
            {
                if (n->get_operation() == Operation::None)
                {
                    inputarray.push_back(n);
                }

            }
            // this gives in order the inputs of a given tree.
            // the only way I access a tree is recurtion thus that is the order. 
        }



        double replay_network(const std::shared_ptr<Node>& node)
        {

        if (node->get_parents().size()>0)
        {
            node->set_value(0);
        }
        else
        {
            return node->get_value(); //if this is an input no operation needs to be done on it
        }

        for (const std::shared_ptr<Node>& n : node->get_parents())
                {
                    replay_network(n);
                }


        bool first_zero_m = true;
        bool first_zero_d = true;

        
        
            if(node->get_operation() == Operation::add)
            {
                node->set_value(node->get_value() + node->get_parents()[0]->get_value() + node->get_parents()[1]->get_value());
            }
            else if(node->get_operation() == Operation::multiply)
            {
                
                if(node->get_value() == 0  && first_zero_m == true)
                {
                    first_zero_m = false;
                    node->set_value(1);
                }
                node->set_value(node->get_value()* node->get_parents()[0]->get_value()* node->get_parents()[1]->get_value());
            }
            else if(node->get_operation() == Operation::subtract)
            {
                node->set_value(node->get_value() + node->get_parents()[0]->get_value() - node->get_parents()[1]->get_value());
            }
            else if(node->get_operation() == Operation::divide)
            {
                
                if(node->get_value() == 0  && first_zero_d == true)
                {
                    first_zero_d = false;
                    node->set_value(1);
                }
                node->set_value(node->get_value()* node->get_parents()[0]->get_value()/node->get_parents()[1]->get_value());
            }
            else if (node-> get_operation() == Operation::negate)
            {
                node->set_value(node->get_parents()[0]->get_value()*-1);
            }
            else if (node->get_operation() == Operation::power)
            {
                node->set_value(pow(node->get_parents()[0]->get_value(), node->get_parents()[1]->get_value()));

            }
            else if (node->get_operation() == Operation::exponent)
            {
                node->set_value(std::exp(node->get_parents()[0]->get_value()));
            }
            else if (node->get_operation() == Operation::log)
            {
                node->set_value(std::log(node->get_parents()[0]->get_value()));
            }
            else if (node->get_operation() == Operation::tanh)
            {
                node->set_value(std::tanh(node->get_parents()[0]->get_value()));
            }
            else if (node->get_operation() == Operation::relu)
            {   
                if(node->get_parents()[0]->get_value() > 0)
                {
                    node->set_value(node->get_parents()[0]->get_value());
                }
                else
                {
                node ->set_value(0);
                }
            }
        

        return node->get_value();
        }


        double Gradient_checker(std::shared_ptr<Node>& input, double h, const std::shared_ptr<Node>& output)
        {
            double origin_in = input->get_value();


            input->set_value(input->get_value() + h);
            double L_p_h = replay_network(output);

            input->set_value(input->get_value() - 2*h);
            double L_m_h = replay_network(output);

            input->set_value(origin_in);
            replay_network(output);

            return (L_p_h - L_m_h)/(2*h);
        }





