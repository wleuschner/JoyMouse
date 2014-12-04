#ifndef JOYPADERROR_H
#define JOYPADERROR_H
#include <string>

class JoypadError
{
public:
    JoypadError(std::string file,std::string error);
    std::string getJoypadFile();
    std::string getErrorMessage();
private:
    std::string joypadFile;
    std::string errorMessage;
};

#endif // JOYPADERROR_H
