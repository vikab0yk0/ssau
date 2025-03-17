// Variant 3
#include <iostream>
#include <iomanip>


bool continueProgram() {
    std::cout << "Enter 'y' to continue: ";
    char choice;
    std::cin >> choice;
    if( (choice == 'y' || choice == 'Y') && std::cin.peek() == '\n')
        return true;
    else return false;
}


int main() {
    int input;

    do {
        std::cout << "\nHexadecimal value: ";
        std::cin >> std::setbase(16);
        
        while (!(std::cin >> input) || std::cin.peek() != '\n') {
            std::cin.clear();
            while (std::cin.get() != '\n');
            std::cout << "Input error. Retry input\n";    
        }
        std::cout << "Decimal value: " << std::setbase(10) << input << std::endl;

    } while (continueProgram());

    return 0;
}
