#include "post.hpp"
#include "../server/server.hpp"
#include "../parsing/location.hpp"
#include "../server/client.hpp"
#include "../parsing/webserv.hpp"


Post::Post(): body_or_head(0), _post_type(0), _chunk_len(0), _hex_len(0) 
{
    memset(this->_hex, 0, 20);
}
Post::~Post(){}

void    Post::call_post_func(Server &serv, Client *client)
{
    int x = Treat_Post(client, serv);
    if(!is_created)
        this->create_file(serv, client);
    if (x == 0)
    {
        switch(this->_post_type)
        {
            case 0:
                this->normal_post(serv, client);
            case 2:
                this->chunked_post(serv, client);
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
}