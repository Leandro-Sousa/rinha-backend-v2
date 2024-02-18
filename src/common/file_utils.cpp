#pragma once

#include <fstream>
#include <string>

class FileUtils
{
public:
   
    static std::string readAllAsText(const std::string& path)
    {
        std::ifstream ifs(path);
        char c;
        std::string content;

        while(ifs.get(c))
        {
            content += c;
        }
        ifs.close();

        return content;
    }
};

