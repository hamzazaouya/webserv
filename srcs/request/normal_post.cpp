#include "post.hpp"
#include "../server/server.hpp"
#include "../parsing/location.hpp"
#include "../server/client.hpp"

void    Post::normal_post(Server &serv, Client *client)
{
        std::list<location> loc = serv.get_locations();
        for (std::list<location>::iterator it = loc.begin(); it != loc.end(); ++it)
        {
            if (it->get_locations() == client->path)
            {
                if(client->file_path.empty())
                {
                    std::map<std::string, std::vector<std::string> >::iterator iter;
                    iter = client->request_pack.find("Content-Type");
                    client->file_path = it->get_upload_pass();
                    client->generate_file_name(*((*iter).second.begin()), serv.file_extensions);
                }
                if (access(it->get_upload_pass().c_str(), F_OK))
                    mkdir(it->get_upload_pass().c_str(), 0777);
                if(access(const_cast<char *>(client->file_path.c_str()), F_OK))
                    client->file.open(client->file_path, std::ios::binary | std::ios::app);
                // i have to write byte by byte because i have to check for content_length.
                // if i change the content_length to some number not <calculated when request is sent> ?
                client->file.write(serv._request, serv._request_size);
            }
        }

}