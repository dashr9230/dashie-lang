
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <Windows.h>

class Parameters;
typedef int(*PFNFUNCTION)(Parameters);

struct tagFunctions
{
	std::string szName;
	PFNFUNCTION pfnHandle;
};

void OutError(std::string strOutput)
{
	std::cout << "Dashie Error: " << strOutput.c_str() << "\n";
}

void OutWarning(std::string strOutput)
{
	std::cout << "Dashie Warning: " << strOutput.c_str() << "\n";
}

class Parameters
{
private:
	std::vector<std::string> m_Parameters;
	std::string m_strFullParameters;

	void SplitIt()
	{
		// Add the full parameters to 0th index.
		m_Parameters.push_back(m_strFullParameters);

		size_t iPos = 0;
		std::string strParametersCopy = m_strFullParameters;
		while ((iPos = strParametersCopy.find(',')) != std::string::npos)
		{
			m_Parameters.push_back(strParametersCopy.substr(0, iPos));
			strParametersCopy.erase(0, iPos + 1);
		}

		m_Parameters.push_back(strParametersCopy);
	}
public:
	Parameters(std::string strParameters)
	{
		m_strFullParameters = strParameters;
		SplitIt();
	}

	template<typename T>
	T GetNumber(size_t iParameter)
	{
		try {
			return (T)std::atoi(m_Parameters.at(iParameter).c_str());
		}
		catch (std::out_of_range& oor)
		{
			OutWarning("Parameter is out of range.");
			return (T)0;
		}
	}

	const char* GetString(size_t iParameter)
	{
		std::string str = "";
		try {
			str = m_Parameters.at(iParameter);
		}
		catch (std::out_of_range& oor)
		{
			OutWarning("Parameter is out of range.");
			return str.c_str();
		}

		size_t iStart = str.find_first_of('\"'),
			iEnd = str.find_first_of('\"', iStart + 1);

		std::string strResult;
		if (iStart != std::string::npos && iEnd != std::string::npos)
			strResult = str.substr(iStart + 1, iEnd - 2);
		else
			OutError("Failed to parse string parameter");

		return strResult.c_str();
	}
};

static int funcPrint(Parameters params)
{
	printf("%s\n", params.GetString(1));
	return 0;
}

static int funcMessageBox(Parameters params)
{
	MessageBoxA(0, params.GetString(1), params.GetString(2), params.GetNumber<UINT>(3));
	return 0;
}

tagFunctions g_tFunctions[] = 
{
	{"print", funcPrint},
	{"MessageBox", funcMessageBox}
};

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
			if(strPrefix.compare(be->szName.c_str()) == 0 && be->pfnHandle != nullptr)
			{
				Parameters params(strLine.substr(strPrefix.length(), std::string::npos));
				be->pfnHandle(params);
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
	std::cout << "Dashie Project (Version: 0.2) by RD42\n";
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
