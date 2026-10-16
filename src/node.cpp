#include "node.h"







    Node::Node(Operation op, double val,const std::vector <std::shared_ptr <Node>>& Parents) : PrevLayer(Parents), value(val),Gradient(0),nodeop(op){
        }

    double Node::get_value() const {return value;}
    double Node::get_gradient() const {return Gradient;}
    Operation Node::get_operation() const {return nodeop;}
    const std::vector <std::shared_ptr <Node>>& Node::get_parents() const {return PrevLayer;}

    void Node::set_gradient(double newGradient) {Gradient += newGradient;}
    void Node::set_value(double newvalue) {value = newvalue;}


double ValArithm (Operation oparith,const std::shared_ptr <Node>& parent1,const std::shared_ptr <Node>& parent2){

    switch(oparith)
    {
        case Operation::add:    return (parent1->get_value() + parent2->get_value());
        case Operation::multiply:    return (parent1->get_value() * parent2->get_value());
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



        void gradient_p_addition(const std::shared_ptr<Node>& node)
        {
           for(std::size_t n=0; n < (node->get_parents()).size(); n++)
            {
                (node->get_parents()[n])->set_gradient(node->get_gradient());
            } 
        }

        void gradient_p_multiply(const std::shared_ptr<Node>& node)
        {
            for(std::size_t n=0; n < (node->get_parents()).size(); n++)
            {
                for (std::size_t j=0; j < (node->get_parents()).size(); j++)
                {
                (node->get_parents()[n])->set_gradient(node->get_gradient()*node->get_parents()[j]->get_value());
                }
            (node->get_parents()[n])->set_gradient(-(node->get_gradient()*node->get_parents()[n]->get_value()));
            }
        }



        void backward(const std::shared_ptr<Node>& node)
        {   
        
            std::unordered_set<std::shared_ptr<Node>> set;
            std::vector<std::shared_ptr<Node>> topo;

            topological(node, set, topo);

            topo[topo.size() - 1]->set_gradient(1);

            for (std::size_t i = topo.size(); i>0; i-- )
            {

                if (topo[i-1]->get_operation() == Operation::add)
                {
                    gradient_p_addition(topo[i-1]);
                }
                if (topo[i-1]->get_operation() == Operation::multiply)
                {
                    gradient_p_multiply(topo[i-1]);
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


        bool first_zero = true;
        
        for (const std::shared_ptr<Node>& i : node->get_parents())
        {
        
            if(node->get_operation() == Operation::add)
            {
                node->set_value(node->get_value() + i->get_value());
            }
            else if(node->get_operation() == Operation::multiply)
            {
                
                if(node->get_value() == 0  && first_zero == true)
                {
                    first_zero = false;
                    node->set_value(1);
                }
                node->set_value(node->get_value()* i->get_value());
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





