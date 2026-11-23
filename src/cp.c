#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 131072
#define PATH_MAX_LEN 4096

// helper: extract just the filename from a path (e.g., "a/b/file.txt" -> "file.txt")
const char *get_basename(const char *path) {
    const char *slash = strrchr(path, '/');
    return (slash != NULL) ? slash + 1 : path;
}

// helper: strip trailing '/' in-place (preserving root "/")
void strip_trailing_slash(char *path) {
    size_t len = strlen(path);
    while (len > 1 && path[len - 1] == '/') {
        path[len - 1] = '\0';
        len--;
    }
}

// copies a single regular file from src_path to dst_path
int copy_file(const char *prog_name, const char *src_path, const char *dst_path) {
    struct stat src_stat, dst_stat;

    if (stat(src_path, &src_stat) != 0) {
        perror(src_path);
        return 1;
    }

    // Prevent overwriting a file with itself
    if (stat(dst_path, &dst_stat) == 0) {
        if (src_stat.st_dev == dst_stat.st_dev && src_stat.st_ino == dst_stat.st_ino) {
            fprintf(stderr, "%s: '%s' and '%s' are the same file\n", prog_name, src_path, dst_path);
            return 1;
        }
    }

    FILE *src = fopen(src_path, "rb");
    if (src == NULL) {
        perror(src_path);
        return 1;
    }

    FILE *dst = fopen(dst_path, "wb");
    if (dst == NULL) {
        perror(dst_path);
        fclose(src);
        return 1;
    }

    unsigned char buf[BUFFER_SIZE];
    size_t bytes_read = 0;
    int status = 0;

    while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0) {
        if (fwrite(buf, 1, bytes_read, dst) != bytes_read) {
            perror("Write failed");
            status = 1;
            break;
        }
    }

    if (ferror(src)) {
        perror("Read failed");
        status = 1;
    }

    fclose(dst);
    fclose(src);

    // preserve source file permissions on the destination
    chmod(dst_path, src_stat.st_mode & 07777);

    return status;
}

// recursively copies a directory and all its contents
int copy_dir(const char *prog_name, const char *src_dir, const char *dst_dir) {
    struct stat src_stat, dst_stat;

    if (stat(src_dir, &src_stat) != 0) {
        perror(src_dir);
        return 1;
    }

    // check if destination already exists
    if (stat(dst_dir, &dst_stat) == 0) {
        if (src_stat.st_dev == dst_stat.st_dev && src_stat.st_ino == dst_stat.st_ino) {
            fprintf(stderr, "%s: '%s' and '%s' are the same directory\n", prog_name, src_dir, dst_dir);
            return 1;
        }
        if (!S_ISDIR(dst_stat.st_mode)) {
            fprintf(stderr, "%s: cannot overwrite non-directory '%s' with directory '%s'\n",
                    prog_name, dst_dir, src_dir);
            return 1;
        }
    } else {
        // create destination directory with the same permissions as source
        if (mkdir(dst_dir, src_stat.st_mode & 07777) != 0) {
            perror(dst_dir);
            return 1;
        }
    }

    DIR *dir = opendir(src_dir);
    if (dir == NULL) {
        perror(src_dir);
        return 1;
    }

    struct dirent *entry;
    int status = 0;

    while ((entry = readdir(dir)) != NULL) {
        // Skip current (.) and parent (..) directory entries to avoid infinite recursion
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char next_src[PATH_MAX_LEN];
        char next_dst[PATH_MAX_LEN];
        snprintf(next_src, sizeof(next_src), "%s/%s", src_dir, entry->d_name);
        snprintf(next_dst, sizeof(next_dst), "%s/%s", dst_dir, entry->d_name);

        struct stat entry_stat;
        if (stat(next_src, &entry_stat) != 0) {
            perror(next_src);
            status = 1;
            continue;
        }

        if (S_ISDIR(entry_stat.st_mode)) {
            if (copy_dir(prog_name, next_src, next_dst) != 0) {
                status = 1;
            }
        } else {
            if (copy_file(prog_name, next_src, next_dst) != 0) {
                status = 1;
            }
        }
    }

    closedir(dir);
    return status;
}

int main(int argc, char *argv[]) {
    int recursive = 0;
    int first_src_idx = 1;

    // Check if -r or -R flag was passed as the first argument
    if (argc > 1 && (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "-R") == 0)) {
        recursive = 1;
        first_src_idx = 2;
    }

    int num_paths = argc - first_src_idx;
    if (num_paths < 2) {
        fprintf(stderr, "%s: Missing file argument\n", argv[0]);
        fprintf(stderr, "Usage: %s [-r] <src>... <dst>\n", argv[0]);
        return 1;
    }

    char dst_clean[PATH_MAX_LEN];
    snprintf(dst_clean, sizeof(dst_clean), "%s", argv[argc - 1]);
    strip_trailing_slash(dst_clean);

    struct stat dst_stat;
    int dst_exists = (stat(dst_clean, &dst_stat) == 0);
    int dst_is_dir = (dst_exists && S_ISDIR(dst_stat.st_mode));

    // If copying multiple items, the destination MUST be an existing directory
    if (num_paths > 2 && !dst_is_dir) {
        fprintf(stderr, "%s: target '%s' is not a directory\n", argv[0], dst_clean);
        return 1;
    }

    int overall_status = 0;

    for (int i = first_src_idx; i < argc - 1; i++) {
        char src_clean[PATH_MAX_LEN];
        snprintf(src_clean, sizeof(src_clean), "%s", argv[i]);
        strip_trailing_slash(src_clean);

        struct stat src_stat;
        if (stat(src_clean, &src_stat) != 0) {
            perror(src_clean);
            overall_status = 1;
            continue;
        }

        // Build final destination path
        char final_dst[PATH_MAX_LEN];
        if (dst_is_dir) {
            snprintf(final_dst, sizeof(final_dst), "%s/%s", dst_clean, get_basename(src_clean));
        } else {
            snprintf(final_dst, sizeof(final_dst), "%s", dst_clean);
        }

        if (S_ISDIR(src_stat.st_mode)) {
            if (!recursive) {
                fprintf(stderr, "%s: -r not specified; omitting directory '%s'\n", argv[0], src_clean);
                overall_status = 1;
                continue;
            }
            if (copy_dir(argv[0], src_clean, final_dst) != 0) {
                overall_status = 1;
            }
        } else {
            if (copy_file(argv[0], src_clean, final_dst) != 0) {
                overall_status = 1;
            }
        }
    }

    return overall_status;
}
