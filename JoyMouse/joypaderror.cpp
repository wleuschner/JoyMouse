#include "joypaderror.h"

JoypadError::JoypadError(std::string file,std::string error)
{
    joypadFile = file;
    this->errorMessage = error;
}

std::string JoypadError::getJoypadFile()
{
    return joypadFile;
}

std::string JoypadError::getErrorMessage()
{
    return errorMessage;
}
