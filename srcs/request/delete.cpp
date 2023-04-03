#include "delete.hpp"
#include "../server/client.hpp"
#include "../server/server.hpp"
#include "../headers_cpp.hpp"


Delete::Delete()
{
    
}

int deleteFolder(const char* folderPath) {
    int status = 0;

    // Open the folder
    DIR* dir = opendir(folderPath);
    if (dir == NULL) {
        std::cerr << "Error opening folder: " << folderPath << std::endl;
        return -1;
    }

    // Iterate over the contents of the folder
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the path to the entry
        char entryPath[PATH_MAX];
        snprintf(entryPath, PATH_MAX, "%s/%s", folderPath, entry->d_name);

        // Recursively delete subdirectories
        if (entry->d_type == DT_DIR) {
            int subStatus = deleteFolder(entryPath);
            if (subStatus != 0) {
                status = subStatus;
            }
        }
        // Delete files and symbolic links
        else {
            int subStatus = unlink(entryPath);
            if (subStatus != 0) {
                status = subStatus;
            }
        }
    }

    // Close the folder
    closedir(dir);

    // Delete the empty folder
    if (status == 0) {
        int subStatus = rmdir(folderPath);
        if (subStatus != 0) {
            std::cerr << "Error deleting folder: " << folderPath << std::endl;
            return subStatus;
        }
    }

    return status;
}

char *read_from_file(std::string file)
{
	char *str;
	str = (char *)malloc(sizeof(char) * 1024);
    memset(str, 0, 1024);
	int fd_file = open(file.c_str(),O_RDONLY);
	int i = read(fd_file,str,1024);
	str[i] = '\0';
	return (str);
}

void Delete::delete_directory(Client *ctl, Server &serv)
{
    if (!deleteFolder(ctl->loc_path.c_str()))
    {
        std::cout << "SHOULD RETURN 204 NO CONTENT" << std::endl;
        std::string header = "HTTP/1.1 204 NO CONTENT\nContent-Type: text/html\nContent-Length: 228\r\nConnection: closed\r\n\r\n";
		// std::string body = "<!DOCTYPE html><html><head><title>404 Not Found</title></head></html>";
        header += read_from_file("404error.html");
        write(ctl->get_sockfd(),header.c_str(),header.length());
    }
    else
    {
        if (access(ctl->loc_path.c_str(), W_OK) == 0)
             std::cout << "SHOULD RETURN 500 INTERNAL SERVER ERROR" << std::endl;
        else
            std::cout << "SHOULD RETURN 403 FORBIDDEN" << std::endl; 
    }
}

void Delete::Treat_directory(Client *ctl, Server &serv)
{
        this->delete_directory(ctl, serv);
}

void Delete::Treat_File(Client *ctl, Server &serv)
{
    remove(ctl->loc_path.c_str());
}

void Delete::erase(Client *ctl, Server &serv)
{
    DIR* dir = opendir(ctl->loc_path.c_str());
    if (dir != NULL)
    {
        std::cout << "The client requested a directory" << std::endl;
        this->Treat_directory(ctl, serv);
    }
    else if (fopen(ctl->loc_path.c_str(), "r") != NULL)
    {
        std::cout << "The client requested a file" << std::endl;
        this->Treat_File(ctl, serv);
    } 
    else
        std::cout << "SHOULD RETURN 404 MOT FOUND" << std::endl;
}
Delete::~Delete()
{

}
