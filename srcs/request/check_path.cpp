# include "check_path.hpp"
#include "../parsing/location.hpp"

Check_path::Check_path(std::list<Client *>::iterator iter, Server &serv): skip(0)
{
    check_transfer_encoding(iter, serv);
}

void   Check_path::check_transfer_encoding(std::list<Client *>::iterator iter, Server &serv)
{
    // (*iter)->path, (*iter)->request_pack, (*iter)->_content_type,
    std::map<std::string, std::vector<std::string> > map_req = (*iter)->request_pack;
    std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Transfer-Encoding");
    if (m_ap != map_req.end())
    {
        std::vector<std::string> vec = m_ap->second;
        std::vector<std::string>::iterator itt = vec.begin();
        if (itt == vec.end())
        {
            std::cout<< "Transfer-Encoding with empty value error /501 not implemented"<<std::endl;
            // drop_client(iter);
            this->skip = 1;
            return ;
        }
        else
        {
            for(; itt != vec.end(); itt++)
            {
                if ((*itt) != "chunked")
                {
                    std::cout<< "Transfer-Encoding not a match error /501 not implemented"<<std::endl;
                    // drop_client(iter);
                    std::cout<<"heeeer"<<std::endl;
                    this->skip = 1;
                    return ;
                }
                else
                {
                    if ((*iter)->_content_type == 1)
                    {
                        std::cout<<"Transfer-Encoding(chunked) and boundry error not implemented"<<std::endl;
                        //set_error page of not implemented
                        this->skip = 1;
                        return ;
                    }
                    else
                        (*iter)->_content_type = 2;
                }
            }
        }
    }
    if (((*iter)->_content_type == 0 || (*iter)->_content_type == 1) && ((*iter)->method == "POST"))
    {
        std::map<std::string, std::vector<std::string> > map_req = (*iter)->request_pack;
        std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Content-Length");
        if (m_ap != map_req.end())
        {
            std::vector<std::string> vec = m_ap->second;
            std::vector<std::string>::iterator itt = vec.begin();
            if (itt == vec.end())
            {
                std::cout<< "Content-Length exist with no value error /400 bad request"<<std::endl;
                // drop_client(iter);
                this->skip = 1;
                return ;
            }
            else
            {
                std::cout<<"con-len : "<<*itt<<std::endl;
            }
        }
        else
        {
            std::cout<< "Content-Length not exist error /400 bad request"<<std::endl;
            // drop_client(iter);
            this->skip = 1;
            return ;
        }
    }
    check_uri(iter, serv);
}

bool isCharAllowed(char c) {
    // List of allowed characters
    const std::string allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    
    // Check if the character is in the allowed list
    return (allowedChars.find(c) != std::string::npos);
}

bool isURIValid(const std::string& uri, int len) {
    // Check each character in the URI
    for (int i = 0; i < len; i++) {
        if (!isCharAllowed(uri[i])) {
            // Character not allowed, URI is invalid
            return false;
        }
    }
    
    // All characters are allowed, URI is valid
    return true;
}

void    Check_path::check_uri(std::list<Client *>::iterator iter, Server &serv)
{
    std::string uri = (*iter)->path;
    int len = uri.length();
    if (len > 2048)
    {
        std::cout<<"request-URI- too long error /414"<<std::endl;
        // drop-client;
        this->skip = 1;
        return ;
    }
    if (!isURIValid(uri, len)) {
        std::cout << "URI have invalid characters error /400 bad request" << std::endl;
        // drop-client;
        this->skip = 1;
        return ;
    }
    get_matched_location_for_request_uri(iter, serv);
}

void    Check_path::get_matched_location_for_request_uri(std::list<Client *>::iterator iter, Server &serv)
{
    std::list<location> loc = serv.get_locations();
    std::list<location>::iterator it;
    int signe = 0;
    for(it = loc.begin(); it != loc.end(); it++)
    {
        if ((*iter)->path.find((*it).get_locations()) != std::string::npos)
        {
            int i = (*it).get_locations().length();
            if ((*iter)->path[i] == '/' || (*iter)->path.length() == i)
            {
                signe  = 1;
                std::cout<<"location found"<<std::endl;
                if (this->loc_path.length() < (*it).get_locations().length())
                {
                    this->loc_path = (*it).get_locations();
                    this->location_match = (*it);
                }
            }
        }
    }
    if (signe == 0)
    {
        std::cout<<"location not found"<<std::endl;
        this->skip = 1;
        return ;
    }
    else
    {
        (*iter)->location_match = this->location_match;
        // std::string str = (*iter)->path.substr(loc_path.length(), (*iter)->path.length());
        this->loc_path = this->location_match.root + &(*iter)->path[loc_path.length()];
        (*iter)->loc_path = this->loc_path;
        is_location_has_redirection(iter, serv);
    }
}

void    Check_path::is_location_has_redirection(std::list<Client *>::iterator iter, Server &serv)
{
    // std::cout<<this->location_match.get_re
    is_method_allowed_in_location(iter);
}

void    Check_path::is_method_allowed_in_location(std::list<Client *>::iterator iter)
{
    std::list<std::string> methodes = this->location_match.get_allow_methods();
    std::list<std::string>::iterator it;
    for(it = methodes.begin(); it != methodes.end(); it++)
    {
        if ((*iter)->method == (*it))
            break ;
    }
    if ((*iter)->method != (*it))
    {
        std::cout<<"error / 405 method not allowed"<<std::endl;
        this->skip = 1;
        return ;
    }
}
