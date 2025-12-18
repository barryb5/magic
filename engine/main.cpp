#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv)
{
    std::cout << "Hello World Test" << std::endl;

    std::ifstream library("card_data/cards_raw/card_library.txt");
    std::string line;

    if (library.is_open())
    {
        // Read data line by line from the file
        while (std::getline(library, line))
        {
            std::cout << line << std::endl;
        }
        // Close the file when done (good practice, though automatic on destruction)
        library.close();
    }
    else
    {
        std::cerr << "Error opening file" << std::endl;
    }

    return 0;
}