
#include "post.hpp"
#include "../server/server.hpp"
#include "../parsing/location.hpp"
#include "../server/client.hpp"
#include "../parsing/webserv.hpp"


Post::Post(): body_or_head(0), _post_type(0) {}
Post::~Post(){}

void    Post::call_post_func(Server &serv, Client *client)
{
    std::cout << "Hi" << std::endl;
    int x = Treat_Post(client, serv);
    if (x == 0)
    {
        std::cout << "No CGI" << std::endl;
        switch(this->_post_type)
        {
            case 0:
                this->normal_post(serv, client);
        }
    }
    else if (x == 1)
        std::cout << "CGI" << std::endl;
    else
        std::cout << "Error!" << std::endl;
}

int Post::Treat_Post(Client *ctl, Server &serv)
{
    if (ctl->location_match.get_upload_pass().empty() && !ctl->location_match.get_cgi_pass().empty())
        return (1);
    else if (!ctl->location_match.get_upload_pass().empty())
        return (0);
    else
        return(-1);
}

std::string Post::check_hexa(std::string buff)
{
    std::string hex;
    int i = 0;
    int ind = buff.find("\r\n");
    
    std::cout << "INDEX ======== " << ind << std::endl;
    while (buff[ind]!= '\r' && buff[ind]!= '\n')
        hex += buff[ind++];
    
    return (hex);
}


int Post::hexToDec(const std::string& hexStr) {
    std::stringstream ss;
    ss << std::hex << hexStr;
    int decNum;
    ss >> decNum;
    return decNum;
}

// void    Post::normal_post(Server &serv, std::string &path)
// {
//     // std::list<location> loc = serv.get_locations();
//     // std::ofstream file;
//     //    for (std::list<location>::iterator it = loc.begin(); it != loc.end(); ++it)
//     //    {
//     //        if (it->get_locations() == path)
//     //        {
//     //            if (access(it->get_upload_pass().c_str(), F_OK))
//     //                int status = mkdir(it->get_upload_pass().c_str(), 0777);
//     //             if(access(std::strcat(const_cast<char *>(it->get_upload_pass().c_str()), "/testing"), F_OK))
//     //                 file(it->get_upload_pass() + "/testing");
//     //             else
//     //                 file.open(it->get_upload_pass() + "/testing");
//     //             if (file.is_open())
//     //             {
//     //                 if(!file.write(serv._request_buff, std::strlen(serv._request_buff)))
//     //                     file.close();
//     //             }
//     //        }
//     //    }
// }

int Post::skip_hex(std::string body)
{
     int i = body.find("\r\n") + 1;
    while (body[++i])
        if (body[i] == '\n')
            break;
    return (i + 1);
}

void Post::exec_head(std::string buff, Server &serv, std::string &path)
{
    // std::cout << "Hello" << std::endl;
    // if (!path.empty())
    // {
    //     std::list<location> loc = serv.get_locations();
    //     for (std::list<location>::iterator it = loc.begin(); it != loc.end(); ++it)
    //     {
    //         if (it->get_locations() == path)
    //         {
    //             if (access(it->get_upload_pass().c_str(), F_OK))
    //                 int status = mkdir(it->get_upload_pass().c_str(), 0777);
    //             std::ofstream file(it->get_upload_pass() + "/testing");
    //             if (file.is_open())
    //             {
    //                 std::string body = seperate_header(buff);
    //                 std::string hex = check_hexa(body);
    //                 int num_to_read = hexToDec(hex);
    //                  int ind = skip_hex(body);
    //                 file.write(&body[ind], num_to_read);
    //                 //if (i == body.find("\r\n0\r\n"))
    //                 //{
    //                     file.close();
    //                   //  break;
    //                 //}
    //             }
    //         }
    //     }
    // }
}


void Post::exec_body(std::string buff, Server &serv, std::string &path)
{
    if (!path.empty())
    {
        std::list<location> loc = serv.get_locations();
        for (std::list<location>::iterator it = loc.begin(); it != loc.end(); ++it)
        {
            if (it->get_locations() == path)
            {
                if (access(it->get_upload_pass().c_str(), F_OK))
                    int status = mkdir(it->get_upload_pass().c_str(), 0777);
                std::ofstream file(it->get_upload_pass() + "/testing");
                if (file.is_open())
                {
                    std::string hex = check_hexa(buff);
                    std::cout << "HEX ===== " << hex << std::endl;
                    int num_to_read = hexToDec(hex);
                     int ind = skip_hex(buff);
                    file.write(&buff[ind], num_to_read);
                    //if (i == body.find("\r\n0\r\n"))
                    //{
                        file.close();
                      //  break;
                    //}
                }
            }
        }
    }
}