#include <stdio.h>
const char* get_language_name(int choice) {
    switch (choice) {
        case 1: return "C";
        case 2: return "C++";
        case 3: return "Java";
        case 4: return "Go";
        case 5: return "Python";
        case 6: return "Rust";
        case 7: return "JavaScript";
        case 8: return "Ruby";
        case 9: return "Swift";
        case 10: return "Kotlin";
        default: return "Invalid choice";
    }
}

const char* language_menu() {
    int choice;

    // Prompt user for input
    printf("Enter a number between 1 and 10 to select a programming language:\n");
    printf("1: C\n");
    printf("2: C++\n");
    printf("3: Java\n");
    printf("4: Go\n");
    printf("5: Python\n");
    printf("6: Rust\n");
    printf("7: JavaScript\n");
    printf("8: Ruby\n");
    printf("9: Swift\n");
    printf("10: Kotlin\n");
    scanf("%d", &choice);

    // Get and print the language name based on user choice
    const char* selected_language = get_language_name(choice);
    printf("Selected language: %s\n", selected_language);

    return selected_language;
}
