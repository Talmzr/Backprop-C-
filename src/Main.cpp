#include "node.h"

int main(){
    std::shared_ptr<Node> a = Node_Input(2);
    std::shared_ptr<Node> b = Node_Input(-3);
    std::shared_ptr<Node> c = Node_Input(5);
    std::shared_ptr<Node> f = Node_Input(-2);

    std::shared_ptr<Node> d = a*b;
        


    std::shared_ptr<Node> g = power(a,3);

    std::shared_ptr<Node> l = ((a/b + (-d))*f)  + (a*c - f)* exp(g) +log(a) + tanh(f);
    

    backward(l);

    graph(l);

    std::cout << Gradient_checker(a, 0.00000001, l) << "\n";
    std::cout << Gradient_checker(b, 0.00000001, l) << "\n";
    std::cout << Gradient_checker(c, 0.00000001, l) << "\n";
    std::cout << Gradient_checker(f, 0.00000001, l) << "\n";
    std::cout << Gradient_checker(d, 0.00000001, l) << "\n";



}
