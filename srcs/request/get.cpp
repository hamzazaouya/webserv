# include "get.hpp"
#include "../server/client.hpp"
#include "../parsing/location.hpp"
#include "../server/server.hpp"
#include <sys/stat.h>
#include <dirent.h>

Get::Get(): state(0)
{
}

Get::~Get()
{
}

void    Get::get_requested_resource(std::list<Client *>::iterator iter)
{
    this->state = 0;
    this->read = 0;
    std::string path = (*iter)->loc_path; 
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) == 0)
    {
        if (S_ISDIR(file_stat.st_mode))
        {
            check_the_end_of_uri(iter);
        }
        else if (S_ISREG(file_stat.st_mode))
            if_location_has_cgi(iter);
        else
        {
            this->state = 1;
            (*iter)->status_code = 404;
            (*iter)->status = "Not Found";
            std::vector<std::string> error = (*iter)->error_pages;
            std::vector<std::string>::iterator it = error.begin();
            if (it != error.end())
            {
                int num;
                std::stringstream ss(*it);
                ss >> num;
                std::cout<<"num : "<<num<<" (*it) : "<< (*it)<<std::endl;
                if (num == (*iter)->status_code)
                {
                    std::cout<<"forbiden"<<std::endl;
                    std::string path = "." + (*++it);
                    struct stat file_stat;
                    if (stat(path.c_str(), &file_stat) == 0)
                    {
                        if (S_ISREG(file_stat.st_mode))
                        {
                            (*iter)->loc_path = path;
                            this->state = 1;
                            return ;
                        }
                        else
                        {
                            (*iter)->loc_path = "./default_error_pages/404.html";
                            this->state = 1;
                            return ;
                        }
                    }
                    else
                    {
                        (*iter)->loc_path = "./default_error_pages/404.html";
                        this->state = 1;
                        return ;
                    }
                }
                else
                {
                    (*iter)->loc_path = "./default_error_pages/404.html";
                    this->state = 1;
                    return ;
                }
            }
            else
                (*iter)->loc_path = "./default_error_pages/404.html";
            return ;
        }
    }
    else
    {
        this->state = 1;
        (*iter)->status_code = 404;
        (*iter)->status = "Not Found";
        std::vector<std::string> error = (*iter)->error_pages;
        std::vector<std::string>::iterator it = error.begin();
        if (it != error.end())
        {
            int num;
            std::stringstream ss(*it);
            ss >> num;
            if (num == (*iter)->status_code)
            {
                std::string path = "." + (*++it);
                struct stat file_stat;
                if (stat(path.c_str(), &file_stat) == 0)
                {
                    if (S_ISREG(file_stat.st_mode))
                    {
                        (*iter)->loc_path = path;
                        this->state = 1;
                        return ;
                    }
                    else
                    {
                        (*iter)->loc_path = "./default_error_pages/404.html";
                        this->state = 1;
                        return ;
                    }
                }
                else
                {
                    (*iter)->loc_path = "./default_error_pages/404.html";
                    this->state = 1;
                    return ;
                }
            }
            else
            {
                (*iter)->loc_path = "./default_error_pages/404.html";
                this->state = 1;
                return ;
            }
        }
        else
            (*iter)->loc_path = "./default_error_pages/404.html";
        return ;
    }
}

void    Get::check_the_end_of_uri(std::list<Client *>::iterator iter)
{
    std::string path = (*iter)->loc_path; 
    if (path[path.length() - 1] != '/')
    {
        (*iter)->redirect_301.append("/");
        std::cout<<"error / 301 Moved Permanently"<<std::endl;
        (*iter)->status_code = 301;
        (*iter)->status = "Moved Permanently";
        this->state = 1;
        return ;
    }
    else
          is_dir_has_index_files(iter);
}

