
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

typedef int(*PFNFUNCTION)(std::string);

struct tagFunctions
{
	std::string szName;
	PFNFUNCTION pfnHandle;
};

static int funcPrint(std::string strParams)
{
	printf("%s\n", strParams.c_str());
	return 0;
}

tagFunctions g_tFunctions[] = 
{
	{"print", funcPrint}
};

void OutError(std::string strOutput)
{
	std::cout << "Dashie Error: " << strOutput.c_str() << "\n";
}

std::string ReadStringParam(std::string strParameters)
{
	if (strParameters.empty())
		return "";

	size_t iStart = strParameters.find_first_of('\"'),
		iEnd = strParameters.find_first_of('\"', iStart+1);

	std::string strString;
	if (iStart != std::string::npos && iEnd != std::string::npos)
		strString = strParameters.substr(iStart+1, iEnd-2);
	else
		OutError("Failed to parse string parameter");

	return strString;
}

bool ExecDashie(std::string strFileName)
{
	if (strFileName.empty())
	{
		OutError("No file specified.");
		return false;
	}

	std::ifstream file(strFileName.c_str());
	if (!file.is_open())
	{
		OutError("Failed to open the file.");
		return false;
	}

	std::string strLine;
	while (std::getline(file, strLine))
	{
		std::string strPrefix = strLine.substr(0, strLine.find_first_of(' '));

		std::vector<tagFunctions> vecFunctions(g_tFunctions,
			g_tFunctions + sizeof(g_tFunctions) / sizeof(g_tFunctions[0]));

		std::vector<tagFunctions>::iterator be = vecFunctions.begin(),
			en = vecFunctions.end();
		for (; be != en; ++be)
		{
			if (strPrefix.compare((const char*)be->szName.c_str()) != std::string::npos)
			{
				if (*be->pfnHandle != nullptr)
				{
					std::string strParameters = strLine.substr(strPrefix.length(), std::string::npos);
					be->pfnHandle(ReadStringParam(strParameters));
				}
			}
		}
	}

	file.close();
	return true;
}

bool CheckExt(std::string strFileName)
{
	if (strFileName.empty())
		return false;
	
	std::string strExt = strFileName.substr(strFileName.find_last_of('.'), std::string::npos);

	if (strExt.compare(".dashie") == 0)
		return true;

	return false;
}

int main(int argc, char *argv[])
{
	std::cout << "Dashie Language Project (Version: 0.1) by RD42\n";
	std::cout << "Github: https://github.com/dashr9230 \n\n";

	if (argc > 1)
	{
		if (CheckExt(argv[1])) // Do not open any file than *.dashie
			ExecDashie(argv[1]);
		else
			std::cout << "Invalid file! Only *.dashie files can be loaded!\n";
	}
	else
	{
		std::cout << "No arguments found.\n";
		std::cout << "Arguments: dashie.exe [filename]\n";
	}

	std::cout << "\nWe're done! Press enter to exit.\n";
	std::cin.get();
	return 0;
}
