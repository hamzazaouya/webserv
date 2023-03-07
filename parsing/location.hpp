# ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>

class location
{
    private:
    std::string locations;
    std::list<std::string> allow_methods;
    std::string redirect;
    std::string root;
    std::string index;
    std::string cgi_pass;
    std::string upload_pass;
    std::string auto_index;
    public:
    location(){}
    ~location(){}
    location(const std::list<std::string> &config);
    void FillLocation(std::string prompt);
    void FillAllow_methods(std::string prompt);
    void FillIndex(std::string prompt);
    void FillRedirect(std::string prompt);
    void FillRoot(std::string prompt);
    void FillCgi_pass(std::string prompt);
    void FillUpload_pass(std::string prompt);
    void FillAuto_index(std::string prompt);
};
# endif