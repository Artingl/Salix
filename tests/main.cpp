/*
 * Simple salix tester. Probably will be rewritten,
 * because it is a mess.
 * */

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <string>
#include <sstream>

#include "test.h"
#include "fs.h"
#include "Logger.h"

struct ExecResult {
    std::string result;
    int code;
};

ExecResult exec(const char* cmd) {
    std::array<char, 128> buffer;
    ExecResult result;
    FILE* p = popen(cmd, "r");

    while (fgets(buffer.data(), buffer.size(), p) != nullptr) {
        result.result += buffer.data();
    }

    result.code = pclose(p);

    return result;
}

std::vector<Test> prepareTests() {
    std::vector<Test> tests;

    tests.push_back({ __LINE__, false, "Checking Lexer", "", R"(
            fn

            MAInnfheyyyWWWW(

                       )        -> int {
}
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Lexer", "", R"(
            fn

            aasFFFEEE(

                       )        -> str {    return 33.222.111;
}
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Lexer", "", R"(
            fn

            fdfsfssdsdiii(
ff: int, aaa:www
                       )        -> void {    return 33.222;
}
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Parser", "", R"(
            fn main() -> int {}
            fn HelloWorld() -> void {}
            fn help(s: int) -> str {  }
            fn help(s: int,args: str) -> float {  }
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Parser", "", R"(
            ;;;;;;;fn main() -> int { ;;;;;;;;;; };;;;
            fn test() -> int {
                {}{}{}{}{}{}{}{}
{{{{{{{{}}}}}}}}
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> {}
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main(a:) -> int {}
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main(a: int,) -> {}
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main( -> {}
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> {}
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {{}
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Parser", "", R"(
            fn main() -> int {
                return 2322;
            }
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Parser", "", R"(
            fn TEST() -> boolean { return true + 1; }

            fn ANOTHER(val: int) -> float { return val + 222 * 111 - 444; }

            fn main() -> int {
                return TEST() - 22 * ANOTHER();
            }
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int
            }
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int = 22232;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int = 111
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test int;
            }
        )"
                    });

    tests.push_back({ __LINE__, false, "Checking Parser", "", R"(
            fn isTrue(a: int, t: int) -> int { return a * t; }

            fn main() -> int {
               let test: int = isTrue(434331, 33) * 2122 + 22;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn isTrue(a: int, t: int) -> int { return a * t; }

            fn main() -> int {
               let test: int = isTrue(434331, 33) * 2122 + ;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int = 433 *;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int = 21312321 / 1 - ;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int = 21312321 / 1 - 322 - - 1;
            }
        )"
                    });

    tests.push_back({ __LINE__, true, "Checking Parser", "", R"(
            fn main() -> int {
               let test: int = 21312321 / 1 - 43232 * * 23123312;
            }
        )"
                    });

    return tests;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        io::debug_print(io::CRITICAL, "You should provide Salix compiler as the first argument!");
        return 1;
    }

    io::debug_print(io::INFO, "Loading tests...");

    std::string currentStage;
    size_t unPassed = 0;
    size_t tests;

    for (const auto& a : prepareTests()) {
        if (currentStage != a.stage) {
            io::debug_print(io::INFO, "Stage: %s", a.stage.c_str());
            currentStage = a.stage;
        }

        io::writeFile(a.code, "test.slx");
        ExecResult result = exec(("./" + std::string(argv[1]) + " test.slx " + a.compilerFlags).c_str());

        if (result.code == 0 && a.shouldBeError || result.code && !a.shouldBeError) {
            printf("\033[31;1;1m[-] Test #%zu: Unpassed on stage %s at line %zu! Compiler output:\n", tests++, a.stage.c_str(), a.id);

            if (result.result.empty()) {
                printf("\tNothing.\n");
            }

            std::stringstream input( result.result );
            std::string line;

            while (std::getline(input, line)) {
                printf("\t%s\n", line.c_str());
            }

            ++unPassed;
        }
        else {
            printf("\033[34;1;1m[+] Test #%zu: Passed!\n", tests++);
        }
    }

    if (unPassed > 0) {
        printf("\033[31;1;1m[-] %zu unpassed test%s\n", unPassed, (unPassed > 1 ? "s!" : "!"));
    }
    else {
        printf("\033[34;1;1m[+] All tests passed!\n");
    }

    system("rm test.slx");

    return 0;
}