void    Get::is_dir_has_index_files(std::list<Client *>::iterator iter)
{
    location loc = (*iter)->location_match;
    std::list<std::string> index = loc.get_index();
    std::list<std::string>::iterator it;
    struct stat file_stat;
    for(it = index.begin(); it != index.end(); it++)
    {
        std::string path = (*iter)->loc_path + (*it);
        if (stat(path.c_str(), &file_stat) == 0)
        {
            if (S_ISREG(file_stat.st_mode))
            {
                this->index_exist = 1;
                // std::cout<<"is file "<<std::endl;
                break ;
            }
        }
    }
    if (this->index_exist == 1)
    {
        (*iter)->loc_path.append((*it));
        // std::cout<<"heer: "<<(*iter)->loc_path<<std::endl;
        if_location_has_cgi(iter);
    }
    else
        check_for_auto_index(iter);
}
#include <fcntl.h>
void    Get::if_location_has_cgi(std::list<Client *>::iterator iter)
{
    // if cgi exist
    //run it && return code depend on it
    //else
    std::map<std::string, std::string> cgi = (*iter)->location_match.get_cgi_pass();
    std::map<std::string, std::string>::iterator it = cgi.find("php");
    std::string str = it->second;
    std::cout<<str<<std::endl;
    // std::ofstream outfile("./cgi-bin/cgi-file");
    int fd = open("./cgi-bin/cgi-file", 1);
    if (fd < 0)
    {
        (*iter)->status_code = 403;
        (*iter)->status = "Forbidden";
        (*iter)->loc_path = "./default_error_pages/403.html";
        this->state = 1;
        return ;
    }
    std::cout << "uhuhuhu" << std::endl;
    if (fork() == 0)
    {
        dup2(fd, STDOUT_FILENO);
        char *arg[3];
        arg[0] = strdup(str.c_str());
        std::cerr << "dfdfd" << std::endl;
        arg[1] = strdup((*iter)->loc_path.c_str());
        arg[2] = NULL;
        std::cerr<<"hehhehejkehjk"<<std::endl;
        std::cerr<<arg[1]<<std::endl;
        execve(arg[0], arg, NULL);
        std::cout << "hihihih " << std::endl;
    }
    wait(NULL);
    close(fd);
    std::ifstream if_file("./cgi-bin/cgi-file", std::ios::in);
    std::string cgi_string;
    char buffer[1025];
    memset(buffer, 0 , 1025);
    if_file.read(buffer, 1024);
    int s_z = if_file.gcount();
    std::cout<<"size : "<<s_z<<std::endl;
    while(s_z)
    {
        cgi_string += buffer;
        memset(buffer, 0 , 1025);
        if_file.read(buffer, 1024);
        s_z = if_file.gcount();
    }
    int pos = cgi_string.find("\r\n\r\n");
    std::cout<<pos<<std::endl;
    std::string cgi_body = &cgi_string[pos + 4];
    std::cout<<"cgi_body ====> "<<cgi_body<<std::endl;
    if_file.close();
    std::ofstream of_file("./cgi-bin/cgi-file", std::ios::out);
    of_file<<cgi_body;
    if_file.close();
    std::cout<<"cgi_string ===> "<<cgi_string<<std::endl;
    (*iter)->loc_path = "./cgi-bin/cgi-file";
    (*iter)->status_code = 200;
    (*iter)->status = "OK";
    return ;
}

void    Get::check_for_auto_index(std::list<Client *>::iterator iter)
{
    location loc = (*iter)->location_match;
    if (loc.get_auto_index() == "on")
    {
        DIR *dir = opendir((*iter)->loc_path.c_str());
        if (dir == NULL)
        {
            (*iter)->status_code = 403;
            (*iter)->status = "Forbidden";
            (*iter)->loc_path = "./default_error_pages/403.html";
            this->state = 1;
            return ;
        }
        std::ofstream outfile("./default_error_pages/auto_index.html");
        if (!outfile.is_open())
        {
            (*iter)->status_code = 403;
            (*iter)->status = "Forbidden";
            (*iter)->loc_path = "./default_error_pages/403.html";
            this->state = 1;
            return ;
        }
        struct dirent *entity;
        entity = readdir(dir);
        outfile<<"<html><head><title>Listing directories</title></head><body><h1>listing files : </h1>";
        while (entity != NULL)
        {
            std::string uri = (*iter)->redirect_301;
            if (uri[uri.length() - 1] != '/')
                uri += "/";
            uri += entity->d_name;
            outfile<< "<li><a href='"+ uri + "'>"+ entity->d_name +"</a></li></br>";
            entity = readdir(dir);
        }
        outfile<<"</body></html>"<<std::endl;
        closedir(dir);
        this->state = 1;
        (*iter)->status_code = 200;
        (*iter)->status = "OK";
        (*iter)->loc_path = "./default_error_pages/auto_index.html";
        this->state = 1;
    }
    else
    {
        (*iter)->status_code = 403;
        (*iter)->status = "Forbidden";
        std::vector<std::string> error = (*iter)->error_pages;
        std::vector<std::string>::iterator it = error.begin();
        if (it != error.end())
        {
            int num;
            std::stringstream ss(*it);
            ss >> num;
            if (num == (*iter)->status_code)
            {
                std::string path = "." + (*++it);
                struct stat file_stat;
                if (stat(path.c_str(), &file_stat) == 0)
                {
                    if (S_ISREG(file_stat.st_mode))
                    {
                        (*iter)->loc_path = path;
                        this->state = 1;
                        return ;
                    }
                    else
                    {
                        (*iter)->loc_path = "./default_error_pages/403.html";
                        this->state = 1;
                        return ;
                    }
                }
                else
                {
                    (*iter)->loc_path = "./default_error_pages/403.html";
                    this->state = 1;
                    return ;
                }
            }
            else
            {
                (*iter)->loc_path = "./default_error_pages/403.html";
                this->state = 1;
                return ;
            }
        }
        else
            (*iter)->loc_path = "./default_error_pages/403.html";
        this->state = 1;
        return ;
    }
}
