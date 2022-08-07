#include <sol/sol.hpp>

#ifdef __unix__
    #define isUnix() true
    #define isWindows() false
#elif defined(_WIN32)
    #define isUnix() false
    #define isWindows() true
#else
    #error "Unsupported system"
#endif

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "%s: fatal error: input is empty\n", argv[0]);
        exit(-1);
    }

    sol::state state;
    state.open_libraries();

    sol::table args = state.create_table();

    for (int i = 2; i < argc; ++i)
        args[i - 1] = argv[i];

    state["arg"] = args;

    state["isUnix"] = sol::readonly_property(sol::var(isUnix()));
    state["isWindows"] = sol::readonly_property(sol::var(isWindows()));
    
    state.script_file(argv[1]);

    return 0;
}
