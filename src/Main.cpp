#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>




enum class Operation{add, multiply, None};



class Node{
    public:
        Node(Operation op, double val,const std::vector <std::shared_ptr <Node>>& Parents) : PrevLayer(Parents), value(val),Gradient(0),nodeop(op){
        }

        double get_value() const {return value;}
        double get_gradient() const {return Gradient;}
        Operation get_operation() const {return nodeop;}
        const std::vector <std::shared_ptr <Node>>& get_parents() const {return PrevLayer;}




    private:
        const std::vector <std::shared_ptr <Node>> PrevLayer;
        double value;
        double Gradient; 
        const Operation nodeop;
};



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

    void Graph(const std::shared_ptr<Node>& node)
        {
            if ((node->get_parents()).size() > 0)
            {   
                for (const std::shared_ptr<Node>& n : node->get_parents())
                {
                    Graph(n);
                }
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
            std::cout << "\n";

        }








int main(){

    std::shared_ptr<Node> a = Node_Input(2); 
    std::shared_ptr<Node> b = Node_Input(-3);
    std::shared_ptr<Node> c = Node_Input(10);
    std::shared_ptr<Node> f = Node_Input(-2);

    std::shared_ptr<Node> l = (a*b + c)*f;




    Graph(l);



}