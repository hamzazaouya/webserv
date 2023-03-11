
# include "parce_server.hpp"

std::vector<std::string> split(const std::string &str)
{
        std::vector<std::string> tokens;
        std::istringstream tokenStream(str);
        std::string token;
        while (std::getline(tokenStream, token, ' '))
        {
            if (!token.empty())
                tokens.push_back(token);
        }
        return (tokens);
}

int set_port(std::vector<std::string> &tokens)
{
    std::vector<std::string>::iterator it = tokens.begin();
    int num;
    if (++it == tokens.end())
        return (8080); // the end of 
    else
    {
        std::string str = *it;
        std::string::const_iterator it = str.begin();
        if (it != str.end())
        {
            for(; it != str.end(); it++)
            {
                if (!isdigit(*it))
                    return (8080);
            }
            std::stringstream ss(str);
            ss >> num;
            return (num);
        }
    }
    return (8080);
}

std::string set_host_name(std::vector<std::string> &tokens)
{
    std::vector<std::string>::iterator it = tokens.begin();
    if (++it == tokens.end())
        return ("127.0.0.1");
    else
    {
        return (*it);
    }
}

int set_max_client_body_size(std::vector<std::string> &tokens)
{
    std::vector<std::string>::iterator it = tokens.begin();
    int num;
    if (++it == tokens.end())
        return (1);
    else
    {
        std::string str = *it;
        for(std::string::const_iterator it = str.begin(); it != str.end(); it++)
        {
            if (!isdigit(*it))
                return (8080);
        }
        std::stringstream ss(str);
        ss >> num;
    }
    return (num);
}

std::vector<std::string> set_error_page(std::vector<std::string> &tokens)
{
    std::vector<std::string>::iterator it = tokens.begin();
    std::vector<std::string> tmp;
    if (++it == tokens.end())
    {
        tmp.push_back("defualt");
        return (tmp);
    }
    else
    {
        tmp.push_back(*it);
        if (++it == tokens.end())
            return (tmp);
        else
            tmp.push_back(*it);
    }
    return (tmp);
}

parce_server::parce_server(const std::list<std::string> &conf, int n_serv)
{
    int ind = 0;
    int count_loc = 0;
    int j = 0;
    std::list<std::string>::const_iterator it = conf.begin();
    while (it != conf.end() && n_serv != 0)
    {
        if (it->find("};") != std::string::npos)
            n_serv--;
        it++;
    }
    this->port = 8080;
    this->host_name = "172.0.0.1";
    this->max_client_body_size = 1;
    // this->error_page;
    for(; it != conf.end(); it++)
    {
        if ((*it).empty())
            for (; (*it).empty() != 0; ++it);
        if ((it->find("location") != -1 && it->rfind("{") != -1) && ind)
            break;
        if (it->find("location") != -1 && it->rfind("{") != -1)
        {
            count_loc++;
            ind++;
        }
        if (it->rfind("}") != -1 && it->rfind("};") == -1)
            ind--;
        if (it->rfind("};") != std::string::npos)
            break;
        std::string input = *it;
        // WE SHOULD REPLACE THE WHITESPACES WITH NORMAL SPACES FIRST THEN SPLIT BY SPACE WHICH MEANS THAT WE DON'T NEED TO TRIM THE TAB ANYMORE
        std::vector<std::string> tokens = split(input);
        std::vector<std::string>::iterator tt = tokens.begin();
        if (*tt == "port")
            this->port = set_port(tokens);
        else if (*tt == "host_name")
            // std::cout<<*tt<<std::endl;
            this->host_name = set_host_name(tokens);
        else if (*tt == "max_client_body_size")
            this->max_client_body_size = set_max_client_body_size(tokens);
        else if (*tt == "error_page")
            this->error_page = set_error_page(tokens);
    }
    while (j < count_loc)
	{
        if (ind)
        {
            std::cout << "You fucked up" << std::endl;
            exit (1);
        }
		location loc(conf, j);
		this->locations.push_back(loc);
		j++;
	}

}