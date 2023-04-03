#include "post.hpp"
#include "../server/server.hpp"
#include "../parsing/location.hpp"
#include "../server/client.hpp"
#include "../parsing/webserv.hpp"


Post::Post(): body_or_head(0), _post_type(0), _chunk_len(0), _hex_len(0), _is_matched(0) 
{
    memset(this->_hex, 0, 20);
}
Post::~Post(){}

void Post::check_post(Client *clt)
{
    if (!clt->location_match.get_upload_pass().empty())
        _is_matched = 1;
}
void    Post::call_post_func(Server &serv, Client *client)
{
    // int x = Treat_Post(client, serv);
    if (_is_matched == 1)
    {
        if(!is_created)
            this->create_file(serv, client);
        switch(this->_post_type)
        {
            case 0:
                this->normal_post(serv, client);
            case 2:
                this->chunked_post(serv, client);
        }
    }
    else
        Treat_Post(client, serv);
    // else if (x == 1)
    //     std::cout << "CGI" << std::endl;
    // else
    //     std::cout << "Error!" << std::endl;
}

void Post::Treat_Cgi(Client *ctl)
{
    if (ctl->location_match.get_cgi_pass().empty())
        std::cout << "SHOULD GET 403 FORBIDDEN" << std::endl;
    else
    {
        std::cout << "CGI HERE" << std::endl;
    }
}

void Post::Treat_directory(Client *ctl)
{
    if (ctl->location_match.get_index().empty())
        std::cout << "SHOULD GET 403 FORBIDDEN" << std::endl;
    else
        Treat_Cgi(ctl);
}

void Post::Treat_file(Client *ctl)
{
    Treat_Cgi(ctl);
}

void Post::Treat_Post(Client *ctl, Server &serv)
{
    DIR* dir = opendir(ctl->loc_path.c_str());
    if (dir != NULL)
    {
        std::cout << "The client requested a directory" << std::endl;
        if (ctl->loc_path[ctl->loc_path.size() - 1] == '/')
            Treat_directory(ctl);
        else
        {
            ctl->loc_path += "/"
            this->Treat_directory(ctl);
        }
    }
    else if (fopen(ctl->loc_path.c_str(), "r") != NULL)
    {
        std::cout << "The client requested a file" << std::endl;
        this->Treat_file(ctl);
    }
    else
        std::cout << "404 NOT FOUND" << std::endl;
}

void    Post::create_file(Server &serv, Client *client)
{
        std::string merge_path;
    if(client->file_path.empty())
    {
        std::map<std::string, std::vector<std::string> >::iterator iter;
        iter = client->request_pack.find("Content-Type");
        merge_path = client->loc_path + "/" + client->location_match.get_upload_pass();
        client->file_path = merge_path;
        client->generate_file_name(*((*iter).second.begin()), serv.file_extensions);
    }
    if (access(merge_path.c_str(), F_OK))
        mkdir(merge_path.c_str(), 0777);
    if(access(const_cast<char *>(client->file_path.c_str()), F_OK))
        client->file.open(client->file_path, std::ios::binary | std::ios::app);
    this->is_created = true;
    std::cout << "--->" << client->file_path.c_str() << std::endl;
}