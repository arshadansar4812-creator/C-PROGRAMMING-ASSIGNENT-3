#include <ctype.h>
#include <stdio.h>

/* Counts words separated by one or more spaces, without using string.h */
int word_count(const char s[]) {
    int count = 0, inside = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == ' ') {
            inside = 0;
        } else if (!inside) {
            inside = 1;
            count++;
        }
    }
    return count;
}

/* Returns 1 if s reads the same both ways, ignoring case and non-letters. */
int is_palindrome(const char s[]) {
    int left = 0, right = 0;
    while (s[right] != '\0') right++;
    right--;
    while (left < right) {
        while (left < right && !isalpha((unsigned char)s[left])) {
            left++;
        }
        while (left < right && !isalpha((unsigned char)s[right])) {
            right--;
        }
        if (left < right) {
            char a = (char)tolower((unsigned char)s[left]);
            char b = (char)tolower((unsigned char)s[right]);
            if (a != b) {
                return 0;
            }
            left++;
            right--;
        }
    }
    return 1;
}

int main(void) {
    printf("Words: %d\n", word_count("  Meet at  the library at 5 PM "));
    printf("%d %d\n", is_palindrome("Madam, I'm Adam"), is_palindrome("Hello"));

    /* Test with a tab character between words */
    printf("Words with tab (s[i] == ' ' only): %d\n", word_count("Hostel\tRoom\t101"));
    return 0;
}
