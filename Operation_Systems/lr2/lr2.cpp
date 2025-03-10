#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <ctime>


const int BUFFER_SIZE = 4096;
const int PROGRESS_BAR_SIZE = 30;
const char PROGRESS_BAR = '=';
const char *MODES[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};


void handle_command(int argc, char *argv[]);
void print_help();
void copy_file(const char *src, const char *dest);
void move_file(const char *src, const char *dest);
void print_file_info(const char *file);
void change_file_mode(const char *file, const char *mode_str);


int main(int argc, char *argv[]) {
    if (argc > 1) {
        handle_command(argc, argv);
        return 0;
    }

    while (true) {
        std::cout << "Enter command (or --help for options, --quit to exit): ";

        char input[256];
        std::cin.getline(input, sizeof(input));

        int cmd_argc = 1;
        char *cmd_argv[10];
        char *token = strtok(input, " ");
        while (token != nullptr && cmd_argc < 10) {
            cmd_argv[cmd_argc++] = token;
            token = strtok(nullptr, " ");
        }

        if (cmd_argc == 0) continue;

        handle_command(cmd_argc, cmd_argv);
    }

    return 0;

}

void handle_command(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No command provided. Use --help for options." << std::endl;
        return;
    }

    if (!strcmp(argv[1], "--help")) print_help();

    else if (!strcmp(argv[1], "--copy")) {
        if (argc < 4)
            std::cerr << "Error: Missing source or destination file." << std::endl;
        else 
            copy_file(argv[2], argv[3]);
    }

    else if (!strcmp(argv[1], "--move")) {
        if (argc < 4)
            std::cerr << "Error: Missing source or destination file." << std::endl;
        else
            move_file(argv[2], argv[3]);
    }

    else if (!strcmp(argv[1], "--info")) {
        if (argc < 3)
            std::cerr << "Error: Missing file." << std::endl;
        else
            print_file_info(argv[2]);
    }

    else if (!strcmp(argv[1], "--chmod")) {
        if (argc < 4)
            std::cerr << "Error: Missing mode or file." << std::endl;
        else
            change_file_mode(argv[3], argv[2]);

    }

    else if (!strcmp(argv[1], "--quit")) {
        std::cout << "Exiting the program. Goodbye!" << std::endl;
        exit(0);
    }

    else {
        std::cerr << "Error: Unknown command." << std::endl;
        print_help();
    }
}


/*       HELP (ME)       */

void print_help() {
    std::cout<< "Options:\n"
    << "  --help                    Show this help message\n" 
    << "  --copy <srs> <dest>       Copy file from <src> to <dest>\n"
    << "  --move <srs> <dest>       Move or rename file\n"
    << "  --info <file>             Show <file> info\n"
    << "  --chmod <mode> <file>     Change permissions of <file> to <mode>\n"
    << "  --quit                    Exit the program\n\n";
}


/*       COPY FILE       */

bool file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void print_progress(size_t total_copied, size_t total_size) {
    size_t progress = (total_copied * PROGRESS_BAR_SIZE) / total_size;
    std::cout << "\rProgress: [";
    for (size_t i = 0; i < PROGRESS_BAR_SIZE; ++i) {
        if (i < progress) std::cout << PROGRESS_BAR;
        else if (i == progress) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int((double(total_copied)/total_size)*100)%100 << '%' << std::flush;
    usleep(3000);
}

void copy_file(const char *src, const char *dest) {
    if (strcmp(src, dest) == 0) {
        std::cerr << "Error: Source and destination files are the same." << std::endl;
        return;
    }

    if (file_exists(dest)) {
        std::cerr << "Error: Destination file '" << dest << "' already exists." << std::endl;
        return;
    }

    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        std::cerr << "Error: Source file '" << src << "' does not exist or cannot be opened." << std::endl;
        return;
    }

    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        close(src_fd);
        std::cerr << "Error: Destination file '" << dest << "' cannot be opened or created." << std::endl;
        return;
    }

    struct stat file_stat;
    if (fstat(src_fd, &file_stat) < 0) {
        std::cerr << "Error: Cannot get size of source file '" << src << "'." << std::endl;
        close(src_fd);
        close(dest_fd);
        return;
    }
    size_t total_size = file_stat.st_size;
    size_t total_copied = 0;

    char buffer[BUFFER_SIZE];

    std::cout << "Copying: " << src << " -> " << dest << std::endl;
    std::cout << "Progress: [";

    while (true) {
        ssize_t bytes_read = read(src_fd, buffer, BUFFER_SIZE);
        if (bytes_read == 0) break;
        if (bytes_read < 0) {
            std::cerr << "Error: Failed to read from source file '" << src << "'." << std::endl;
            break;
        }

        ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            std::cerr << "Error: Failed to write to destination file '" << dest << "'." << std::endl;
            break;
        }

        total_copied += bytes_read;
        print_progress(total_copied, total_size);
    }

    std::cout << "\rProgress: [";
    for (size_t i = 0; i < PROGRESS_BAR_SIZE; ++i) std::cout << PROGRESS_BAR;
    std::cout << "] 100%" << std::endl;

    close(src_fd);
    close(dest_fd);
}


/*       MOVE FILE       */

void move_file(const char *src, const char *dest) {
    if (rename(src, dest) < 0) {
        std::cerr << "Error moving file from " << src << " to " << dest << std::endl;
    }
}


/*       FILE INFO       */

void print_permission_info(mode_t mode) {
    int owner_mode = (mode >> 6) & 0x7;
    int group_mode = (mode >> 3) & 0x7;
    int other_mode = mode & 0x7;

    std::cout << MODES[owner_mode] << MODES[group_mode] << MODES[other_mode];
}

void print_file_info(const char *file) {
    struct stat file_stat;

    if (stat(file, &file_stat) < 0) {
        std::cerr << "Error getting file info: " << file << std::endl;
        return;
    }

    std::cout << "File: " << file << std::endl;
    std::cout << "Size: " << file_stat.st_size << " bytes" << std::endl;
    std::cout << "Permissions: ";
    print_permission_info(file_stat.st_mode);
    std::cout << "\nLast modified: " << std::ctime(&file_stat.st_mtime) << std::endl;

}


/*         CHMOD         */

bool parse_mode(const char *mode_str, mode_t &mode) {
    if (isdigit(mode_str[0])) {
        mode = strtol(mode_str, nullptr, 8);
        return true;
    }

    if (strlen(mode_str) != 9) return false;

    mode = 0;

    for (int i = 0; i < 3; ++i) {
        const char *group = &mode_str[i*3];
        mode_t group_mode = 0;

        for (int j = 0; j < 8; ++j) 
            if (!strncmp(group, MODES[j], 3)) {
                group_mode = j;
                break;
            }
        
        mode = (mode << 3) | group_mode;
    }

    return true;

}

void change_file_mode(const char *file, const char *mode_str) {
    mode_t mode;
    if (!parse_mode(mode_str, mode)) {
        std::cerr << "Error: Failed to change permissions for file '" << file << "'." << std::endl;
        return;
    }
    if (chmod(file, mode) < 0) {
        std::cerr << "Error changing file permissions: " << file << std::endl;
        return;
    }
    std::cout << "Permissions for file '" << file << "' changed to ";
    print_permission_info(mode);
    std::cout << " (" << std::oct << mode << ")." << std::endl;
}
