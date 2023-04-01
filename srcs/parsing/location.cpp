#include "location.hpp"
//DON'T FORGET TO SET THE DEFAULT VALUES OF SOME PARAMETERS
std::vector<std::string> splitString(const std::string& str,const char splet)
{
    std::vector<std::string> tokens;
    std::istringstream tokenStream(str);
    std::string token;
    while (std::getline(tokenStream, token, splet))
    {
        if (!token.empty())
            tokens.push_back(token);
    }
     return (tokens);
}

int count_slash(std::string location)
{
    int i = 0;
    int x = 0;
    for (; i != location.size() ; i++)
    {
        if (location[i] == '/')
            x++;
    }
    return (x);
}
std::string location::trim_directory(int slash)
{
    int x = this->locations.rfind("/");
    std::string sub = locations.substr(0, x);
    return (sub);
}

void location::FillLocation(std::string prompt)
{

    std::vector<std::string> substring = splitString(prompt, ' ');

    if (substring.size() != 3)
    {
        for (std::vector<std::string>::iterator it = substring.begin(); it != substring.end(); ++it)
            std::cout << "********" << *it << std::endl;
        std::cout << "Error! the location block should start like this: Location 'your desired location' {" << std::endl;
        exit (1);
    }
    std::vector<std::string>::iterator it = substring.begin() + 1;
    this->locations = *it;
    // LET YOUSSEF USE IT FOR GET
    //int slash = count_slash(this->locations);
    //std::string tmp;
    /*while (slash)
    {
        if (access(tmp.c_str(), F_OK) == 0)
            break;
        else
        {
            this->locations = trim_directory(slash);
            tmp = this->locations + "/";
        }
        slash--;
    }
    this->locations = tmp;*/
}

void location::FillAllow_methods(std::string prompt)
{
    std::vector<std::string> substring = splitString(prompt, ' ');
    if (substring.size() < 2 || substring.size() > 4)
    {
        std::cout << "There's something wrong with the allow_methods parameters" << std::endl;
        exit (1);
    }
    allow_methods.clear();
    for (std::vector<std::string>::iterator it = substring.begin() + 1 ; it != substring.end(); ++it)
            this->allow_methods.push_back(*it);
}

void location::FillIndex(std::string prompt)
{
    std::vector<std::string> substring = splitString(prompt, ' ');
    if (substring.size() < 2)
    {
        std::cout << "Error! there's something wrong with the index parameter" << std::endl;
        exit (1);
    }
    if (substring.size() > 1)
    {
        for(std::vector<std::string>::iterator it = substring.begin() + 1; it != substring.end(); ++it)
            index.push_back(*it);
    }
}

void location::FillRedirect(std::string prompt)
{
    std::vector<std::string> substring = splitString(prompt, ' ');
    if (substring.size() != 2)
    {
        std::cout << "Error! there's something wrong with the redirect parameter" << std::endl;
        exit (1);
    }
    std::vector<std::string>::iterator it = substring.begin() + 1;
    this->redirect = *it;
}

void location::FillRoot(std::string prompt)
{
    std::vector<std::string> substring = splitString(prompt, ' ');
    if (substring.size() != 2)
    {
        std::cout << "Error! there's something wrong with the root parameter" << std::endl;
        exit (1);
        return;
    }
    std::vector<std::string>::iterator it = substring.begin() + 1;
    this->root = *it;
}

void location::FillCgi_pass(std::string prompt)
{
    std::vector<std::string> substring = splitString(prompt, ' ');
    if (substring.size() != 2)
    {
         std::cout << "Error! there's something wrong with the cgi_pass parameter" << std::endl;
         exit (1);
        //return ;
    }
    std::vector<std::string>::iterator it = substring.begin() + 1;
    std::vector<std::string> substring2 = splitString(*it, ':');
    if (substring2.size() != 2)
    {
         std::cout << "Error! there's something wrong with the cgi_pass parameter" << std::endl;
         exit (1);
        //return ;
    }
    this->cgi_pass.insert(std::make_pair(*(substring2.begin()), *(substring2.end() - 1)));
}

void location::FillUpload_pass(std::string prompt)
{
    std::vector<std::string> substring = splitString(prompt, ' ');
    if (substring.size() != 2)
    {
        std::cout << "Error! there's something wrong with the upload_pass parameter" << std::endl;
        exit (1);
    }
    std::vector<std::string>::iterator it = substring.begin() + 1;
    this->upload_pass = *it;
}

void location::FillAuto_index(std::string prompt)
{
    std::vector<std::string> substring = splitString(prompt, ' ');
    if (substring.size() != 2)
        return;
    std::vector<std::string>::iterator it = substring.begin() + 1;
    this->auto_index = *it;
}

void location::initialize()
{
    auto_index = "off";
    root = "/var/www/html/";
}

location::location(const std::list<std::string> &config, int j)
{
    std::string nextvalue;
    std::list<std::string>::const_iterator it = config.begin();
    while (it != config.end() && j != 0)
    {
        if (it->find("}") != -1)
            j--;
        it++;
    }
    this->initialize();
    //DON'T REDIRECT WHEN IT'S EMPTY
    for (; it != config.end() && std::next(it) != config.end(); ++it)
    {
        std::string input = *it;
        //std::vector<std::string> prompt = splitString(input);
        //std::vector<std::string>::iterator tt = prompt.begin();
        nextvalue = *std::next(it);
        if ((*it).find("}") != -1)
            break;
        if ((*it).find("location") != -1)
            //TRY TO HANDLE IF THE LOCATION ISN'T SPECIFIED
            this->FillLocation(*it);
        else if ((*it).find("allow_methods") != -1)
            this->FillAllow_methods(*it);
        else if ((*it).find("redirect") != -1)
            this->FillRedirect(*it);
        else if ((*it).find("auto_index") != -1)
            this->FillAuto_index(*it);
        else if ((*it).find("root") != -1)
            this->FillRoot(*it);
        else if ((*it).find("index") != -1)
            this->FillIndex(*it);
        else if ((*it).find("upload_pass") != -1)
            this->FillUpload_pass(*it);
        else if ((*it).find("cgi_pass") != -1)
            this->FillCgi_pass(*it);
        else if (*it == "}" && (nextvalue != "};" || (nextvalue.find("location") == -1 && nextvalue.find("{") == -1)))
        {
            std::cout << "Please only have another location block after starting with one" << std::endl;
            exit (1);
        }
    }
}
