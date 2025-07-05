#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
// Your autosave function
void autosave() {
    char current_branch[128];
    FILE *fp;

    // Step 1: Get current branch
    fp = popen("git rev-parse --abbrev-ref HEAD", "r");
    if (!fp) {
        perror("Failed to get branch");
        return;
    }
    fgets(current_branch, sizeof(current_branch), fp);
    pclose(fp);

    // Remove newline
    current_branch[strcspn(current_branch, "\n")] = 0;

    // Step 2: Stash changes (quiet)
    system("git stash push -q --keep-index");

    // Step 3: Switch to or create autosave branch
    if (system("git rev-parse --verify kpm-auto-save > /dev/null 2>&1") != 0) {
        system("git checkout -b kpm-auto-save");
    } else {
        system("git checkout kpm-auto-save");
    }

    // Step 4: Apply stashed changes (quiet)
    system("git stash pop -q");

    // Step 5: Stage everything
    system("git add .");

    // Step 6: Commit with timestamp
    char commit_cmd[512];
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strcspn(timestamp, "\n")] = 0;

    snprintf(commit_cmd, sizeof(commit_cmd),
             "git commit -m \"[autosave] %s\" --allow-empty", timestamp);
    system(commit_cmd);

    // Step 7: Switch back to original branch
    char checkout_cmd[256];
    snprintf(checkout_cmd, sizeof(checkout_cmd), "git checkout %s", current_branch);
    system(checkout_cmd);
}
// Optional signal handler to allow clean exit
void handle_signal(int sig) {
    printf("Stopping autosave loop...\n");
    exit(0);
}

int main() {
    // Fork to run in the background
    // pid_t pid = fork();

    // if (pid < 0) {
    //     perror("Fork failed");
    //     return 1;
    // } else if (pid > 0) {
    //     // Parent process exits so child runs in background
    //     printf("Autosave daemon started (PID %d)\n", pid);
    //     return 0;
    // }

    // // Child continues as daemon
    // signal(SIGINT, handle_signal);
    // signal(SIGTERM, handle_signal);

    while (1) {
        autosave();
        sleep(10); // 180 seconds = 3 minutes
    }

    return 0;
}
