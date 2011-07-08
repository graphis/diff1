
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char *argv[])
{
	std::ifstream in;
	std::string line;
	std::vector<std::string> args;
	std::vector<std::string> files;
	std::string first_path = ".";
	std::string second_path = ".";
	bool configuration_changed = false;
	struct passwd *pw = getpwuid(getuid());
	std::string homedir = pw->pw_dir;
	char current_working_dir[FILENAME_MAX];

	getcwd(current_working_dir,sizeof(current_working_dir));

	in.open((homedir + "/.diff1").c_str(),std::ios::in);
	if(in.is_open())
	{
		std::string token;
		while(getline(in,line))
		{
			std::stringstream line_stream(line);
			line_stream >> token;
			if("firstpath" == token)
			{
				getline(line_stream,token);
				first_path = token;
			}
			else if("secondpath" == token)
			{
				getline(line_stream,token);
				second_path = token;
			}
		}
		
	}
	in.close();
	for(int i=1;i<argc;i++)
	{
		if('-' != ((std::string)argv[i])[0])
		{
			files.push_back(argv[i]);
		}
		else if(("--first-path" == ((std::string)argv[i]))||("-1" == ((std::string)argv[i])))
		{
			i++;
			if(i < argc)
			{
				first_path = argv[i];
				if("." == first_path)
				{
					first_path = current_working_dir;
				}
				configuration_changed = true;
			}
		}
		else if(("--second-path" == ((std::string)argv[i]))||("-2" == ((std::string)argv[i])))
		{
			i++;
			if(i < argc)
			{
				second_path = argv[i];
				if("." == second_path)
				{
					second_path = current_working_dir;
				}
				configuration_changed = true;
			}
		}
		else
		{
			args.push_back(argv[i]);
		}
	}
	if(configuration_changed)
	{
		std::ofstream out;
		out.open((homedir + "/.diff1").c_str(),std::ios::out);
		if(out.is_open())
		{
			if("" != first_path)
			{
				out << "firstpath " << first_path << std::endl;
			}
			if("" != second_path)
			{
				out << "secondpath " << second_path << std::endl;
			}
		}
		out.close();
	}
	if(1 == files.size())
	{
		std::ostringstream command_stream;
		command_stream << "diff ";
		std::copy(args.begin(),args.end(),std::ostream_iterator<std::string>(command_stream," "));
		command_stream << ' ' << first_path << '/' << files[0] << ' ' << second_path << '/' << files[0];
		system(command_stream.str().c_str());
	}
	else if(0 == files.size())
	{
		if(!configuration_changed)
		{
			std::cerr << "nothing to be done." << std::endl;
			return -2;
		}
		return 0;
	}
	else
	{
		std::cerr << "error: invalid number of file parameters, use diff or diff3." << std::endl;
		return -1;
	}
	return 0;
}

