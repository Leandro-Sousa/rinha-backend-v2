#include <fstream>
#include <string>

class FileUtils
{
public:
	static std::string readAllText(const std::string &path)
	{
		std::ifstream ifs(path);
		char c;
		std::string content;

		while (ifs.get(c))
		{
			content += c;
		}

		return content;
	}
};
