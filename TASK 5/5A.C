#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* --- Manual implementations (no string.h) --- */

int manual_strlen(const char s[]) {
    int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

void manual_strcpy(char dest[], const char src[]) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int manual_strcmp(const char a[], const char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if ((unsigned char)a[i] != (unsigned char)b[i]) {
            return (unsigned char)a[i] - (unsigned char)b[i];
        }
        i++;
    }
    return (unsigned char)a[i] - (unsigned char)b[i];
}

int manual_word_count(const char s[]) {
    int count = 0;
    int inside = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (isspace((unsigned char)s[i])) {
            inside = 0;
        } else if (!inside) {
            inside = 1;
            count++;
        }
    }
    return count;
}

void manual_to_upper(const char src[], char dst[]) {
    int i = 0;
    while (src[i] != '\0') {
        unsigned char ch = (unsigned char)src[i];
        if (ch >= 'a' && ch <= 'z') {
            dst[i] = (char)(ch - ('a' - 'A'));
        } else {
            dst[i] = (char)ch;
        }
        i++;
    }
    dst[i] = '\0';
}

void manual_reverse(const char src[], char dst[]) {
    int len = manual_strlen(src);
    for (int i = 0; i < len; i++) {
        dst[i] = src[len - 1 - i];
    }
    dst[len] = '\0';
}

int manual_is_palindrome(const char s[]) {
    int left = 0;
    int right = manual_strlen(s) - 1;
    while (left < right) {
        while (left < right && !isalnum((unsigned char)s[left])) {
            left++;
        }
        while (left < right && !isalnum((unsigned char)s[right])) {
            right--;
        }
        if (left < right) {
            unsigned char c1 = (unsigned char)s[left];
            unsigned char c2 = (unsigned char)s[right];
            if (c1 >= 'A' && c1 <= 'Z') c1 = (unsigned char)(c1 + ('a' - 'A'));
            if (c2 >= 'A' && c2 <= 'Z') c2 = (unsigned char)(c2 + ('a' - 'A'));
            if (c1 != c2) {
                return 0;
            }
            left++;
            right--;
        }
    }
    return 1;
}

/* --- <string.h> & <ctype.h> versions --- */

int lib_word_count(const char s[]) {
    int count = 0, inside = 0;
    size_t n = strlen(s);
    for (size_t i = 0; i < n; i++) {
        if (isspace((unsigned char)s[i])) {
            inside = 0;
        } else if (!inside) {
            inside = 1;
            count++;
        }
    }
    return count;
}

void lib_to_upper(const char src[], char dst[]) {
    size_t n = strlen(src);
    for (size_t i = 0; i < n; i++) {
        dst[i] = (char)toupper((unsigned char)src[i]);
    }
    dst[n] = '\0';
}

void lib_reverse(const char src[], char dst[]) {
    size_t n = strlen(src);
    for (size_t i = 0; i < n; i++) {
        dst[i] = src[n - 1 - i];
    }
    dst[n] = '\0';
}

int lib_is_palindrome(const char s[]) {
    int left = 0, right = (int)strlen(s) - 1;
    while (left < right) {
        while (left < right && !isalnum((unsigned char)s[left])) left++;
        while (left < right && !isalnum((unsigned char)s[right])) right--;
        if (left < right &&
            tolower((unsigned char)s[left]) != tolower((unsigned char)s[right])) {
            return 0;
        }
        left++;
        right--;
    }
    return 1;
}

int main(void) {
    const char *samples[] = {
        "  Meet at  the library at 5 PM! ",
        "Madam, I'm Adam",
        "",
        "   \t  "
    };

    for (int i = 0; i < 4; i++) {
        char m_up[128], m_rev[128], l_up[128], l_rev[128];
        manual_to_upper(samples[i], m_up);
        manual_reverse(samples[i], m_rev);
        lib_to_upper(samples[i], l_up);
        lib_reverse(samples[i], l_rev);

        printf("Input: \"%s\"\n", samples[i]);
        printf("  Manual -> words=%d, upper=\"%s\", rev=\"%s\", pal=%d\n",
               manual_word_count(samples[i]), m_up, m_rev, manual_is_palindrome(samples[i]));
        printf("  Lib    -> words=%d, upper=\"%s\", rev=\"%s\", pal=%d, same_up=%d\n",
               lib_word_count(samples[i]), l_up, l_rev, lib_is_palindrome(samples[i]),
               manual_strcmp(m_up, l_up) == 0);
    }

    /* Safe bounded copy demonstration using snprintf / fgets */
    char small_buf[12];
    const char *long_sms = "This SMS message is much longer than eleven characters";
    snprintf(small_buf, sizeof(small_buf), "%s", long_sms);
    printf("\nSafe truncated copy (size 12): \"%s\"\n", small_buf);

    char line_buf[64];
    printf("Enter a short feedback line (or press Enter): ");
    if (fgets(line_buf, sizeof(line_buf), stdin) != NULL) {
        line_buf[strcspn(line_buf, "\n")] = '\0';
        printf("Read safely: \"%s\" (words=%d)\n", line_buf, lib_word_count(line_buf));
    }
    return 0;
}
