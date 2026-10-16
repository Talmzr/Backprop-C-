#include "node.h"

int main(){
    std::shared_ptr<Node> a = Node_Input(2);
    std::shared_ptr<Node> b = Node_Input(-3);
    std::shared_ptr<Node> c = Node_Input(5);
    std::shared_ptr<Node> f = Node_Input(-2);

    std::shared_ptr<Node> d = a*b;
    std::shared_ptr<Node> l = (a*b + d)*f  + (a*c + f)*d;    


    backward(l);

    graph(l);

    std::cout << Gradient_checker(b, 0.0001, l);




}
