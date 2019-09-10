/**
 * Utility functions to copy-paste into mucs-submit.cpp for debuggig.
 */

// class ExecArgs {
//
// public:

    // Overload insertion operator for ExecArgs with output streams
    friend ostream& operator<<(ostream& os, ExecArgs& ea) {
        if (ea._argc < 0 || ea._argv == nullptr)
            return os;

        auto contains_whitespace = [](char *s) {
            for (; *s != '\0'; s++) {
                if (*s == ' ' || *s == '\t' || *s == '\n')
                    return true;
            }
            return false;
        };
        auto wrap = [&](char *s) {
            return contains_whitespace(s)
                ? '\'' + string(s) + '\''
                : s;
        };

        if (ea._argc > 0)
            os << wrap(ea._argv[0]);
        for (int i=1; i<ea._argc-1; i++)
            os << ' ' << wrap(ea._argv[i]);

        return os;
    }

// };
